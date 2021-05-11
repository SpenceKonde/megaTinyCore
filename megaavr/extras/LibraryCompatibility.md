# Library Compatibility
Please include information that uniquely identifies the library, as popular libraries have dozens of forks all over the internet, and it is often difficult to identify which is the "right one". Bonus points if you include a version number too - but I've given up on that!

| Library      | Version | Library URL or reference                              | Status                               | Notes                                                     |
|--------------|---------|-------------------------------------------------------|--------------------------------------|-----------------------------------------------------------|
| tinyGPS++    |         | https://github.com/mikalhart/TinyGPSPlus              |                 Compiles amd works   |                                                           |
| ~NeoGPS~     |         |                                                       |  Architecture warning, doesn't work  |                                                           |
| VL53L0X      |         | Pololu                                                |                 Compiles amd works   |                                                           |
| VEML6070     |         | Adafruit                                              |                 Compiles amd works   |                                                           |
| MLX90614     |         | https://github.com/adafruit/Adafruit-MLX90614-Library |                 Compiles amd works   |                                                           |
| BMP180       |         | Adafruit                                              |                 Compiles amd works   |                                                           |
| BME280       |         | Adafruit                                              |                 Compiles amd works   |                                                           |
| OneWireNg    |         | https://github.com/pstolarz/OneWireNg                 |                 Compiles amd works   |                                                           |
| ~OneWire~    | Original| ~https://github.com/PaulStoffregen/OneWire~           |                  Does not compile.   | Not compatible with modern AVRs other than the ATmega4809 |
| OneWire      | Forked  | https://github.com/SpenceKonde/OneWire                |                 Compiles amd works   | Tested PR submitted August 2020. No response from Paul.   |
| ~Neopixel~   |         | ~Adafruit~                                            |             DOES NOT WORK w/AVRxt    | Use included tinyNeoPixel - Same API, adapted for these   |
| LED Backpack |   1.1.8 | https://github.com/adafruit/Adafruit_LED_Backpack     |                 Compiles and works   |                                                           |
| Tiny4kOLED   |         | https://github.com/datacute/Tiny4kOLED                |                 Compiles and works   | SSD1306, not just for tiny - anything with Wire.h lib     |
| rc-switch    |         | https://github.com/sui77/rc-switch  (TX mode)         |                 Compiles and works   | A surprise. I don't expect RX will work                   |
| ~FAB-LED~    | Original| ~https://github.com/sonyhome/FAB_LED~                 |        Architecture is unsupported   | FAB-LED is a WS2812 w/out buffer library; impressive      |
| FAB-LED      | Forked  | https://github.com/SpenceKonde/FAB_LED                | I added support for megaavr, works   | No response to my PR to get my fix into his version :-/   |
| ~Servo~      |         | ~From library manager~ (the one included w/core works)|                      Compile error   | Use Servo_megaTinyCore if installed Servo via lib. mgr.   |
| TLC5947      |         | https://github.com/adafruit/Adafruit_TLC5947          |                 Compiles and works   |                                                           |
| MFRC522      | Frozen  | https://github.com/miguelbalboa/rfid                  |                 Compiles and works   | Long ago had issue relating to F(). This is the F()ing library that forced the return of the macro! |
