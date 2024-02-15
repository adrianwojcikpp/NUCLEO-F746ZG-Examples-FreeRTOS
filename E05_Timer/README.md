# E05: Timer
Example presents simple use case of *timer*. Application uses **LED** as user output and **serial port** as user input. Timer is initialized with period od 100 ms and toggles blue LED after each period elapses. Serial port listen for four character message. If message represents numeric value between 100 and 1000 (noted with leading zeros), timer is stopped and restarted with new period.

**Serial port configuration:** 115200 bps, 8 data bits, 1 stop bit, no parity.
