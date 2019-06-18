import serial
import serial_comm_constant as co

ser = serial.Serial('COM31',9600, timeout=0.1);
print('[MOTOR] >> Mulai . . .');

if ser.is_open :
    print('[MOTOR] >> Serial dibuka . . .');

    ser.write(co.MSG_MOTOR_READY.encode('utf-8'));
    print('[MOTOR] >> Mengirim status ready -> ' + repr(co.MSG_MOTOR_READY) + ' ke server . . .');
    print('[MOTOR] >> Menunggu command . . .');

    while True :
        
        d = ser.readline().decode('utf-8').split(','); # Format ->> $,DATA,\r\n
 
        if len(d) > 1 :

            if d[1] == co.MOTOR_START_RESET :
                ser.write(co.MSG_MOTOR_READY.encode('utf-8'));
                print('[MOTOR] >> Mengirim status ready -> ' + repr(co.MSG_MOTOR_READY) + ' ke server . . .');

            if d[1] == co.MOTOR_SET_VELOCITY:
                #print('[MOTOR] >> Command masuk -> ' + repr(co.MOTOR_SET_VELOCITY) + '...');
                print('[MOTOR] >> Set velocity di eksekusi ' + d[2] + ' ...');