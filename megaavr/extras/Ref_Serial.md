# Hardware Serial reference (AKA UART or USART)
All of the supported parts contain USART serial ports. We all use them every day, but not all of us are familiar with the details of these. While USART, SPI, and TWI are all serial protocols, "Serial" without qualification almost universally refers to a USART. These are the only protocol for which USB adapters are widely available and ubiquitous. SPI and I2C USB adapters exist - but they aren't standard and generally require some measure of drivers. And while there are countless applications that will act as a serial console, software for I2C and SPI, is usually vendor specific if it has been written at all.

On all supported devices, where the appropriate pins are present, they can be pin-swapped, and on DA and DB parts, each port except PORTD gets a USART, which defaults to pins 0, 1, 2 and 3 (TX, TX, XCK and XDIR), and 4, 5, 6 and 7 when pinswapped. This is configured using the Serial.swap() or Serial.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling `Serial.begin()`

## Background - quick overview of how serial is different
In addition to being by far the most popular interface for Arduino users, rthere are several things that really set it apart... and these have consequences, for better and worse. First the vital stats:
* It uses 2 wires, transmit, and receive. Transmit of one connects to receive of the other (that is, they cross over. Sometimes connectors are marked relative to the device, other times to the device you connect to it; because of this, devices often extend "TX" and "RX" to "TXO" (Transmit Out) and "RXI" (Receive In).
* It is operated at a selectable baud rate, from incredibly slow speeds like 300 baud from early teletypes up to several megabaud, but in practice, USART is normally used between 9600 baud and 1megabaud.
* *There is no clock* - timing is used to differentiate bits. That presents some problems, as *the two sides must have agreed on the baud rate* and have clocks that agree.
  * Mismatched baud rates are generally interpreted as gibberish with lots of non-printing characters. Sometimes, but not always, you can examine the bit patterns of the gibberish and figure out whether it's too fast or too slow. Simply trying different speeds is usually much more efficient.
* It's not just the baud either that needs to be agreed on. The number of "stop" bits (in other words, how long the pause between bytes should be; it can be 1 or 2 stopbits for an AVR - but other devices sometimes support 1.5 varies from 1 to 2. Not even the number of bits to a byte is universally agreed upon! Anywhere between 5 and 9 can be supported by the hardware, but dealing with 9-bit characters is not supported by this implementation of serial.
* It is active low, readily lending itself to open drain multidrop networks
* Running a single combined TX and RX line is not uncommon, particularly when RS-485 is in use.
* There are a great many protocols with a different PHY layer but the same format - RS485, LIN, and RS232 are common examples.


### USART vs UART
UART us "Universal Asynchronous Receiver Transmitter"

USART is "Universal Synchronous/Asynchronous Receiver Transmitter"

A UART can only communicate in asynchronous mode (with TX and RX), while a USART optionally adds a third pin, XCK, that can be configured as a clock. The serial ports on AVR devices have always been USARTs (a very small number of ancient classic AVRs have UARTs instead)

The synchronous mode isn't as flexible as you might think, largely because of the whole start/stop bit thing. Another reason it is less frequently used is the availability of adapter hardware for desktop computers, or rather the lack thereof. It is, if anything, even harder to find support for than SPI.

Note that, unlike the `MSPI` mode, Synchronous Mode does support operation as either slave or master; the MSPI mode, which drops the start and stop bits, and works largely like a proper SPI interface is master only.

`MSPI` mode is not supported by the core. A third party library must be used.



### Terminology (non-standard)
* "Pin set" - synonym for "pin mapping option" and other descriptive terms, consisting of all the pins that could be enabled with a given PORTMUX configuration and their roles,
* "Mux Code" - For a given pinset, the mux code is the value written to the appropriate bits of the PORTMUX register controlling USART pin selection. It is a value of 0, 1, 2, or 3 leftshifted some number of places. For the same pinset, for the same peripheral, with the same swap level, different parts may have a different mux code.
* "Swap Level" - The swap level is the logical number of the pinset - the default mapping is always 0, swap level n is the option named as ALTn in the datasheet. Microchip has never articulated any policy of keeping pinsets consistent between parts, all indications are that the Dx, Ex, and megaAVR 0-series will have the same pinsets, though some parts have different swap levels.

For example, TX = PC4, RX = PC5, XCK = PC6, XDIR = PC7 is pinset 1 for USART1.
It's swap level is 1, and it's mux code is is 0x04 (on DA/DB parts - it will presumably be 0x08 on the EA-series, assuming it stays like the product brief describes, with the DD-series mux options).

## Serial in DxCore and megaTinyCore
We have tweaked the Serial classes for greater flexibility, efficiently, and performance.
It implements the standard Serial API, plus a small number of additional options.

### Serial.swap(mux option)

### Serial.pins(tx, rx)

These two methods both change the pins connected to the device. Swap takes a number - that's 0 for the "default" 1 for "ALT1" and so on. It is more efficient and is the preferred method. Serial.pins does exactly what you would think - sets the TX and RX pins of that port to the TX and RX pins given by the arguments. Assuming of course, that the pins are valid mux options. Either way, both of these functions return a boolean, true for a successful change of the pins, and this always must be called before Serial.begin() (or after Serial.end() but before reopening it with Serial.begin() again). If the swap or pins given are invalid, it will return false, and reset the pins to the default value. If the compiler can determine at compile time that it will never work (generally, constant arguments that aren't valid, like calling `Serial.swap(21)` instead of `Serial.swap(2)`) we will refuse to compile and throw a `badArg()` error.

The pinsets are shown on the pinout charts and/or part specific reference pages. In brief:
* On DA/DB parts, every PORT except the middle one (PORTD) got a USART. and in ascending number, the USARTs are associated with PORTA, PORTC, PORTF, PORTB, PORTE, PORTG. Parts that don't have the port  Default on pins 0-3 TX to XDIR, alternate option on 4-7. That is identical to the megaAVR 0-series.
* The DD has made things a lot more complicated/exciting with regards to pin mapping for many peripherals, but nothing benefited more than USART0. The options are:
  * USART0: PA0-PA3, PA4-PA7, PA2-PA3 (no XCK/XDIR), PD4-7, PC1-PC3 (no XDIR). (note that PA2-PA7 are not available on 14-pin parts, so valid swap options are 0, 3, 4, with only 4 having all pins)
  * USART1: PC0-PC3 (note that 14/20 pin parts have no PC0), PC4-PC7 (not available because pins not present.), PD6-PD7 (no XCK/XDIR).
