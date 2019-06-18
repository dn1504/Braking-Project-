from serial_comm import SerialComm
import serial_comm_constant as co

class CommMotor(SerialComm) :

    _isMotorReady   = False;

    def Start(self) :
        
        # Start
        if self._Start() :
            
            print('[CommMotor] >> Telah mulai . . .');

        else :

            print('[CommMotor] >> Gagal mulai . . .');

    def IsReady(self) :
        return self._isMotorReady;

    def ConfirmMotorReady(self) :

        self.DataSent(co.MSG_MOTOR_START_RESET);
        print('[CommMotor] >> ' + 'Kirim command mulai ' + repr(co.MSG_MOTOR_START_RESET) + ' . . .');
        print('[CommMotor] >> ' + 'Menunggu motor siap . . .');

    def DataReceived(self, data):
        
        if len(data) > 0 :

            if data[0] == co.MOTOR_READY :
               self._isMotorReady = True;