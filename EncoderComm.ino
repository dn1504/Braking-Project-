#ifndef _ARDUINO_SERIALCOMM_INO_
#define _ARDUINO_SERIALCOMM_INO_

#include "MessageDefinition.h"

// Struct untuk settingan pin tiap encoder
struct rotary_t 
{
  byte pinA; // kabel pin A
  byte pinB; // kabel pin B
  int count; 
};

// Settingan untuk tiap encoder
rotary_t encoder[] = 
{ 
  {2,3}, // encoder[0].pinA, encoder[0].pinB
  {6,7}, // encoder[1].pinA, encoder[1].pinB
};

// Menyatakan jumlah tick/putaran (sesuai spec encoder)
#define NUMTICKREV 100

// Menyatakan resolusi encoder (rad/tick)
#define RESOLUTION (2*PI)/NUMTICKREV;

// Menyatakan radius roda
#define WHEELRADIUS 0.2 // meter

// Menyatakan panjang poros roda (kanan - kiri)
#define WHEELBASELINE 0.265 // meter

// Menyatakan berapa jumlah encoder yang terpasang
#define NUMENCODERS (sizeof(encoder)/sizeof(encoder[0]))

// Konversi millisecond to second
#define MILLI2SECOND 0.001

// Serial baud rate
#define BAUDRATE 4800 

// Variabel untuk menyimpan state semua encoder
volatile byte state_ISR[NUMENCODERS];
volatile int8_t count_ISR[NUMENCODERS];

// Variabel untuk menyimpan data sebelumnya
unsigned long tSebelum = 0; // [mSec]
double thetaEncASebelum = 0; // [rad]
double thetaEncBSebelum = 0; // [rad]
double xOdo = 0; // [meter]
double yOdo = 0; // [meter]
double thetaOdo = 0; // [rad]

// Untuk serial communication
String strEncoderLastData = ""; // Ingat format data harus ->>> "$,DATA,\r\n"; kalau ngga nanti yang python nya ga bs baca

bool bInitialization = false;

int8_t readEncoder(byte i)
{ 
  // this function is called within timer interrupt to read one encoder!
  int8_t result = 0;
  byte state = state_ISR[i];
  state = state << 2 | (byte)digitalRead(encoder[i].pinA) << 1 | (byte)digitalRead(encoder[i].pinB);
  state = state & 0xF;  // keep only the lower 4 bits
  /* // next two lines would be code to read 'quarter steps'
    if (state==0b0001 || state==0b0111 || state==0b1110 || state==0b1000) result= -1;
    else if (state==0b0010 || state==0b1011 || state==0b1101 || state==0b0100) result= 1;
  */
  // next two lines is code to read 'full steps'
  if (state == 0b0001) result = -1;
  else if (state == 0b0010) result = 1;
  state_ISR[i] = state;
  return result;
}

void startTimer2()  // start TIMER2 interrupts
{
  noInterrupts();
  // Timer 2 CTC mode
  TCCR2B = (1 << WGM22) | (1 << CS22)  | (1 << CS20);
  TCCR2A = (1 << WGM21);
  OCR2A = 124;   // 249==500,  124==1000 interrupts per second
  // 63 ==2000,  31==4000
  // 15 ==8000,   7==16000
  TIMSK2 = (1 << OCIE2A); // enable Timer 2 interrupts
  interrupts();
}

void stopTimer2() // stop TIMER2 interrupts
{
  noInterrupts();
  TIMSK2 = 0;
  interrupts();
}

ISR(TIMER2_COMPA_vect)  // handling of TIMER2 interrupts
{
  for (int i = 0; i < NUMENCODERS; i++)
  {
    count_ISR[i] += readEncoder(i);
  }
}

// Memulai encoder
void beginEncoders()
{ 
  // Untuk tiap encoder dimulai
  for (int i = 0; i < NUMENCODERS; i++)
  {
    pinMode(encoder[i].pinA, INPUT_PULLUP);
    pinMode(encoder[i].pinB, INPUT_PULLUP);
    readEncoder(i); // Initialize start condition
  }

  // Mulai timer
  startTimer2();
}

// Mendapatkan data dari encoder
boolean updateEncoders()
{ 
  // Untuk tiap encoder dilakukan baca data
  boolean changeState = false;
  for (int i = 0; i < NUMENCODERS; i++)
  {

    // Jika ada perubahan
    if (count_ISR[i] != 0)
    {
      changeState = true;
      noInterrupts();
      encoder[i].count += count_ISR[i];
      count_ISR[i] = 0;
      interrupts();
    }
  }

  // Kalau tidak ada update, maka yang di return = false
  return changeState;
}

// Menampilkan data dari encoder pertama -> encoder[0] -> encoderA
volatile double printEncodersA()
{
  double thetaSekarang  = ((double)encoder[0].count)*RESOLUTION 
  double deltaTheta     = thetaSekarang - thetaEncASebelum;
  thetaEncASebelum = thetaSekarang;
  return deltaTheta;
}

// Menampilkan data dari encoder kedua -> encoder[1] -> encoderB
double printEncodersB() 
{
  double thetaSekarang  = ((double)encoder[1].count)*RESOLUTION 
  double deltaTheta     = thetaSekarang - thetaEncBSebelum;
  thetaEncBSebelum      = thetaSekarang;
  return deltaTheta;
}

