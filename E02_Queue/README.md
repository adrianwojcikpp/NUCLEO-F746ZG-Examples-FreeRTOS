# E02: Queue
Example presents simple use case of *message queue*. Application uses **LED** as user output and **serial port** as user input. Each character received via serial port is put to message queue. After four characters are in queue (length of LED control message), queue is cleared and control message is parsed and executed. Failed attempt at putting character to queue (e.g. overrun) will start red LED blinking. 

**Control messages**: 
* *LD11* - turns on green LED (LD1)
* *LD10* - turns off green LED (LD1)
* *LD21* - turns on blue LED (LD2)
* *LD20* - turns off blue LED (LD2)
* *LD31* - turns on red LED (LD3)
* *LD30* - turns off red LED (LD3)

<br> **Serial port configuration:** 115200 bps, 8 data bits, 1 stop bit, no parity.
