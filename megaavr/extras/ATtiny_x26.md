# ATtiny 426/826/1626/3226
![x26 Pin Mapping](ATtiny_x26.gif "Arduino Pin Mapping for ATtiny x26")

 Specifications       |    ATtiny426   |   ATtiny826   |   ATtiny1626  |  ATtiny3226   |
----------------------|----------------|---------------|---------------|---------------|
Availability          |    Q2/Q3 2021  |    Q2/Q3 2021 | Since Q1 2021 |    Future     |
Flash (program memory)|     4096 bytes |    8192 bytes |   16384 bytes |   32768 bytes |
Flash w/Optiboot      |     3584 bytes |    7680 bytes |   15872 bytes |   32256 bytes |
RAM                   |      512 bytes |    1024 bytes |    2048 bytes |    3072 bytes |
EEPROM                |      128 bytes |     128 bytes |     256 bytes |     256 bytes |
Bootloader (optional) | Optiboot (not recommended) | Optiboot | Optiboot |    Optiboot |


Feature       |             Specification         |
--------------|-----------------------------------|
GPIO Pins     | 18 (17 usable, 16 with alt-reset) |
ADC           |        12-bit, differential w/PGA |
ADC Channels  |      15, 7 can be neg. diff. pin. |
DAC           |          Internal for AC ref only |
Comparator    |                                 1 |
PWM Channels  |                                 6 |
Timer Type B  |                                 2 |
Timer Type D  |                                No |
CCL logic     |              4 LUTs, 10/12 inputs |
Interfaces    |                UART (2), SPI, I2C |
Millis Timer  |   TCA0, TCB0, TCB1 (default), RTC |
RTC crystal   |      Optional  32KHz TOSC1, TOSC2 |
Package       |   SOIC-20, TSSOP-20, QFN-20 3x3mm |

The 20-pin 2-series parts bring one very exciting feature, particularly for those who like using serial bootloaders instead of UPDI - the Reset function can be moved to pin PB4, allowing PA0 to remain UPDI, while also having a proper hardware reset! These are being advertised in SOIC-20 and the super tiny 3mm x 3mm QFN of the 1-series, plus SSOP. Not sure SSOP will set the world on fire. It's also not certain yet if they will actually make the 32k parts available in the QFN - fingers crossed.

DAC, TCD, and the extra analog comparators have been removed in favor of a second UART, and the super ADC, which can accumulate 1024 samples in a single burst read; decimation of the accumulated value so all of the digits are meaningful can give up to 17-bits of accuracy. For advanced ADC functionality, several new functions are provided, see ADC section of the main readme for more information.

Optiboot is included and with the alternate reset option, you can have normal autoreset to enter the bootloader like a "normal" optiboot device without rendering it un-reprogrammable via UPDI (which precludes setting fuses among other things). That said, many users prefer to use one serial adapter for SerialUPDI programming, not using any bootloader, and have a second serial adapter, which is kept open in some serial serial monitor that isn't the Arduiino IDE's (This is what I do, using hTerm).

## Buy official megaTinyCore breakouts and support continued development
TBD



## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
