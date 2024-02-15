# E01: Binary Semaphore
Example presents simple use case of *binary semaphore*. Application uses **LED** as user output and **serial port** as user input. Binary semaphore is initialized with counter value 0. Semaphore is released after receiving character via serial port. If semaphore can be successfully acquired blue LED is toggled. After each attempt at semaphore acquisition (successful or after timeout), greed LED is toggled. Failed release attempt will turn on red LED. 

**Serial port configuration:** 115200 bps, 8 data bits, 1 stop bit, no parity.