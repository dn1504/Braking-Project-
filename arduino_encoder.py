import serial_comm_constant as co
from serial_comm_intepreter import SerialCommIntepreter as itp
import serial
import time

ser = serial.Serial('COM29',9600, timeout=0.1);
print('[ENCODER] >> Mulai . . .');

if ser.is_open :

    print('[ENCODERs] >> Serial dibuka . . .');

    ser.write(co.MSG_ENCODER_READY.encode('utf-8'));
    print('[ENCODER] >> Mengirim status ready -> ' + repr(co.MSG_ENCODER_READY) + ' ke server . . .');

    print('[ENCODER] >> Menunggu command . . .');

    while True :
        
        # Message yang masuk
        d = ser.readline().decode('utf-8').split(','); # Format ->> $,DATA,\r\n

        if len(d) > 1 :

            if d[1] == co.ENCODER_GET_DATA :

                print('[ENCODER] >> Command masuk -> ' + repr(co.ENCODER_GET_DATA));
                dk = str(int(round(time.time() * 1000)));
                dk = itp.FormatMessageData(co.ENCODER_DATA, dk);
                print('[ENCODER] >> Reply ke server -> ' + repr(dk));
                ser.write(dk.encode('utf-8'));

            elif d[1] == co.ENCODER_START_RESET : 

                print('[ENCODER] >> Command masuk -> ' + repr(co.ENCODER_START_RESET));
                ser.write(co.MSG_ENCODER_READY.encode('utf-8'));