* The EA-series has the same options as the DDs, plus USART2 and maybe USART3 on 48-pin parts. The product brief is all we have to go on, and it's not self-consistent on the matter of how many USARTs these will have. It looks like they just copy-pasted pieces of the DD and DB's table together, .
* On tinyAVR parts with more than 8 pins, the two mapping options for Serial are PB2, PB3, PB1, PB0 and PA1, PA2, PA3, PA4 for TX, RX, XCK, XDIR.
* On 8-pin parts, they are PA6, PA7, PA3, PA0 (yes, that means no XDIR for the 8-pin parts without setting UPDI as GPIO), and PA1, PA2 (no XCK or XDIR with alt mapping)
* On 2-series parts, the second USART shares the default mux position with the first USART's alternate option. The options are PA1, PA2, PA3, PA4 or PC0, PC1, PC3, PC3. Note that this means that the there is no alternate option for 14-pin 2-series parts.

### Serial.getPin(pin_name)
Returns the pin corresponding to that function with the current mux option.
```c
PIN_SERIAL_TX
PIN_SERIAL_RX
PIN_SERIAL_XCK
PIN_SERIAL_XDIR
```

This function is meant for use by libraries to allow them to figure out which pin is in use without duplicating the logic we have already implemented.

### Serial.printHex() and Serial.printHexln()
One extremely common task in embedded programming, particularly debugging embedded code, is printing data out as hexadecimal numbers. There is of course,  `Serial.print(number,HEX)`, but not only does that burn more flash, it doesn't add an appropriate number of leading zeros (making it hard to read). It's designed to print numbers in the way that programmers would want them printed - the number of leading zeros will match the data type, ie if you print an unsigned long, with 1 in the low byte and 0's in the other three, it will print 00000001, not 1. As you would expect, printHexln() does the same thing and adds a newline.
Below is an unabridged list of the versions:
```c++
    void                printHex(const     uint8_t              b);
    void                printHex(const    uint16_t  w, bool s = 0);
    void                printHex(const    uint32_t  l, bool s = 0);
    void                printHex(const      int8_t  b)              {printHex((uint8_t )   b);           }
    void                printHex(const        char  b)              {printHex((uint8_t )   b);           }
    void                printHex(const     int16_t  w, bool s = 0)  {printHex((uint16_t)w, s);           }
    void                printHex(const     int32_t  l, bool s = 0)  {printHex((uint32_t)l, s);           }
    void              printHexln(const      int8_t  b)              {printHex((uint8_t )b   ); println();}
    void              printHexln(const        char  b)              {printHex((uint8_t )b   ); println();}
    void              printHexln(const     uint8_t  b)              {printHex(          b   ); println();}
    void              printHexln(const    uint16_t  w, bool s = 0)  {printHex(          w, s); println();}
    void              printHexln(const    uint32_t  l, bool s = 0)  {printHex(          l, s); println();}
    void              printHexln(const     int16_t  w, bool s = 0)  {printHex((uint16_t)w, s); println();}
    void              printHexln(const     int32_t  l, bool s = 0)  {printHex((uint32_t)l, s); println();}
    uint8_t *           printHex(          uint8_t* p, uint8_t len, char sep = 0            );
    uint16_t *          printHex(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    volatile uint8_t *  printHex(volatile  uint8_t* p, uint8_t len, char sep = 0            );
    volatile uint16_t * printHex(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
```

There are two particular features worth noting in addition to the correct number of leading zeros, and the fact that it is not horrendously bloated like full serial print.
1. For 16 and 32-bit datatypes, you can pass a boolean as the second argument. If it is true, the endianness will be reversed.

2. You can also pass a pointer to either a uint8_t or a uint16_t variable. In this case the arguments are:
```c
uint8_t *  printHex(uint8_t * p, uint8_t len, char sep = 0);
uint16_t * printHex(uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
```

It will print `len` elements starting from the address the pointer is pointed at, if `sep` is non-zero, that character will be placed between each byte or word - these are **characters** not strings. A single character, enclosed between a pair of single quotes. ":" is a 2 character string - a colon followed by a null terminator, and is invalid. Anything between double quotes is invalid. Use single quotes as shown below. If s is true for a pointer to 16-bit values, the endianness will be swapped as well. There is a slightly different implementation for pointers to volatile variables to help prevent problems when using this to dump the contents of peripheral registers. Which is what printHex was made for.
```c
  // dump every register associated with the CCL
  volatile uint8_t * cclconfig= (volatile uint8_t*)&CCL;
  cclconfig = Serial.printHex(cclconfig, 0x08, ':');  // per datasheet register summary, first 8 are either reserved bytes or effect all LUTs
  cclconfig = Serial.printHex(cclconfig, 0x4, ':');   // LUT0 each LUT has it's own 4 bytes.
  cclconfig = Serial.printHex(cclconfig, 0x4, ':');   // LUT1
  cclconfig = Serial.printHex(cclconfig, 0x4, ':');   // LUT2
  cclconfig = Serial.printHex(cclconfig, 0x4, ':');   // LUT3
/* Output (would be more interesting if I actually was using the CCL at the time, instead of demonstrating serial functions:
00:00:00:00:00:00:00:00
00:00:00:00
00:00:00:00
00:00:00:00
00:00:00:00
*/
```
Many peripherals have a couple of 16-bit registers, amongst a sea of 16-bit ones. Say I'm trying to debug a problem involving the synchronous mode serial (because recently I was), it's got a baud register in the middle that I'd like to have formatted nicely. A union of a word and a byte pointer is just the thing here:
```c
  union {
    volatile uint16_t *  intp;
    volatile uint8_t *  bytep;
  } data;
  syncBegin(Serial1,1000000,(SERIAL_8N1 | SERIAL_MODE_SYNC), SYNCBEGIN_NORMAL_MASTER);
  data.bytep = (volatile uint8_t*)&USART1;
  data.bytep += 4; // skip the two buffers
  data.bytep = Serial.printHex(data.bytep, 4, ':'); // prints STATUS, CTRLA, CTRLB, CTRLC
  data.intp = Serial.printHex(data.intp, 1, ':'); //next up is the BAUD register, a 16-bit one
  data.bytep = Serial.printHex(data.bytep, 6, ':'); //and then there are 6 more byte registers....
/* Output:
  20:80:C0:43
  0280
  00:00:00:00:00:00
*/
```