void ResetEncoderData()
{
    xOdo = 0; // [meter]
    yOdo = 0; // [meter]
    thetaOdo = 0; // [rad]
    strEncoderLastData = ""; 
    Serial.println("Encoder reset ...");
}

void UpdateEncoderData()
{
    if (!bInitialization)
    {
      // Mulai encoder
      beginEncoders();
      tSebelum = millis();
      bInitialization = true;
    }

    double omegaR, omegaL, rpmR, rpmL, VR, VL;
    omegaR = omegaL = rpmR = rpmL = VR = VL = 0.0;

    // Delta time
    unsigned long tSekarang = millis();
    
    bool bUpdateExist = updateEncoders();
    if (bUpdateExist) 
    {
      double dT = MILLI2SECOND*(tSekarang - tSebelum); // [sec]
      
      
      // Kecepatan sudut tiap roda, omega = deltaTheta / deltaTime
      omegaR = printEncodersA()/dT; // [rad/sec]
      omegaL = printEncodersB()/dT; // [rad/sec]

      // Kecepatan linear tiap roda, V = r . omega 
      VR = WHEELRADIUS*omegaR; // [m/sec]
      VL = WHEELRADIUS*omegaL; // [m/sec]

      // Kecepatan titik tengah baseline roda
      double V = 0.5*(VR + VL);

      // ICC (Instaneous Center of Curvature) dalam bentuk :
      // omegaICC -> kecepatan sudut terhadap titik tengah ICC dan 
      // radiusICC -> jari-jari terhadap titik tengah ICC
      double radiusICC  = WHEELBASELINE*((VR+VL)/(VR-VL));
      float omegaICC    = (VR - VL)/WHEELBASELINE;

      // Perbedaan kecepatan roda kanan dan kiri
      double diffVelocity = abs(omegaR - omegaL);

      // Update x dan y odometer
      if (diffVelocity == 0.0) // if (diffVelocity < 0.000000000001) // Artinya sedang jalan lurus
      {
         double D = V*dT;
         xOdo = xOdo + cos(thetaOdo)*D; // X' = X + dX
         yOdo = yOdo + sin(thetaOdo)*D; // Y' = Y + dY
      }
      else // Sedang berbelok
      { 
        double RSin = radiusICC*sin(thetaOdo);
        double RCos = radiusICC*cos(thetaOdo);        
        xOdo = cos(omegaICC)*(RSin) - sin(omegaICC)*(-RCos) + (xOdo-RSin);
        yOdo = sin(omegaICC)*(RSin) + cos(omegaICC)*(-RCos) + (yOdo+RCos);
      }

      // Update theta odometer
      thetaOdo = thetaOdo + omegaICC*dT; // THETA' = THETA + dTHETA

      // Dalam satuan RPM
      rpmR = (((omegaR*60)/(2*PI))/5); // [rpm]
      rpmL = (((omegaL*60)/(2*PI))/5); // [rpm]
  }
  // Kirim informasi dalam string
  String msg = "$";
  msg += String("," + String(ENCODER_DATA)); 
  msg += String("," + String(tSekarang));
  msg += String("," + String(omegaR,4));
  msg += String("," + String(omegaL,4));
  msg += String("," + String(rpmR,4));
  msg += String("," + String(rpmL,4));
  msg += String("," + String(VR,4));
  msg += String("," + String(VL,4));
  msg += String("," + String(xOdo,4));
  msg += String("," + String(yOdo,4));
  msg += String("," + String(thetaOdo,4));
  msg += ",\r\n";
  strEncoderLastData = msg;
  //delay (30); //Penyamaan kecepatan baca python dengan kecepatan kirim arduino  
  Serial.println(strEncoderLastData);
}

String CallBackWrite()
{
  // DISINI UNTUK KASUS DIMANA ARDUINO SECARA AKTIF KIRIM TERUS DATA
  // KALAU KASUS DIMANA ARDUINO KIRIM DATA BERDASARKAN INCOMMING MESSAGE
  // (Mekanisme REPLY ...) MAKA ITU LETAKNYA DI CALLBACK READ
  // Disini mulai implementasi kirim ke serial
  // Format message harus : "$,DATA,\r\n";
  // DATA = pesan yang kita kirim ke serial
  // ...............
  // ...............
  // ContohCallBackWrite();

  //return WriteEncoderData();
  return "$,AAAAAA,\r\n";
}

void Write(String str); // Biar seneng deklarasiin dulu

void CallBackRead(String str)
{
  // Disini mulai implementasi kirim ke serial
  // Format message harus diakhiri CR+LF : "$,DATA,\r\n";
  // ...............
  // ...............

  // Sering banget salah disini, kelupaan ngirim \r\n di messagenya, PASTIKAN !!!
  if (str == MSG_ENCODER_GET_DATA) 
  { 
    Write(strEncoderLastData); 
  }  
  else if (str == MSG_ENCODER_START_RESET  ) 
  { 
    ResetEncoderData(); 
    Write(MSG_ENCODER_READY);
  }
}

#endif
