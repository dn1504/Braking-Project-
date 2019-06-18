import serial_comm_constant

class SerialCommIntepreter :

    @staticmethod
    def FormatMessage(str) : 
        return '$,' + str + ',\r\n';

    @staticmethod
    def FormatMessageData(code, data) :
        return SerialCommIntepreter.FormatMessage(code + ',' + data);