### Serial.begin(uint32_t baud, uint16_t options)
This starts the serial port. Options should be made by combining the constant referring to the desired baud rate, parity and stop bit length, zero or more of the modifiers below

#### Basic USART options

| Data Size | Parity | 1 stop bit | 1 stop bit |
|-----------|--------|------------|------------|
| 5 bit     |  NONE  | SERIAL_5N1 | SERIAL_5N2 |
| 6 bit     |  NONE  | SERIAL_6N1 | SERIAL_6N2 |
| 7 bit     |  NONE  | SERIAL_7N1 | SERIAL_7N2 |
| 8 bit     |  NONE  | SERIAL_8N1 | SERIAL_8N2 |
| 5 bit     |  EVEN  | SERIAL_5E1 | SERIAL_5E2 |
| 6 bit     |  EVEN  | SERIAL_6E1 | SERIAL_6E2 |
| 7 bit     |  EVEN  | SERIAL_7E1 | SERIAL_7E2 |
| 8 bit     |  EVEN  | SERIAL_8E1 | SERIAL_8E2 |
| 5 bit     |   ODD  | SERIAL_5O1 | SERIAL_5O2 |
| 6 bit     |   ODD  | SERIAL_6O1 | SERIAL_6O2 |
| 7 bit     |   ODD  | SERIAL_7O1 | SERIAL_7O2 |
| 8 bit     |   ODD  | SERIAL_8O1 | SERIAL_8O2 |


#### Modifiers
* SERIAL_RS485        - Enables RS485 mode.
* SERIAL_OPENDRAIN    - Sets port to open-drain mode
* SERIAL_LOOPBACK     - Enables single wire operation and internally connects tx to rx.
* SERIAL_TX_ONLY      - Enables only Tx.
* SERIAL_RX_ONLY      - Enables only Rx.
* SERIAL_EVENT_RX     -
* SERIAL_HALF_DUPLEX  - Synonym for (SERIAL_OPENDRAIN | SERIAL_LOOPBACK)
* SERIAL_MODE_SYNC    - Uses synchronous mode instead of asynchronous. See notes below, additional configuration required.

#### MSPI options
* SERIAL_MSPI_MSB_FIRST
* SERIAL_MSPI_LSB_FIRST
* SERIAL_MSPI_MSB_FIRST_PHASE
* SERIAL_MSPI_LSB_FIRST_PHASE


Can be combined as in:
`Serial1.begin(115200,(SERIAL_8N1 | SERIAL_OPENDRAIN | SERIAL_RS485 ))`

If you use the two argument form of Serial.begin() be certain to remember to pass the constant, not just a modifier.

### Loopback Mode
When Loopback mode is enabled, the RX pin is released, and TX is internally connected to Rx. This is only a functional loopback test port, because another device couldn't drive the line low without fighting for control over the pin with this device. Loopback mode itself isn't very useful. But see below.

### Open Drain Mode
In Open Drain mode, the TX pin will no longer drive high. The pin must not be set as OUTPUT. If you don't fiddle with it, this will correctly handled for you. This is also not terribly useful on it's own - though it would be useful communicating with a lower voltage device, with TX pulled up to the other device's Vcc.

### RS485 Mode
In RS485 mode, at initialization, the HWSerial class configures the XDIR pin as an OUTPUT. 1 bit-time prior to sending any data, the XDIR pin will be driven HIGH. If using RS485, this should be connected to the external line driver IC to enable transmit. XDIR will be lowered 1 bit-time after the last bit has been sent. If you require the opposite polarity, simply set PORTx.PINnCTRL |= PORT_INVEN_bm;
or use `pinConfigure()` [See Digital I/O Reference](Ref_Digital.md)

RS485 mode in combination with RX_ONLY will simply set the pin to an output, but never use it, because the TX module isn't enabled.

#### These options were meant to be combined
* Loopback + Open Drain - These two not-particularly-useful options, when combined, become very useful - this gives you a half-duplex single serial interface. This is fairly common (UPDI is actually implemented this way) it's almost ubiquitous in RS485. Reference for the configuration is provided in [Microchip Application note AN2658: USART in One-wire Mode](https://www.microchip.com/en-us/application-notes/an2658.html)
* Loopback + Open Drain + RX485: In this mode, it will work perfectly for the case where there is an external line driver IC but it has only a single TX/RX combined wire and a TX_Enable pin (terminology may vary).

