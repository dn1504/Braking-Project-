from serial_comm import SerialComm
from serial_comm_intepreter import SerialCommIntepreter as itp
import serial_comm_constant as co
import serial_comm_motor

class CommEncoder(SerialComm) :

    _isEncoderReady = False;
    _serialMotor    = None;

    def SetSerialMotor(self, serMotor) :
        self._serialMotor = serMotor;

    def Start(self) :
        
        # Start
        if self._Start() :
            print('[CommEncoder] >> ' + 'Telah mulai . . .');

            self.DataSent(co.MSG_ENCODER_START_RESET);
            print('[CommEncoder] >> ' + 'Tunggu sebentar reset . . .');
            print('[CommEncoder] >> ' + 'Kirim command mulai ' + repr(co.MSG_ENCODER_START_RESET) + ' . . .');
            print('[CommEncoder] >> ' + 'Menunggu encoder siap . . .');
 
        else :
            print('[CommEncoder] >> Gagal mulai . . .');

    def DataReceived(self, data) :
        
        if len(data) > 0 :

            # Ini data yang masuk
            print('[CommEncoder] >> Data masuk ->>' + ''.join(data));

            if not self._isEncoderReady :
                self.DataSent(co.MSG_ENCODER_START_RESET);
                print('[CommEncoder] >> ' + 'Kirim command mulai ' + repr(co.MSG_ENCODER_START_RESET) + ' . . .');

            # Jika terima status ready dari encoder
            if data[0] == co.ENCODER_READY :
                self._isEncoderReady = True;

            # Jika data encoder 
            if data[0] == co.ENCODER_DATA :
                
                # Ceritanya disini sehabis dapat encoder data, kita kirim perintah 
                # ganti kecepatan ke motor
                if self._serialMotor != None :
                    #self._serialMotor.DataSent(itp.FormatMessageData(co.MOTOR_SET_VELOCITY, data[1]));
                    if self._serialMotor.IsReady() :
                        self._serialMotor.DataSent(itp.FormatMessageData(co.MOTOR_SET_VELOCITY, data[1]));

                    else :
                        self._serialMotor.ConfirmMotorReady();

            # Ini perintah kalau mau minta kirim lagi, tergantung arduinonya untuk device apa
            if self._isEncoderReady :
                self.DataSent(co.MSG_ENCODER_GET_DATA);