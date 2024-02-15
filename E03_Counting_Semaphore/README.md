# E03: Counting Semaphore
Example presents simple use case of *counting semaphore*. Application uses **LED** as user output and **serial port** as user input and output. Counting semaphore is initialized with counter value 16 (*maximal value*). Semaphore is released after receiving character via serial port. If semaphore can be successfully acquired blue LED is toggled every 100 ms. If semaphore counter is greater than 0, message with counter value is sent via serial port. Failed release attempt will turn on red LED. 

**Serial port configuration:** 115200 bps, 8 data bits, 1 stop bit, no parity.
