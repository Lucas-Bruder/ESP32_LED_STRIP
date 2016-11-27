# ESP32 LED Strip library

## Description
This library can be used to drive LED strips from the ESP32 using the RMT peripheral. This allows the RMT peripheral to handle all of the transmission while the processor is free to support other tasks.

## Limitations
Right now the library only supports the timing for WS2812/WS2812B LED strips. I believe most of the Adafruit Neopixel strips use these LED drivers.  The ability to add drivers for other LEDs like the SK6812 and WS2811 is implemented, but I don't have any to test on.
