from serial_comm_encoder import CommEncoder
from serial_comm_motor import CommMotor

comArduinoMotor         = 'COM4';
comArduinoEncoder       = 'COM28';

MODE = '' #'CLIENT PYTHON'

if MODE == 'CLIENT PYTHON' :

    # Pakai virtual commport 
    comArduinoMotor     = 'COM32';
    comArduinoEncoder    = 'COM30';

# Buat komunikasi dengan arduino nya motor 
serMotor = CommMotor(9600,comArduinoMotor);
serMotor.Start();

# Buat komunikasi dengan arduino nya encoder
serEncoder = CommEncoder(9600,comArduinoEncoder);
serEncoder.SetSerialMotor(serMotor);
serEncoder.Start();
    


