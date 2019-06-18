#ifndef _ARDUINO_SERIALCOMM_LIB_CPP_
#define _ARDUINO_SERIALCOMM_LIB_CPP_

#include "ArduinoSerialCommLib.h"
#include "MessageDefinition.h"

void Write(String str)
{
   serialKomunikasi->flush();
   if (str.length() != 0) 
   {
        for (int i = 0; i < str.length(); i++)
        {
          serialKomunikasi->write(str.charAt(i));   // Push each char 1 by 1 on each loop pass
        } 
    }
}

void Initialization()
{
  // Ganti sesuai dengan settingan sekarang
  serialKomunikasi = new SoftwareSerial(10,11); // (RX, TX) >> RX yang nerima = terhubung ke TX nya pengirim
  //serialKomunikasi = &Serial3; 
  mode = READONLY; // WRITEONLY  READONLY  WRITEANDREAD; 
  baudRate = 4800;
}

// Mulai
void setup() 
{
    Initialization();
    
  	// Sarana untuk menampilkan di serial monitor arduino
  	Serial.begin(baudRate);
  
  	// Sarana untuk komunikasi utama
  	serialKomunikasi->begin(baudRate);

    // Bersihkan sebelumnya
    while (serialKomunikasi->read() > -1);

    // Kirim info ke PC kalau ready ...
    Write(MSG_ENCODER_READY);
}

void ThreadRead()
{
  // Baca dari pin tx rx bluetooth (dari SoftwareSerial)
  if (serialKomunikasi->available()) 
  {     
    int rcv = serialKomunikasi->read(); // Baca per char
 
    if (rcv != -1)
    {
        rcvString += (char)rcv;  // Masukkan ke array

        // Deteksi apakah CR+LF sudah sampai ? kalau iya maka penanda string sudah lengkap
        if (rcvString[rcvString.length()-1] == 0x0A) // Linefeed LF 
        {
          bFinishRead = rcvString[rcvString.length()-2] == 0x0D ? true : bFinishRead; 
        }
    }
  }

  // String sudah lengkap
  if (bFinishRead)
  {
     // Output
     CallBackRead(rcvString);

     // Reset
     rcvString = "";
     bFinishRead = false;
     return true;
  }
}

void ThreadWrite()
{
   String str = CallBackWrite();
   if (str != "") { Write(str); }
}

void ThreadWriteAndRead()
{
    // Jeda antara write dan read harus cukup dan ga bisa terlalu cepat (* mungkin keterbatasan arduino)
    // Misal kasus arduino -> PC -> arduino
    // Kalau kirim datanya dari arduino cepet banget, trus PC nya jg ngebalikinnya cepet banget, 
    // maka nanti arduino pas Read nya kaya nge-hang, oleh karena itu, kalau mode = WRITEANDREAD
    // si pengirim pesan (misal : PC) jangan terlfalu cepat, misal jeda 0.1 second
    
    ThreadWrite();
    ThreadRead();
}

String ContohCallBackWrite()
{
	String str;

  // DISINI UNTUK KASUS DIMANA ARDUINO SECARA AKTIF KIRIM TERUS DATA
  // KALAU KASUS DIMANA ARDUINO KIRIM DATA BERDASARKAN INCOMMING MESSAGE
  // (Mekanisme REPLY ...) MAKA ITU LETAKNYA DI CALLBACK READ
	// Disini mulai implementasi kirim ke serial
	// Format message harus : "$,DATA,\r\n";
	// DATA = pesan yang kita kirim ke serial
	// ...............
	// ...............

	str += "$,";
	str += String(millis());
	str += ",\r\n";
	return str;
}

void ContohCallBackRead(String str)
{
	// Disini mulai implementasi kirim ke serial
	// ...............
	// ...............

	Serial.println(str);
}

void UpdateEncoderData(); // Biar seneng deklarasiin dulu

// Main loop
void loop() 
{
    UpdateEncoderData();

    switch(mode)
    {
        case WRITEONLY :
            ThreadWrite();
            break;

        case READONLY :
            ThreadRead();
            break;

        case WRITEANDREAD :
            ThreadWriteAndRead();
            break;

        default :
            break;
    }    
}

#endif
