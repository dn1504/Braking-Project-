from serial import Serial
from threading import Thread
from queue import Queue
from threading import Lock
import threading
import serial_comm_constant
import time

# https://stackoverflow.com/questions/323972/is-there-any-way-to-kill-a-thread
class StoppableThread(Thread):
    """Thread class with a stop() method. The thread itself has to check
    regularly for the stopped() condition."""

    def __init__(self, callback):
        super(StoppableThread, self).__init__(target = callback);
        self._stop_event = threading.Event();

    def stop(self):
        self._stop_event.set();

    def stopped(self):
        return self._stop_event.is_set();

class SerialComm :

    _serial                 = None;
    _threadComm             = None; # Untuk komunikasi
    _threadMain             = None; # Untuk mengeksekusi fungsi yang thread komunikasi spesifikasikan
    _baudRate               = None;
    _portNo                 = None;
    _queue_main_functions   = Queue(); 

    def __init__(self, baudRate, portNo) :
        """ Inisialisasi """

        self._baudRate  = baudRate;
        self._portNo    = portNo;
        self._serial    = Serial(self._portNo, self._baudRate, timeout=serial_comm_constant.TIMEOUT_READLINE);

    # https://stackoverflow.com/questions/18989446/execute-python-function-in-main-thread-from-call-in-dummy-thread
    def _callbackMain(self) :
        """ Callback main yang dipanggil """

        while True :
            try:
                f = self._queue_main_functions.get(); #doesn't block
            except Queue.Empty: #raised when queue is empty
                break;
            
            # https://stackoverflow.com/questions/683542/how-to-put-a-function-and-arguments-into-python-queue
            # Baru dieksekusi      
            func = f[0];
            args = f[1];
            func(*args); # ini cara manggilnya f[0] adalah fungsinya, f[1] adalah argumen nya     

    def _callbackComm(self) :
        """ Callback comm yang dipanggil """

        # Reading (basisnya delimiter koma dan diawali dengan $, diakhiri dengan \r\n
        # fungsi _serial.readline() bermasalah nih kalau dia ga ada data masuk yang diakhiri \r\n
        # _serial.readline() akan nunggu forever ...., tapi bisa diatasin dengan kasih timeout Serial(self._portNo, self._baudRate, timeout=constant.TIMEOUT_READLINE)
        while True :
            try:
                #print("[SerialCommThread] >> Menunggu  ...");
                dataSerial = self._serial.readline().decode('utf-8');  # diakhir dengan \r\n
                if dataSerial != '' :
                    #print("[SerialCommThread] >> Data masuk ->> " + dataSerial);
                    self._queue_main_functions.put((self._DataReceived,[dataSerial]));

            except UnicodeDecodeError: # catch error and ignore it (ga ngerti harus diapain..)
                print('UnicodeDecodeError -> for now neglect it ...')
  
    def _DataReceived(self, data) :

        d = data.split(',');
        if d[0] == '$' and d[-1] == '\r\n' : # Sejauh ini penanda cuma ini, selain itu bukan message
            del d[0]; # hapus $
            del d[-1]; # hapus \r\n
            if len(d) != 0 :
                self.DataReceived(d);
                        

    """ Menerima data -> fungsi ini bisa dioveride """
    def DataReceived(self, data) :
        #...print(data)
        return;

    """ Mengirim data -> fungsi Jangan dioveride """
    def DataSent(self, data) :      
       
        if self._serial.is_open :
            self._serial.write(data.encode('utf-8'));
        
    def _Start(self) :
        """ Memulai """

        # Stop dulu
        self._Stop();

        # Mulai lagi
        if not self._serial.is_open :
            self._serial.open();

        # Kasih waktu beberapa detik biar ready, katanya seperti itu ...
        time.sleep(serial_comm_constant.TIME_WAITING_READY_SERIAL);

        # Read the buffer in
        while self._serial.in_waiting > 0:
            self.read();
        
        # Run threads        
        if self._serial.is_open :
            self._threadMain = StoppableThread(self._callbackMain);
            self._threadMain.start();
            self._threadComm = StoppableThread(self._callbackComm);
            self._threadComm.start();
            #self._thread.join(); // Ga ngerti maksudnya : https://stackoverflow.com/questions/323972/is-there-any-way-to-kill-a-thread

        return True; #self._serial.is_open and self._threadMain.is_alive and self._threadComm.is_alive;

    def _Stop(self) :
        """ Menutup """

        # Cek kalau thread masih running
        if (self._threadComm != None) :
            if self._threadComm.is_alive :
                self._threadComm.stop();
        
        if (self._threadMain != None) :
            if self._threadMain.is_alive :
                self._threadMain.stop();

        # Cek kalau serial comm masih jalan
        if (self._serial != None) :
            if self._serial.is_open :
                self._serial.close();