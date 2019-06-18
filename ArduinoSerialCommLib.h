#ifndef _ARDUINO_SERIALCOMM_LIB_H_
#define _ARDUINO_SERIALCOMM_LIB_H_

#include <Arduino.h>
#include <HardwareSerial.h>
#include "SoftwareSerial.h"

#define WRITEONLY 0
#define READONLY 1
#define WRITEANDREAD 2

// Sarana untuk komunikasi utama
// SoftwareSerial sering datanya bercampur, lebih baik pakai HardwareSerial
// Misal menggunakan : Arduino Mega

//HardwareSerial* serialKomunikasi = NULL;
SoftwareSerial* serialKomunikasi = NULL;

// Baca serial
bool bFinishRead = false;
String rcvString;

// Mode
int mode = WRITEANDREAD; 
int baudRate = 4800;

// Deklarasi saja
void Initialization();
void CallBackRead(String str);
void ContohCallBackRead(String str);
String CallBackWrite();
String ContohCallBackWrite();


#endif
