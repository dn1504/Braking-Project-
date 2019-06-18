#ifndef _ENCODER_SERIAL_INO_
#define _ENCODER_SERIAL_INO_

#include "lib\ArduinoSerialCommLib.h"
#include "EncoderImplementation.cpp"

// Penting untuk settingan
ArduinoSerialComm serialComm(&Serial2, BAUDRATE9600, WRITEONLY);
EncoderImplementation encoderImpl;

String WriteCallback()
{
    return ""; //encoderImpl.WriteEncoderData();
}

void ReadCallback(String str)
{
  //encoderImpl.ReadEncoderData(str);
}

void setup()
{
    serialComm.SetCallbackWrite(&WriteCallback); // Fungsi yang dipanggil kalau mau kirim data
    serialComm.SetCallbackRead(&ReadCallback); // Fungsi yang dipanggil kalau ada data masuk
    serialComm.Initialization(); // Mulai
}

void loop()
{
    serialComm.Running(); // Hanya butuh ini saja yang di looping
}

ISR(TIMER2_COMPA_vect)  // handling of TIMER2 interrupts
{
  encoderImpl.InnerISR();
}

#endif
