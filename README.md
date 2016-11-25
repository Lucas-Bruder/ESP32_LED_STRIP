# ESP32 LED Strip library

## Description
This library is used to drive addressable LED strips using the ESP32. 
It is currently a work in progress; right now this library relies on bit-banging to drive the LEDs. I am currently working to change the library to use the RMT module so the processor isn't busy using nop loops to do the timing.
I will try to avoid changing any public functions, but expect the functions in the led_strip.c file to change periodically as I move to using the RMT module in the ESP32.
