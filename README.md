# ESP32 LED Strip library

## Description
This library can be used to drive LED strips from the ESP32 using the RMT peripheral. This allows the RMT peripheral to handle all of the transmission while the processor is free to support other tasks.

## Limitations
Right now the library only supports the timing for WS2812/WS2812B LED strips. I believe most of the Adafruit Neopixel strips use these LED drivers. However, adding functionality for other LED strips with single-wire data like the SK6812 or WS2811 should be easily achievable. I don't currently have access to any of these LED strips, but I can add support for these in the near future.
Also, because of interrupt numbers, this library only supports one led strip. Functionality for more than one will be added soon!