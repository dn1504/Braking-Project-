from serial_comm_intepreter import SerialCommIntepreter as itp

# UMUM
MAX_SENT_DATA_BUFFER = 5;
TIMEOUT_READLINE = 0.1; # second (JANGAN NOL !!!), ini juga perlu diatur jangan terlalu cepat. kalau terlalu cepat nanti ga nemu /r/n dalam segera, alhasil hang
TIME_DELAY_BETWEEN_WRITE_READ = 0.01;
TIME_WAITING_READY_SERIAL = 1;

# ENCODER
ENCODER_DATA        = '[ED]';
ENCODER_GET_DATA    = '[EG]';
ENCODER_START_RESET = '[ES]';
ENCODER_READY       = '[ER]';

# MOTOR
MOTOR_READY         = '[MR]';
MOTOR_SET_VELOCITY  = '[MV]';
MOTOR_SET_ANGLE     = '[MA]';
MOTOR_START_RESET   = '[MS]';

# MESSAGE FOR COMMAND (INFO) WITHOUT DATA
MSG_ENCODER_GET_DATA    = itp.FormatMessage(ENCODER_GET_DATA);
MSG_ENCODER_START_RESET = itp.FormatMessage(ENCODER_START_RESET);
MSG_ENCODER_READY       = itp.FormatMessage(ENCODER_READY);
MSG_MOTOR_READY         = itp.FormatMessage(MOTOR_READY);
MSG_MOTOR_START_RESET   = itp.FormatMessage(MOTOR_START_RESET);

