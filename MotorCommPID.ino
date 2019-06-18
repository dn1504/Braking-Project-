#ifndef _ARDUINO_SERIALCOMM_INO_
#define _ARDUINO_SERIALCOMM_INO_

#include "MessageDefinition.h"

#define MAX_LIST_DATA 15
// Mendefinisikan variable global pada program
String Ahoy;
float RealData;
extern String Ahoy;
extern float RealData;

// Mulai konstruksi data dari serial communication
struct ListData
{
  ListData() { }
  int n = 0;
  String content[MAX_LIST_DATA];
};

void Parse(ListData* output, String strInput)
{
    int loc[5]      = {-1, -1, -1, -1, -1};
    int found       = 0;
    int maxIndex    = strInput.length();

    // Penanda paling awal
    loc[0] = -1;
    
    // Setiap nemu koma, taruh index nya
    for (int i = 0; i < maxIndex; i++) 
    {
        if (strInput.charAt(i) == ',') 
        {
          found++;
          loc[found] = i;
        }
    }
    loc[found+1] = maxIndex;

    output->n = found+1;
    
    // Give the result
    for (int j = 0; j < (found+1); j++)
    {
      output->content[j] = strInput.substring(loc[j]+1,loc[j+1]);
    }
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
  // Format message harus diakhiri CR+LF : "$,DATA\r\n";
  // ...............
  // ...............

//  Serial.println(str);
    
  // Sering banget salah disini, kelupaan ngirim \r\n di messagenya, PASTIKAN !!!
  if (str == MSG_MOTOR_START_RESET) 
  { 
      Write(MSG_MOTOR_READY); 
  } 
  else 
  {
      ListData listData;
      Parse(&listData, str); // "$,DATA\r\n" --> DATA = DATA[0], DATA[1], DATA[2], ... * bergantung pada listData.n 
      
      // Minimal di message kita ada 3 entry, 
      // listData.content[0] = "$"
      // listData.content[1] = "DATA" -> DATA[0] adalah jenis perintah, bisa diakses dengan cara listData.content[1]
      // listData.content[terakhir] = "\r\n" -> ini entry terakhir
      if (listData.n > 2) 
      { 
          if (listData.content[1] == MOTOR_SET_VELOCITY)
          {
//Serial.println("Perintah : sebesar = " + listData.content[2]); (ini kalo mau liat string hasilnya)

              // Mendeteksi nilai tiap variable 
              Ahoy = listData.content[2]; // ini masih string, harus dikonversi jadi float 
              RealData = Ahoy.toFloat(); //Seharusnya data sudah jadi bentuk float 
              
          }
      } 
  }
}

#endif
