# Library Compatibility
Please include information that uniquely identifies the library, as popular libraries have dozens of forks all over the internet, and it is often difficult to identify which is the "right one". Bonus points if you include a version number too - but I've given up on that!

| Library      | Version | Library URL or reference                              | Status                               | Notes                      |
|--------------|---------|-------------------------------------------------------|--------------------------------------|----------------------------|
| tinyGPS++    |         | https://github.com/mikalhart/TinyGPSPlus              |                 Compiles amd works   |                            |
| ~NeoGPS~     |         |                                                       | ~Architecture warning, doesn't work~ |                            |
| VL53L0X      |         | Pololu                                                |                 Compiles amd works   |                            |
| VEML6070     |         | Adafruit                                              |                 Compiles amd works   |                            |
| MLX90614     |         | https://github.com/adafruit/Adafruit-MLX90614-Library |                 Compiles amd works   |                            |
| BMP180       |         | Adafruit                                              |                 Compiles amd works   |                            |
| BME280       |         | Adafruit                                              |                 Compiles amd works   |                            |
| OneWire      |         | https://github.com/pstolarz/OneWireNg                 |                 Compiles amd works   |                            |
| ~Neopixel~   |         | ~Adafruit~                                            |            ~DOES NOT WORK w/AVRxt~   | Use included tinyNeoPixel - modified to work |
| LED Backpack |   1.1.8 | https://github.com/adafruit/Adafruit_LED_Backpack     |                 Compiles and works   |                            |
| Tiny4kOLED   |         | https://github.com/datacute/Tiny4kOLED                |                 Compiles and works   | SSD1306, not just for tiny |
| rc-switch    |         | https://github.com/sui77/rc-switch  (TX mode)         |                 Compiles and works   | A surprise. I don't expect RX will work |
| FAB-LED      |         | https://github.com/sonyhome/FAB_LED                   |        Does not work, my fork does   | No response to my PR :-/ WS2812 w/out buffer  |
| ~Servo~      |         | ~From library manager~ (the one included works fine)  |                      Compile error   | Use Servo_megaTinyCore if installed Servo via lib. mgr. |
|              |         |                                                       |                                      |                            |
|              |         |                                                       |                                      |                            |
|              |         |                                                       |                                      |                            |
|              |         |                                                       |                                      |                            |