#### Half-duplex schemes change the behavior of Serial in important ways
Normally, RX functionality is not disabled unless the user specifically requests it. Bytes received at any time will be placed into the buffer by the USARTn RxC interrupt as long as it is not full. With loopback mode enabled, you receive all the characters you transmit. That's fine for just loopback - since TX is actively driven high when idle, you can't exactly receive data any other way. When Open Drain mode is also active, though, the stuff that you sent would end up intermixed with actual received data. This is not very helpful (It would be nice to check the received data for to ensure there were no collisions. However, this is challenging since you'd have to keep a record of what you sent to compare it to. There are almost 3 bytes of buffer (receive is double-buffered, plus the incoming shift register; but transmit is handled by the DRE interrupt, while receive is handled by the RXC interrupt, so it would need to implement ring buffer that both of these could access.... But those are written in assembly with n alarming small number of clockcycles after the end of the ISR before the next character has to be dealt with (see Appendix A). So under these circumstances behavior regarding RX is altered slightly:

```
CTRLA: LBME
CTRLB: ODME and TXEN and RXEN
```

That configuration will result from calling the two argument version of begin() with SERIAL_OPEN_DRAIN and SERIAL_LOOPBACK, or equivalently, SERIAL_HALF_DUPLEX, and neither SERIAL_TX_ONLY nor SERIAL_RX_ONLY, configuring the USART for one-wire half duplex, open drain mode. This variant of UART is used in a fairly large number of devices for low speed signalling

In this case, Any *write* will temporarily disable the RXC interrupt (hence, disabling receiving) and instead enable the TXC interrupt. When the TXC interrupt executes, it will read RXDATAL until the RXC flag is cleared (to flush out the characters you sent), disable TXC and enable the RXC interrupt again.

### Inverted Serial
Rarely, one needs to have *inverted* serial, ie, idle line is low, the start bit is high, high bits are 0, low bits are 1 and the stop bit is low.) This can be achieved by by inverting the port (either manually, `PORTx.PINxCTRL |= PORT_INVEN_bm;` or via pinConfigure() - [see Digital I/O Reference](Ref_Digital.md) . Generally, when one of the pins is inverted, the other one is to, so you probably want to invert both TX and RX, and you probably don't want the pullup on either of them, since they lines are idle LOW when inverted.

```c
//after Serial.begin(), which would mess all this up.
// assumg TX_PIN and RX_PIN are the TX and RX pins respectively
pinConfigure(TX_PIN,(PIN_PULLUP_OFF | PIN_INVERT_ON));
pinConfigure(TX_PIN,(PIN_PULLUP_OFF | PIN_INVERT_ON));
// for Sync mode, you can use the constants described below
```
### Event RX - documentation pending testing and verification

### Using Synchronous modes
There are two synchronous (clocked) communication modes possible with the hardware serial modules: Synchronous (Sync USART) and MSPI (Master SPI). These are "semi-supported" by the hardware serial class: You cannot just call Serial.begin() with the right arguments and have everything configured fully. But manually scaling the baud rate, and configuring the XCK pin, should be all that is required, and these modes should then work through Serial.print() and similar. The API is of course not ideal for SPI (recall that the master receives exactly 1 byte per byte sent with similar considerations for synch USART), but it is expected to be sufficient for most purposes. As these modes are rarely used, there is little existing code related to them, so more specialized libraries may not always be available - but at the same time, the rare usage does not justify compromising normal operation for the users of Serial (ie literally everyone) improve the user experience of a very small number of users working with the clocked modes. You must either know the clock pin, or know which swap level you are using - which you presumably do, since you set it. `PIN_HWSERIALn_XCK_PINSWAP_s` contains the XCK pin for USART `n` with swap level `s`; the default XCK pin is simply `PIN_HWSERIALn_XCK`.

There are three major differences between SPI and USART in sync mode. The first and most obvious is that the data framing is different: USART mode has a start and a stop bit (potentially 2 of the latter) possibly a parity bit, and the character size is configurable, while SPI always uses 8-bit bytes. The second is that MSPI has 4 data modes, just like real SPI - these control whether the data is clocked on the rising or falling edge of the clock through inverting of the pin, but unlike sync USART, it also has support for sampling on the first transition of the clock, with data being set up prior to that as opposed to the second transition (as the clock moves back to it's idle state). Thus, essentially MSPI mode acts as an additional SPI peripheral that can only be used as a master, but has a more flexible clock. Sync USART on the other hand is it's own beast altogether. The core makes no attempt to prevent you from configuring modifiers which do not make sense in these modes - you get whatever Microchip's engineers decided would happen if you enable weird combinations of them. In any event, the baud rate comes out at around 8x the number passed to `Serial.begin()` - this must be manually corrected for.

#### Synchronous Mode (Sync USART)

To use the synchronous mode, you must do four things:
* You must set pinMode() on the XCK pin to OUTPUT for master, or INPUT for slave modes.
* By default, data is clocked on the RISING edge of a clock that is LOW when idle, and the pin is sampled on the FALLING edge. If the clock is HIGH when idle, and data should be clocked on the FALLING edge, and sampled on the RISING edge of the clock. To achieve that, you must invert the XCK pin. This can be done using the pinConfigure() function, which can also simultaneously set the pinMode to INPUT or OUTPUT `pinConfigure(PIN_HWSERIAL0_XCK_PINSWAP_1, (PIN_DIR_OUTPUT | PIN_INVEN_SET))` will configure the XCK pin of USART0 using alternate pin mapping 1 (`Serial.swap(1)`) for master with inverted clock polarity, and set the pin to the high, , `pinConfigure(PIN_HWSERIAL0_XCK, (PIN_DIR_INPUT | PIN_INVEN_SET))` will configure the XCK pin of USART0 with the default pin mapping (`Serial.swap(0)`) for slave and inverted clock polarity. If you for some weird reason have the pin inverted already, the constant to turn it off is `PIN_INVEN_CLR` (there are several synonyms for all of these constants - see the [Digital I/O Reference](./Ref_Digital.md)).
* If operating as master, **divide the desired baud rate by 8**; do this with the rightshift operator (`>> 3`) to avoid a slow division operation. The desired baud rate must be at most half of the system clock. If operating as slave, it doesn't matter what you pass as baud rate (but the maximum applies to whatever clock the master is generating).
* Add the `SERIAL_MODE_SYNC` constant to the second argument to Serial.begin() by ORing it with the rest of the value, ex: `Serial.begin(100000 >> 3, (SERIAL_MODE_SYNC | SERIAL_8N1));` should start USART0 in sync mode with a baud rate of 100kbaud.

There is a macro which *should* work for this (done as a macro so it doesn't add anything to the class if not used). This should be treated as experimental (the API may change)

```c
syncBegin(port, baud, config, syncoptions);
/*example*/
syncBegin(Serial1, 1000000, (SERIAL_MODE_SYNC | SERIAL_8N1), SYNCBEGIN_INVERT_MASTER)
/*
This will divide the baud rate by 8 for you, call Serial.begin(), and call pinConfigure() with appropriate options to configure the pin as output or input and invert or no-invert if needed.
constants are:
SYNCBEGIN_INVERT_MASTER
SYNCBEGIN_NORMAL_MASTER
SYNCBEGIN_INVERT_SLAVE
SYNCBEGIN_NORMAL_SLAVE
*/
```

That *should* be all that is needed to use Synchronous mode. Note however that this mode has not been tested rigorously. There is one potential issue - they specify that the low 6 bits of the baud register are not used and should be written as zero. Serial won't do that. It is not clear whether those bits, if non-zero, will cause problems or not. If you have problems operating as sync master (probably taking the form of clock generation problems), I would suggest trying `USARTn.BAUD &= 0xFFC0;` after Serial.begin. If it was broken, and that fixed it, or if you have experience with this mode, please let me know so that I can update this document.

If operating as sync. slave, and the master does not provide a clock signal, you will eventually fill the TX buffer. At this point the next character you attempt to write will hang. Check Serial.availableForWrite() before writing to it to ensure that there is room to do so if you think this could be an issue.

Nothing prevents you from combining incongruous options like Open Drain, Loopback, or RS485 mode with SYNC mode - other than fear of the unknown, as it is not always clear what these options would do in combination - Input vs output on the clock pin controls whether the device is master or slave, so ODME is unlikely to give you open drain behavior on XCK as master, since it requires setting TX as input). It may (or may not) be possible to make a sync USART with a clock and a bidirectional data line, and god only knows if RS485 mode does anything - these combinations may not have been considered during design, and so the behavior you get could be the result of how all the behavior they did implement happens to interact when combined in weird ways.

#### Master SPI (MSPI) mode
This mode is very similar to synchronous mode, and even more rarely used - it is most notable in that it lets you configure the clock speed of SPI far more precisely and at much lower speeds than the normal SPI peripheral. The steps to use it are much the same as for synchronous mode:
* You must set pinMode() on the XCK pin to OUTPUT - only master mode is supported.
* If the clock is HIGH while idle, you must invert the pin; just like Sync mode - except now the clock phase is configurable (and the data frame is not)
* Divide the desired baud rate by 8.
* As the options, specify one of the 4 MSPI options listed above. Refer to the discussion of data modes above, or the table below. If this is specified, it will sample on the second transition and transmit on the first, (like SPI modes 1 and 3, or sync USART), otherwise, it will sample on the leading edge.

Character size, stop bits, and parity are not used in MSPI mode

There is also a macro which *should* work for this. It should return 0 on success. This should be treated as experimental (the API may change)
```c
syncBegin(port, baud, config, mspioptions);
/*example*/
syncBegin(Serial1, 1000000, (SERIAL_MODE_MSPI | SERIAL_MSPI_LSB_FIRST), MSPIBEGIN_NORMAL)
/*
This will divide the baud rate by 8 for you, call Serial.begin(), and call pinConfigure() with appropriate options to configure the pin as output and invert or not-invert if needed.
MSPI options is simply:
MSPIBEGIN_INVERT or MSPIBEGIN_NORMAL
*/
```

### Summary of clocking options

| Idle | Leading | Trailing |(Sync)<br/>Invert|(MSPI)<br/>Invert|(MSPI)<br/>PHASE|(SPI)<br/>Mode|
|------|---------|----------|-----------------|-----------------|----------------|--------------|
|  LOW |  Sample | Transmit |       N/A       |              No |             No |            0 |
|  LOW |Transmit |   Sample |              No |              No |            Yes |            1 |
| HIGH |  Sample | Transmit |       N/A       |             Yes |             No |            2 |
| HIGH |Transmit |   Sample |             Yes |             Yes |            Yes |            3 |

**The general rule for inverting is that if it's HIGH when idle, you need to invert the pin.**

## More in-depth background

### Character size
Baud is bits per second. But those are not just data bits. USART data is sent in "frames". These frames start with a "start bit" and end with 1 or 2 stop bits (rarely, 1.5; we don't support that, but some devices to), may or may not have parity, and while the 8-bit byte is almost ubiquitous now, it hasn't always been, leading the the proliferation of character lengths supported. The hardware supports 5, 6, 7, 8 or 9 bit characters. Since the common datatypes on the AVR are 8 or 16 bits, it is awkward to handle such larger size units of data, and impossible to do so efficiently or gracefully. DxCore and megaTinyCore only support 5-8 bit characters, 9-bit serial is vanishingly rare, and with good reason - the overhead to supporting it on a modern system (with 8-bit bytes) is brutal. Its not entirely clear for what applications it *was* intended for, though some sources suggest "industrial multi-drop systems" using the 9th bit to mark "address" bytes were at one point in time common, but what is clear is that it's almost unheard-of nowadays.

The origin of the smaller character sizes though, is clear - they date to the earliest days of teletypes, where all the characters involved were letters anyway, so why would you need more than 32 character codes? Most plain text can be represented with 7-bits , since the standard ASCII character set maps the low 128 characters to (mostly) printable, familiar letters, and that's used almost universally. The upper half is not so universal, and typically consists of regionally specific/language specific glyphs, box-drawing characters, and varies by implementation. Some old applications are still in use that define "binaries" as having 8-bit bytes and "text" as 7-bit.


### Framing and levels
The idle UART is held high by either pullups on the receiving side, or active drive on the transmit side. An Open Drain configuration involves the USART transmitter either asserting the pin low, or releasing it to the pullups to bring it high. The advantage of this is that multiple devices can use one set of wires to communicate, though the top attainable speed is lower. This is how I2C works, too.
Anyway - each frame starts with a "start bit": of normal length. This is always a 0. (so you can see it against the high idle state), then the data bits go out, starting with the smallest (this is the opposite of how I2C and most every other protocol works - when you look at the scope trace of a serial transmission on the 'scope that's why bits are in reverse order - the leftmost edge of a frame is the start bit, followed by the LSB instead of the MSB). After all databits, the parity bit (if any) is sent, followed by the "stop bit[s]" which are always high. Because the stop bits are always high, the same as the idle state, using 2 stopbits is essentially the same as adding a 1-bit-period delay between characters. 2 stopbits should be used when specified by the documentation of what you are talking to, or when you find that 1 or 2 characters in a row are handled correctly, but strings of more than a few are not (if it takes strings of length comparable to the buffers to manifest, that's more likely the problem, but if the boundary between working and not working is on the order of just a few characters, adding another stop bit may help).

Receipt and transmission both depend on the two devices having agreed ahead of time on what baud rate and connection settings to use, and on both having reasonably accurate clock speeds, including that they be running at the speed they think they are. A 16 MHz modern AVR running 115200 might calculate the baud rate to an accuracy of 0.1% - but it will be 25% higher than that if the chip is actually running at 20 MHz.

Stop bits are used detect so-called framing errors which are the cause of the gibberish you have probably seen on serial when the baud rates are mismatched.

### Buffer Size
The hardware itself has a 2-byte buffer on both transmit and receive. When receiving, if both bytes in the buffer are full, a third byte is waiting to be transferred into them, and the start bit of a fourth is detected, data is lost. The core Serial class implements the RXC (Receive Complete) interrupt, and copies received data from the hardware RXDATA register to software implemented ring buffer - unless there is no room in that buffer, in which case data will also be lost. Hence two things will cause data to be lost: Keeping interrupts disabled (including by execution of another interrupt) for longer than the time it takes to receive more than 3 bytes, or allowing the ring buffer to fill up (not using Serial.read() even as Serial.available() reaches the size of the buffer). At very high baud-to-clock ratios, the first possibility becomes precarious. U2X permits baud rates as high as F_CPU/8, so 1 byte (8 bits + 2 framing bits) could come in every 80 clocks. As of the latest versions of the core, the receive complete interrupt, including getting to the interrupt and returning from it takes..... 75 clocks with the ASM RXC enabled. Without ASM_RXC, it's about 89 clocks if there's more than one serial port on the part (single port parts have always been comparable to the ASM RXC implementation - the assembly was used to bypass overhead associated with handling multiple ports without undue flash waste). Thus, as long as interrupts are never disabled while incoming data is arriving, the latest versions will be able to keep up with the maximum hardware-supported baud rate, but older versions or configurations not using the ASM RXC implementation are limited to under 90% of that rate. In both cases, it's far faster than normally encountered unless the clock speed is unusually low.

Similarly transmission is handled through the Data Register Empty interrupt (DRE) and a second ring buffer, on top of the 2 bytes of buffering provided by TXDATA. Unlike receiving, if the ring buffer is full, we can just wait until there is room. This sometimes surprises users who have used the slow 9600 baud (very common in examples) while using very verbose logging. They quickly fill the buffer, and then execution slows such that not more than 960 bytes of debugging information are printed per second. And they can't figure out why it's so slow, so they add more debugging print statements to try to figure it out... These are modern AVRs, there's no reason not to default 115200 baud, which pushes the amount of logging that triggers that sort of thing outside the realm of the normal.

The sizes of the two buffers depends on the size of the memory and which core is in use, and apply to 2.5.0 and 1.4.0 and later; they were different in the past.

|   Part   |  RAM  |  Rx  |  Tx  | Notes                             |
|----------|-------|------|------|-----------------------------------|
| All      | >= 2k | 64b  | 64b  | 16k+ 1/2-series tiny. Dx. Most EA |
| AVR EA   | 1k    | 64b  | 32b  | 8k EA-series parts                |
| tinyAVR  | 1k    | 64b  | 32b  | 8k 2-series, 16k 0-series.        |
| tinyAVR  | 512b  | 32b  | 16b  | 4k 2-series and 8k 0/1-series.    |
| tinyAVR  | less  | 16b  | 16b  | 2/4k 0/1-series.                  |

### Data Rate
The data rate is the total number of bit times per frame: For the most common, 8N1 (8 bit, no parity, 1 stop bit) this is 10 bit times.

While there's always some dead time between bits, that is usually *very* small, and and isn't really relevant. Dividing the baud rate by the bits per frame, including overhead, is an effective any appropriate way to calculate data rate.


### How bad baud rate calculation used to be

[AVR Baud Rate Accuracy Chart](https://docs.google.com/spreadsheets/d/1uzU_HqWEpK-wQUo4Q7FBZGtHOo_eY4P_BAzmVbKCj6Y/edit?usp=sharing)

It was mentioned previously that one of most common places to encounter grossly inaccurate baud rates is classic AVRs. This example illustrates just *how bad* one of the most popular baud rate was on classic AVRs, namely 115200 baud. "Well it says the baud rate can be up to 1/8th the system clock, this is less than a quarter if this, no problem" you think... Nope - on a classic AVR, you've dug a hole, covered it with a tablecloth, and waited until the sun had set to take a walk: You're begging for trouble.

Imagine 4 ATmega328p's - one has a crystal and runs at 16 MHz, and three that all run at 8 MHz - one using a crystal, and the others the internal oscillator, which is 2% fast on one and 2% slow on the other - both easily within spec. The 8 MHz one with the w/crystal can talk to the other 8 MHz ones. The crystal-less ones are on the edge when they try to talk to each other due to the variation between them being about the limit - small temperature differences could push it either way, and since the maximum error isn't quite symmetric (it's easier to receive something if you're 4% too fast than if 4% too slow, according to the datasheet), sometimes the temperatures might conspire such that the fast one could receive from the slow one, but not the other way around (It doesn't quite depend on the "phase of the moon" but if one is near a window, it could depend on whether it's sunny out). The slow crystal-less one can't even talk to a serial adapter, but the other three can, and the fast crystal-less one can also talk to the 16 MHz one, but neither of the other 8 MHz ones can.

Now, imagine you were to reconfigure the serial port on the 16 MHz one, so that it didn't run with U2X enabled. Suddenly, it goes from being 2% fast, to being 3.5% slow - Neither of these is correct, and both of them contribute to problems, but having gotten a net 5.5% slower, it can now talk to the 8 MHz devices no problem!

Now consider dropping a tinyAVR 1/2-series (ex, 1614) in there, and it's a fraction of a percent fast (they usually are 0-0.5% fast. So how far off are the baud rates here?
* The tinyAVR's internal oscillator is between 0.0 and 0.5 % fast; it's baud rate calculation error is negligible.
* The 16 MHz classic is running 2.12% fast assuming perfect clock due to calculation error with U2X - and 3.5% slow without it. Ironically, this will improve communication with the classic AVRs and hinder it with the modern AVR whose baud rate is closest to the "correct" value.
* The 8 MHz one with a crystal is 3.5% slow - but it will do worse than the 16 MHz one at receiving, even though they're both calculating a baud rate 3.5% low), because U2X reduces the allowable baud error. The faster of them is hence net 2.5% slow, while the other one is a mindnumbing 5.5% slow.
* The tiny will likely have no trouble talking the fast crystalless or the 16 MHz with crystal and U2X - though it may have trouble when U2X is turned off.
* The surprise though is that the tiny might have trouble even talking to the 8MHz one with a crystal.

It would come as no surprise to the owner of that hypothetical quarrelsome quartet when their new tinyAVR couldn't talk to some of them. However, someone who had been using classic AVRs with crystals in an existing setup, and they were all talking only to other classic AVRs might be unaware that their "115200 baud" is actually 111111 baud and something with a nearly accurate baud rate, which is off slightly in the other direction, would have problems communicating with them. They would probably blame the modern AVR (I have had that discussion a few times).

In the old days, people would often advise "lower the baud rate" as a solution. This would help baud calculation error, but it was of zero benefit if the oscillator accuracy was the limit, and if the speed was still high enough that calculation error was significant, could make it worse. On classic AVRs, it was not that unusual to find parts where, at certain unfavorable speeds, UART was flaky. What's surprising is that the incredibly popular 115200 and 57600 bauds were some of the worst for 8 and 16 MHz parts! 76800 is nearly perfect on 8 MHz with U2X, and that or twice that are nearly perfect at 16MHz - but instead we use some of the least accuratly calculated clock speeds.

The point of this is to demonstrate by example just how large the error baud rate error was on classic AVRs, due to the way they generated the baud rate from the system clock by integer division - and the only way to fix it on those parts is to use a "UART crystal" and clock the whole chip at some weird speed like 7.37 MHz or 9.21, and so on. This adversely impacts the runtime of `micros()` to a lesser extent, the accuracy of timekeeping in general.

#### Bringing this back to modern AVRs
So, you should all be very, very thankful for the new fractional baud rate generators, which are responsible for the charts linked below being a sea of sub 1% and mostly 0.1% error at most. Here, your problem isn't the baud rate calculation. It's usually not the oscillator either, which is rarely even 1% off on any modern AVR. No, your problem 9 times out of 10, is going to be that the device you're talking to generating an incorrect baud rate, more often than not this encountered when talking to a classic AVR . The path of least resistance with such legacy devices? They probably aren't worth trying to find a way to improve the baud accuracy on. Instead, just nudge the baud rate the modern AVR up or down 2% (whichever fixes it) and make note that that role in the system ought to be on the list to replace with newer hardware.

### Baud rate reference chart
See the [**AVR Baud Rate Chart**](https://docs.google.com/spreadsheets/d/1uzU_HqWEpK-wQUo4Q7FBZGtHOo_eY4P_BAzmVbKCj6Y/edit?usp=sharing) in google sheets for a table of actual vs requested baud rates.

### Final table of this section
**Total Error vs Data Frame Size**
First three columns are normal mode, last three are U2X, which is used if you are within a 2:1 speed increase of the limit of the hardware for your clock speed. U2X is never used any other time by these cores; (It is always used by default on classic parts where the clock calculation accuracy is the limiting factor)

| Data bits per frame |Minimum |Maximum | Recommend |U2X Min | U2XMax | Recommend |
|---------------------|--------|--------|-----------|--------|--------|-----------|
|              5 bits | -6.80% | +6.67% | +/- 3,0 % | -5.88% | +5.66% | +/- 2.5 % |
|              6 bits | -5.88% | +5.79% | +/- 2.5 % | -5.08% | +4.92% | +/- 2.0 % |
|              7 bits | -5.19% | +5.11% | +/- 2.0 % | -4.48% | +4.35% | +/- 1.5 % |
|              8 bits | -4.54% | +4.58% | +/- 2.0 % | -4.00% | +3.90% | +/- 1.5 % |
| (8+1 parity) 9 bits | -4.19% | +4.14% | +/- 1.5 % | -3.61% | +3.53% | +/- 1.5 % |
|(9+1 parity) 10 bits | -3.83% | +3.78% | +/- 1.5 % | -3.30% | +3.23% | +/- 1.0 % |

That table is from the receiver's perspective - notice how baud rate does not appear there: *baud rate only has larger than marginal impact error when it changes baud division calculations*. Otherwise, it's all about the number of bits, and the magnitude of the mismatch (and if U2X is in use - it is favorable if you have larger baud rate calculation error without it, which is often the case on classic AVRs. Classic AVRs always used it on Arduino land... because they desperately needed anything that would lower the baud rate calculation error. On a modern AVR, however, U2X mode is only favorable if you can't otherwise reach the required speed.

### Minimum and Maximum baud rates
Like Classic AVRs the maximum baud rate is F_CPU / 8, using the `U2X` mode, which is still the case.

Unlike classic AVRs, the gap between the theoretical maximum and the practical maximum is much smaller thanks to the fractional baud rate generator". On the older parts there were "gaps" between the closest options. Since the numbers used for standard baud rates don't resemble round numbers, people who needed accurate baud rates would use crystals with bizarre frequencies like 18.42 MHz, so it could be divided down to the match standard baud rates. That - unsurprisingly - led to slower, less accurate timekeeping, where it was supported by an Arduino core at all. Luckily, the days of UART crystals are over! Instead of supplying a whole number, the value passed to the fractional baud rate generator is in 64ths, so as long as it is within the supported range of baud rates, the farthest any two settings are from each other is 1/64, or 1.56%, so the highest baud rate error from the calculation is half that, comfortably within the limits of USART 0.78%. This corresponds to baud rates just below the maximum possible for a given system clock, which for typically used clock speeds is far above what would commonly be used.

#### Maximums
The highest "commonly used" baud rates (almost none of which are commonly used) possible are:
* 6 mbaud can be reached at 48 MHz  (Faster than almost all adapters and too fast for casual wiring, the FT4232HQ claims to support it)
* 5 mbaud can be reached at 40 MHz+ (same as above)
* 4 mbaud can be reached at 36 MHz+ (same as above)
* 3 mbaud can be reached at 24 MHz+ (FT232 maximum speed)
* 2 mbaud can be reached at 16 MHz+ (the maximum speed most serial adapters support; the CH340G claims to support 2 MBaud; I wasn't able to make it work, though)
* 1 mbaud and more importantly, 921600 baud can be reached at 8+ MHz.
* 460800 baud can be reached at 4+ MHz
* 115200 baud can be reached at 1+ MHz

**Note that those are the absolute maximum that the UART peripheral can do**, in practice the speed of the Serial class will limit continuous transmission to lower speeds. At high baud rates (relative to the CPU clock speed), `Serial.print()` may no longer use the buffer, as the relatively slow `print` class begins to take a similar amount of time as sending data down the wire (`Serial.print` - and all things like them are surprisingly slow - it's not just the speed limit imposed by the baud rate); there is no precise set point where this switches - the buffer may be used for short stretches while sending a long string, and print takes different amounts of time for numbers, strings, and so on); the ISR takes similar measures for receiving: after reading the byte and putting it into the buffer, it checks the flag to see if there's any more before returning. At some point below this maximum, the overhead of taking the received character and storing it in memory will take longer than it will for the next character to arrive, at which point characters will be lost if attempting to receive a continuous stream of data at that speed. Use a faster system clock if this becomes a problem; it is unlikely to be encountered for reasonable baud rates except at very low system clock speeds.

Combined with a highly accurate internal oscillator with virtually no voltage dependence, you are essentially guaranteed that UART will work without even resorting to autotune, and cases where such measures are required will be the fault of the other device being way off of what they advertise. The most likely situation which would encounter problems is when trying to communicate with.... a classic AVR operating near it's limits, such as an 8 MHz ATmega328p at 115200 baud. The ATmega328p will actually be speaking 111111 baud even with a crystal due to the clock division described above, which is a difference of 3.55%, which is just on the edge of working. In situations like this, since you know which direction the other device is off, and by approximately how much you can just nudge the clock speed down a bit. If you *don't* know what direction it is off by, only that it is not quite working, if the other device can talk to a serial adapter, you only need to check two options: 2% higher than the nominal baud rate and 2% lower, since the accuracy tolerance of UART serial is around 4%. You will rarely encounter devices where the UART baud rates are so far off that they're on the edge of not working (other than classic AVRs running at 8 MHz and 115200 baud, which is a particularly bad speed for them) but which are not all wrong in the same direction - anything off by more than a few % is probably wrong because of integer-math like classic AVR, rather than oscillator inaccuracy.

#### Minimums
On classic AVRs, minimum baud rate was when the register was at it's maximum of 4095 (F_CPU / 4096 * 16) = 1/65536th of the system clock, so 16 MHz would have a minimum of 244 baud. On modern AVRs the BAUD register can take a full 16-bit value (2<sup>4</sup> times larger than the 12-bit value of classic parts), but needs to be 64 times (2<sup>6</sup>) higher for the same baud rate and system clock. Hence, the minimums are 4 times higher (2<sup>(6-4)</sup> = 2<sup>2</sup> = 4), or 1/16384th of the system clock. This still allows very low baud rates, but whereas on classic AVRs there was essentially nothing that required a baud rate too slow for them to generate, on modern AVRs, that is not so - very rarely, one will encounter something that uses 1200 (or even lower) baud rates:
* 300 baud will work only when F_CPU is no higher than 5 MHz.
* 600 baud will work only when F_CPU is no higher than 10 MHz.
* 1200 baud will work as long as F_CPU is no higher than 20 MHz.


#### Synchronous Mode
Synchronous mode can operate at significantly higher frequencies. Since it does not require the two devices to have matching clocks and baud rates configured, there isn't a list of standard baud rates like for async. modes. The minimum baud rate in synchronous mode as master, and in and MSPI mode, is F_CPU / 2048 - 8 times the minimum asynchronous baud rate - and the maximum is F_CPU / 2 (that is, 8 times the maximum without U2X mode); These speeds are well beyond the range of what one would plausibly want except at extremely low system clock frequencies.

The maximum baud rate while operating as slave in synchronous mode is F_CPU / 4 (2 system clock cycle minimum for each high and low of the clock).

### Common Baud Rates

| Baud Rate |         Popularity | Notes
|-----------|--------------------|---------------------------------------------------------|
|      1200 | rare today, Legacy | A modern AVR cannot generate this above 20 MHz          |
|      2400 | rare today, Legacy | A modern AVR cannot generate this above 40 MHz          |
|      4800 | rare today, Legacy | Sometimes used for niche applications.                  |
|      9600 | V. high in Arduino | V. common default, so slow (1ms/char() that it causes <br/> confusing problems just because of how slow it is)  |
|     14400 |           Uncommon | Was an unpopular modem speed, and rare everywhere else  |
|     19200 |      Fairly common | Used by Arduino ISP, and some bootloaders for upload    |
|     28800 |   Very Rare... now | During the dialup days, this was one of the most common |
|     38400 |           Uncommon | No major uses, but not infrequently seen today          |
|     57600 |             Common | Often used to upload to 8 MHz classic AVRs (a bad choice) |
|     76800 |               Rare | A standard speed, but not commonly used.  This is what <br/> people SHOULD bootload classic 16 MHz AVRs at |
|    115200 |        Very Common | Most common modern bootloader speed, Great default now  |
|    172800 |          Unheardof | You would expect this - but its not actually used anywhere |
|    230400 |           Uncommon | The first non-exotic speed higher than 115200           |
|    256000 |            V. Rare |  Looka like the math would be easy. Nope. Not really.   |
|    345600 |             Exotic | 1.5x 230400, continues the pattern. DxCore uses it <br/> in SerialUPDI, (460.8 exceeds the write speed limit) |
|    460800 |           Uncommon | The next to last of the standard usart speeds.          |
|    512000 |            V. Rare | As with 256k above, it looks more convenient than it is |
|    921600 |               Rare | You don't see this one in use much, that's for sure <br/> Hard to generate from most devices, and very close to 1mbaud |
|   1000000 |           Uncommon | The highest commonly used baud rate. more popular than <br/> 921.6kbaud easier to make by dividing common clocks |

These are some of the most common; Most - but not all - adapters can generate nearly arbitrary baud rates. The CP2102 has a preprogrammed list, though the vendor has a utility that can change them. The HT42B534 has no mechanism for changing the supported baud rates baud rates, etc. Low-end serial adapters usually cap out at 1.5, 2. or 3mbaud. The highest I have seen is 6 mbaud, available only on FTDI's top-end parts, and which is fast enough that the wire's electrical properties that we normally neglect would become relevant; physics would make communication fiddly.

*fun project idea:* Make a true "autobaud" system that monitors listens to serial at an unknown baud rate, figures out what it is, and then configures the Serial appropriate. You'd want the RX line of the serial port, and probably a event input pin piped to a type B timer, such that you could keep measuring to see what the shortest time between two transitions that you see is 1 bit period. 1-over-period = baud rate.


## Appendix A: Notes on the ISR implementation
In order to keep up with the baud rate (to handle the perfectly valid, continuous data streams in both directons), the ISRs for DRE and RXC **must** be exceute very fast. The "stock" implementation in the Nano Every / Uno Wifi Rev. 2 was not very fast. The compiler rendered the C code so inefficently that near the maximum baud rate, neither TX nor RX worked correctly. Not only that, but a combination of a race condition and misuse of chip feature would leave the chip stuck in an infinite loop near the maximum Tx speed. Just below that speed, it was too slow to keep up - you might have asked for 1 mbaud, but would end up with characters transmited at 1 mbaud with a pause betwee them. So the effective speed was lower. For Rx, a continuous datastream would simply miss characters. The former was absolutlely unacceptable (especially since it was undocumented and dependent on timing), the latter resulted in an artificially low maximum baud. To address these, as well as the wasted flash cause by 1 copy of each ISR per serial port (particularly problematic on, say a 424), a complicated series of inline asembly blocks was used with a few particularly unusual "tricks": First, all the ISRs are "naked" - with no prologue or epiloge, the blob of inline assembly. It pushes a single register, and neither changes nor saves SREG. A value of twice the USART number is loaded into the register that was pushed, and then it jmps/rjmps to a shared handler function. This function is also declared `naked`, as well as `used` to keep the compiler from optimizing it away. It knows which register contains twice the USART number, used to find the start of the Serial instance and from there, fi, and it is there that the full prologue, epilogue, and reti are located (along with, of course, . The
