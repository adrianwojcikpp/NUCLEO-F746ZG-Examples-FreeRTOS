# E04: Mutex
Example presents simple use case of *mutex*. Application uses **LED** and **serial port** as user output and **USER button** as user input. Every second blue LED is toggled and message with LED state is sent via serial port. Every time a button push is detected green LED is toggled and message with LED state is send via serial port. Attempts at writing to serial port are guarded by pair of acquisition and release of mutex.

**Serial port configuration:** 115200 bps, 8 data bits, 1 stop bit, no parity.
