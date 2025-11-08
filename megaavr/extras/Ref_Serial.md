# Hardware Serial reference (AKA UART or USART)
All of the supported parts contain USART serial ports. We all use them every day, but not all of us are familiar with the details of these. While USART, SPI, and TWI are all serial protocols, "Serial" without qualification almost universally refers to a USART. These are the only protocol for which USB adapters are widely available and ubiquitous. SPI and I2C USB adapters exist - but they aren't standard and generally require some measure of drivers. And while there are countless applications that will act as a serial console, software for I2C and SPI, is usually vendor specific if it has been written at all.

On all supported devices, where the appropriate pins are present, they can be pin-swapped, and on DA and DB parts, each port except PORTD gets a USART, which defaults to pins 0, 1, 2 and 3 (TX, TX, XCK and XDIR), and 4, 5, 6 and 7 when pinswapped. This is configured using the Serial.swap() or Serial.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling `Serial.begin()`

## Background - quick overview of how serial is different
In addition to being by far the most popular interface for Arduino users, rthere are several things that really set it apart... and these have consequences, for better and worse. First the vital stats:
* It uses 2 wires, transmit, and receive. Transmit of one connects to receive of the other (that is, they cross over. Sometimes connectors are marked relative to the device, other times to the device you connect to it; because of this, devices often extend "TX" and "RX" to "TXO" (Transmit Out) and "RXI" (Receive In).
* It is operated at a selectable baud rate, from incredibly slow speeds like 300 baud from early teletypes up to several megabaud, but in practice, USART is normally used between 9600 baud and 1mbaud.
* *There is no clock* - timing is used to differentiate bits. That presents some problems, as *the two sides must have agreed on the baud rate* and have clocks that agree.
  * Mismatched baud rates are generally interpreted as gibberish with lots of non-printing characters, with framing errors being reported (see Serial.getStatus())
* It's not just the baud either that needs to be agreed on - while usually we transmit characters consisiting of 8-bit characters, serial can be used with as few as 5 bits - and 7-bits is sufficient for all the normal letters, numbers and symbols in the ASCII character encoding. While the hardware supports using as many as nine characters, that is not supported by this implementation - it would double the memory footprint of the buffers, and rather than characters, you'd get numbers between 0 and 511; Sometimes the 9th bit has been used as some sort of indicator - say to mark a control code, while still using normal bytes for data; this requires special handling by the serial code, and would require a custom or third party library.
* The data lines are active low, which can allow for half duplex single wire communication with an open drain output, or even multidrop open drain networks.
* There are a great many protocols with a different PHY layer but the same format; adapters to convert 5v serial into these are common - RS485, LIN, and RS232 are common examples.
* The word "Serial" itself can mean two different things, as well, depending on context. in a generic sense, a serial protocol is one that sends bits one at a time, in contrast to a parallel interface, which has multiple wires which each simultaneously transmit 1 bit. However, more often, "serial" refers to a specific type of serial interface, properly known as a UART or USART. So while I2C and SPI are both serial interfaces in the first sense (as is USB, ethernet, and many other electronic interfaces we interact with on a daily basis), when someone talks about using "Serial" it a near certainly that they're talking abou5t a UART or USART port specifically.

For a more detailed description of how USART Serial works, see the detailed background section closer to the end of this document.

### UART vs USART

UART stands for "Universal Asynchronous Receiver Transmitter"

USART is "Universal Synchronous/Asynchronous Receiver Transmitter"

A UART can only communicate in asynchronous mode (with TX and RX).

A USART can either do that, or operate in synchronous mode, with one of the devices (the "master") generating the clock on a third pin called XCK. This eliminates the possibility of baud rate mismatch - but it is rather awkward to work with, and the "slave" device cannot send any data out if the clock is not presest, so the clock is generally supplied continuously, which is inefficient, and it requires another wire.

The key way in which this differs from SPI is that in SPI the master only generates a clock when transmitting data, so the slave transmits one byte for every byte that the master does, while in Sync USART, the clock is continuously present, and either device can send data when it pleases - additionally, the data framing is still used, unlike SPI. This mode is very rarely used, and in casual conversation, the distinction often isn't made.

AVRs can also use TX, RX and XCK in "MSPI" mode, where the pins act like the MISO, MOSI, and SCK pins of an SPI interface in master mode.

These modes are supported by the megaTinyCore and DxCore HardwareSerial class; they are not supported by most Arduino cores.

### Terminology (non-standard)
* "Pinset" - synonym for "pin mapping option" and other descriptive terms, consisting of all the pins that could be enabled with a given PORTMUX configuration. These are generally consecutive series of pins - TX, RX, XCK and XDIR, in that order.
* "Mux Code" - For a given pinset, the mux code is the value written to the appropriate bits of the PORTMUX register controlling USART pin selection. It is a value of 0, 1, 2, or 3 leftshifted some number of places. For the same pinset, for the same peripheral, with the same swap level, different parts may have a different mux code.
* "Swap Level"  - The swap level is the logical number of the pinset - the default mapping is always 0, swap level n is the option named as ALTn in the datasheet. Microchip has never articulated any policy of keeping pinsets consistent between parts and on future parts but all indications are that they intend to keep pinsets consistent, and only add new ones. The mux codes are, however, not treated with the same level of respect - it's likely that the D

For example, TX = PC4, RX = PC5, XCK = PC6, XDIR = PC7 is pinset 1 for USART1.
It's swap level is 1, and it's mux code is is 0x04 (on DA/DB parts - it will presumably be 0x08 on the EA-series, assuming it stays like the product brief describes, with the DD-series mux options).

## Serial in DxCore and megaTinyCore
We have tweaked the Serial classes for greater flexibility, efficiency, and performance.
It implements the standard Serial API, plus a number of additional options.

### Selecting pins
There are very few serial ports on modern AVRs that don't have at least two available pinsets that they can be used on (the exceptions are all cases where there would be alternete pins, but the part in question is not the largest pincount in the family, and doesn't have the alternate pins. This is true for USART2 on DA and DB parts with less than 48 pins, for example). Pins are selected by using the Serial.swap() (which uses less flash an executes faster, but requires you to know which "swap level" you want) or Serial.pins() which takes the tx and rx pin, which must correspond to the same swap level.

Note that it is not practical to confirm that no other peripheral is using a pinset. Presumably one of them will

#### Serial.swap(mux option)

#### Serial.pins(tx, rx)

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

#### Serial.getPin(pin_name)
Returns the pin corresponding to that function with the current mux option.
```c
PIN_SERIAL_TX
PIN_SERIAL_RX
PIN_SERIAL_XCK
PIN_SERIAL_XDIR
```
**Warning** - on DD-series parts, this may return an inaccurate values in two specific cases. For the case of USART0 mapping 4, XDIR (that pin does not exist, but we return the the non-existent pin instead), and for USART1, mapping 0, TX on 14/20 pin parts (that pin doesn't exist, but we return it anyway).

This function is meant for use by libraries to allow them to figure out which pin is in use without duplicating the logic we have already implemented.



### Serial.begin(uint32_t baud, uint16_t options)
This starts the serial port. The second argument is optional - if not specified, you get 8-bit characters, 1 stop bit and no parity. Since that's normally what you want, Arduino users aren't even aware that begin can take a second argument,

#### Basic USART options
The options field takes a 16-bit value. The low 8 bits configure the "basic" uart parameters: These are the number of bits per character, whether to use 1 or 2 stop bits when transmitting, and with to use no parity, even parity, or odd parity.

**Parity**: if used, this must be set up in the same way on both the sender and receiver. It adds an additional bit to each character which is used to verify that the character was received correctly. This is far from foolproof - it's very good at catching single bit errors caused by noise, but errors that flip two bits (or, in fact, any even number of bits) will not be caught). To conform to the behavior of the standard Arduino Serial implementation, unlike framing errors, if a parity error is detected, the character, known to be erroneous, is discarded.

**Stop Bits**: Since the idle state and a stop bit are both HIGH, using 2 stop bits is equivalent to pausing for an extra bit period between each character sent. This is of great utility (necessity, often) in two situations:
1. When required by the receiving device according to it's documentation. Some devices (particularly ones with software serial implementations) need an extra moment to set themselves back up for the next byte. An extra stop bit gives them that chance (normally, in software serial, you stop sampling when you read in the middle of the stop bit, and try to dothat cleanup in the remaining 1.5 bit periods), otherwise these marginal software serial implementations would struggle with twp consecutive characters.
2. When you are using non-ideal clock source, *and* you wish to send long blocks of continuous data (including excessively verbose console logging. Note: Unless you broke into God's stock room and style some His clock generators (He won't be happy when he finds out, which he already did because he's omniscient - so you can't hide, and he's omnipotent, so you can't run. Overall, stealing stuff from God(s) is not recommended - didn't work for Prometheus, and it won't work for you.

These basic options all conveniently reside in a single register.

| Data Size | Parity | 1 stop bit | 2 stop bit |
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
The modern tinyAVR and AVR Dx-series parts have a number of additional features. A few of these were available on classic AVRs (and just not exposed), but most of them are new.
To use these, they should be coombined with one of the options from the table above using the bitwise or operator. More than one of these modifiers can be used, though many of them do not make sense in combination. When using the two argument form of Serial.begin(), remember to pass the constant (such as SERIAL_8N1) not just a modifier.

```C
Serial.begin(115200, (SERIAL_8N1 | SERIAL_TX_ONLY));
Serial1.begin(9600, (SERIAL_HALF_DUPLEX | SERIAL_RS485));
```

* SERIAL_OPENDRAIN    - Sets port to open-drain mode
* SERIAL_LOOPBACK     - Enables single wire operation and internally connects tx to rx.
* SERIAL_TX_ONLY      - Enables only Tx.
* SERIAL_RX_ONLY      - Enables only Rx.
* SERIAL_EVENT_RX     - Enables the event input
* SERIAL_HALF_DUPLEX  - Synonym for (SERIAL_OPENDRAIN | SERIAL_LOOPBACK)
* SERIAL_MODE_SYNC    - Uses synchronous mode instead of asynchronous. See notes below, additional configuration required.
* SERIAL_RS485        - Enables RS485 mode.
* SERIAL_RS485_OTHER  - Enables the "other" RS485 mode, whatever that is (see note)

Note:
The "other" RS485 mode, according to the ATtiny3216/3217 datasheet:
"Writing RS485[1] to `1` enables the RS-485 mode which automatically sets the TXD pin to output one clock cycle
before starting transmission and sets it back to input when the transmission is complete."

Obviously this begs the question of how any of the devices involved are supposed to prevent collisions - I don't think there *is* a way. That would explain why this feature was removed from the Dx-series documentation. I was suspecting this was going to get quietly killed, but it seems to in fact be coming back with the V2 USART debuting on the LA and likely EC-series parts, under the name of hardware flow control.

Yes, serial will need total reimplementation.



### Serial.printHex() and Serial.printHexln()
One extremely common task in embedded programming, particularly debugging embedded code, is printing data out as hexadecimal numbers. There is of course,  `Serial.print(number,HEX)`, but not only does that burn more flash, it doesn't add an appropriate number of leading zeros (making it hard to read). It's designed to print numbers in the way that programmers would want them printed - the number of leading zeros will match the data type, ie if you print an unsigned long, with 1 in the low byte and 0's in the other three, it will print 00000001, not 1. As you would expect, printHexln() does the same thing and adds a newline.
Below is an unabridged list of the versions:
```c++
    void                  printHex(const     uint8_t              b);
    void                  printHex(const    uint16_t  w, bool s = 0);
    void                  printHex(const      int8_t  b)              {printHex((uint8_t  )     b);        }
    void                  printHex(const        char  b)              {printHex((uint8_t  )     b);        }
    void                  printHex(const     int16_t  w, bool s = 0)  {printHex((uint16_t )  w, s);        }
    void                  printHex(const    uint32_t  l, bool s = 0)  {_prtHxdw((uint8_t *) &l, s);        } // _prtHxdw() is a private member function
    void                  printHex(const     int32_t  d, bool s = 0)  {_prtHxdw((uint8_t *) &d, s);        } // that prints a 4 byte type from a pointer,
    void                  printHex(const       float  f, bool s = 0)  {_prtHxdw((uint8_t *) &f, s);        } // and these function definitions pick
    void                  printHex(const      double  f, bool s = 0)  {_prtHxdw((uint8_t *) &f, s);        } // out the argument types it can use and
    void                printHexln(const      int8_t  b)              {printHex((uint8_t )b   ); println();} // make them pointy.
    void                printHexln(const        char  b)              {printHex((uint8_t )b   ); println();}
    void                printHexln(const     uint8_t  b)              {printHex(          b   ); println();}
    void                printHexln(const    uint16_t  w, bool s = 0)  {printHex(          w, s); println();}
    void                printHexln(const     int16_t  w, bool s = 0)  {printHex((uint16_t)w, s); println();}
    void                printHexln(const       float  f, bool s = 0)  {_prtHxdw((uint8_t *) &f, s); println();} // Why float and double? Apparently, the
    void                printHexln(const      double  f, bool s = 0)  {_prtHxdw((uint8_t *) &f, s); println();} // compiler knows the difference.
    void                printHexln(const     int32_t  d, bool s = 0)  {_prtHxdw((uint8_t *) &d, s); println();} // with just float, it can't tell if an
    void                printHexln(const    uint32_t  l, bool s = 0)  {_prtHxdw((uint8_t *) &l, s); println();} // a "long" "unsigned long" or "float"
    uint8_t *             printHex(          uint8_t* p, uint8_t len, char sep = 0            );
    uint16_t *            printHex(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    volatile uint8_t *    printHex(volatile  uint8_t* p, uint8_t len, char sep = 0            );
    volatile uint16_t *   printHex(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
```

There are two particular features worth noting in addition to the correct number of leading zeros, and the fact that it is not horrendously bloated like full serial print.
1. For 16 and 32-bit datatypes, you can pass a boolean as the second argument. If it is true, the endianness will be reversed.

2. You can also pass a pointer to either a uint8_t or a uint16_t variable (likely part of an array). In this case the arguments are:
```c
uint8_t *  printHex(uint8_t * p, uint8_t len, char sep = 0);
uint16_t * printHex(uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
```

It will print `len` elements starting from the address the pointer is pointed at, if `sep` is non-zero, that character will be placed between each byte or word - these are **characters** not strings. A single character, enclosed between a pair of single quotes ":" is a 2 character string - a colon followed by a null terminator, and is invalid. Anything between double quotes is invalid. Use single quotes as shown below. If `s` is true for a pointer to 16-bit values, the endianness will be swapped as well. There is a slightly different implementation for pointers to volatile variables to help prevent problems when using this to dump the contents of peripheral registers. Which is what printHex was made for.
```c
  // dump every register associated with the CCL
  volatile uint8_t * cclconfig= (volatile uint8_t*)&CCL;
  cclconfig = Serial.printHex(cclconfig, 0x08, ':');  // per datasheet register summary, first 8 are either reservedbytes or effect all LUTs
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
Many peripherals have a couple of 16-bit registers, amongst a sea of 8-bit ones. Say I'm trying to debug a problem involving the synchronous mode serial (because recently I was), it's got a baud register in the middle that I'd like to have formatted nicely. A union of a word and a byte pointer is just the thing here:
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
This starts the serial port. Options should be made by combining the constant referring to the desired character size, parity and stop bit length, zero or more of the modifiers below

#### Basic USART options
| Data Size | Parity | 1 stop bit | 2 stop bit |
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

**Tip**: SERIAL_8N2 is much more robust to clock variation than SERIAL_8N1. I advocate defaulting to using two stop bits unless there's a compelling reason not to (there almost never is, because any signal that could be output with 2 stop bits selected could also have been output idf only 1 stop bit was selected, if you assume that

#### Modifiers
* SERIAL_RS485        - Enables RS485 mode.
* SERIAL_RS485_OTHER  - sets the othger RS485 bit (if present) or attempts to (if not). It is unclear whether this bit is setable and if settable, whether it has any function. This feature returns as hardware flow control in the refreshged USART coming on the LA.
* SERIAL_OPENDRAIN    - Sets port to open-drain mode
* SERIAL_LOOPBACK     - Enables single wire operation and internally connects tx to rx.
* SERIAL_TX_ONLY      - Enables only Tx.
* SERIAL_RX_ONLY      - Enables only Rx.
* SERIAL_EVENT_RX     - Enables the event input
* SERIAL_HALF_DUPLEX  - Synonym for (SERIAL_OPENDRAIN | SERIAL_LOOPBACK)
* SERIAL_MODE_SYNC    - Uses synchronous mode instead of asynchronous. See notes below, additional configuration required.

#### MSPI options
* SERIAL_MSPI_MSB_FIRST
* SERIAL_MSPI_LSB_FIRST
* SERIAL_MSPI_MSB_FIRST_PHASE
* SERIAL_MSPI_LSB_FIRST_PHASE


As the second argument to begin, you should pass the modifiers bitwise or'ed with any modifiers.
`Serial1.begin(115200,(SERIAL_8N1 | SERIAL_OPENDRAIN | SERIAL_RS485 ))`

If you use the two argument form of Serial.begin() be certain to remember to pass the constant, not just a modifier.

### Autobaud mode
As of 1.5.0, generic autobaud can be enabled. This can be done either through bitwise OR with SERIAL_AUTOBAUD, or using SERIAL_MAKE_AUTOBAUD(baud), or by passing SERIAL_DEMAND_AUTOBAUD as the baud rate, which makes it use the maximum baud rate so any sync field will be seen without having to generate a long break. It's important to keep in mind that both sides must understand that this is the case. There are at least 6 general approaches:
1. Use SERIAL_AUTOBAUD on the autobaud device. The other device MUST send a sync.
2. Use SERIAL_AUTOBAUD_START on both. Both devices can talk at default speed, but if they see framing errors, they should `Serial.doubleBreak(); Serial.write(0x55);` which will be seen as a sync, and the other baud rate will now match.
3. Use SERIAL_AUTOBAUD on one device, other uses fixed baud rate unknown to first device. Other device must start initial communication with a simple-sync (`Serial.write(0x00),Serial.write(0x55)`)
4. Use SERIAL_AUTOBAUD_START on one device, other uses fixed baud rate. If non-autobaud device gets framing errors, it it could be either slow or fast, so use `Serial.doubleBreak(); Serial.write(0x55);` to sync
5. Use either mode on autobaud device. If  SERIAL_AUTOBAUD used, communication must start with simple-sync as above. Otherwise they begin talking at their preconfigured speeds that must match. Under conditions determined by the programmer, the devices must agree when to switch baud rate. Autobauder will then WFB and await sync simple-sync.
6. A kludgey multidrop one-to-many master-slave topology is in use. All slaves use SERIAL_AUTOBAUD. Master begins communication with a simple-sync sent to all slaves, who now know the baud rate. If master sees framing errors, `Serial.doubleBreak(); Serial.write(0x55);` will resync.
There are doubtless others. Autobaud device can be made more forgiving by reacting to framing errors by emptying the receive buffer (which since it's getting framing errors is full of gibberish), setting WFB, and then doing something like `Serial.sendTestFrams();` to hopefully trigger a framing error on the other device. The other device should wait 1 byte-period, and then empty it's receive buffer (full of gibberish because framing errors) and send a sync when it gets a framing error.

```c++
Serial.begin(SERIAL_DEMAND_AUTOBAUD); // Sets serial port to an invalid (far too fast) baud. Other device expected to send SYNC as first message.
Serial.begin(SERIAL_AUTOBAUD | 115200); // starts serial at 115200, but in autobaud mode, so if it receives framing errors, it can trigger a WFB. It should also send something at that point which the target will see as a framing error (ex: Serial.autobaudWFB(); Serial.write(0x00); Serial.write(0x00);; This will result in 0 0000 0000 1 0 0000 0000 1 on the wire. Framing error is when it sees a non-1 stop bit, indicating baud rate mismatch. Unless baud rates are matched or the the other baud rate is half of this one or less, it will see framing error. The nulls must be sent consecutively.
```

#### Three approaches to getting the sync for autobaud
Which one is appropriate will depend on the needs of your application.

##### Serial.simpleSync()
Should only be called by the device talking TO a device with autobaud enabled
This clears anything in the RX buffer (should be nothing) sends a 0x00 followed by a 0x55 sync char (`Serial.write(0x00); Serial.write(0x55)`). It is appropriate only when you know the other device has just started up with SERIAL_DEMAND_AUTOBAUD, or has set WFB.

#### `Serial.breakAndSync(uint8_t factor = 1)`
Should only be called by the device talking TO a device with autobaud enabled.

Clears the receive buffer (which if sync is needed, we know contains garbage. A break is then generated by inverting the pin, waiting 8ms, and then uninverting it (this is long enough that it's virtually guaranteed to be seen as a break) and send a sync character.

##### `Serial.autobaudWFB()`
Should only be called by the device using autobaud.

The next low of any length received will be treated as a break and expected to be followed by a sync field. Somehow the other device must know when to do this.

##### `Serial.autobaudWFB_and_request(n = 2)`
Should only be called by the device using autobaud

This will execute Serial.flush() to clear the transmit buffer, if not already empty (which it hopefully should be) then set Serial.autobaudWFB() and send two (or more) nulls. The application code on the other side should see the framing error, and must reply with a sync.

##### `Serial.waitForSync()`
Can be called by the device that has autobaud enabled (and may have set WFB) to wait up to 8ms for an expected sync packet. It keeps track of how long it was in that loop and will set millis() ahead by 1 ms less than that period of time (since the interrupt flag will also be set, causing the interrupt to fire immediately upon reenabling interrupts)

##### See Serial.getStatus() below
This is critical for both sides when using autobaud

### Serial.getStatus()
This function returns a byte encoding the current status of the serial port.
| Bit   | Cleared? | Constant name              | Notes
|-------|----------|----------------------------|--------------------------------------
| 0     | No       | SERIAL_WRITTEN             | Serial port has transmitted any data since begin() was called. Used internally.
| 1     | No       | SERIAL_HALF_DUPLEX_ENABLED | Serial port is operating in half duplex mode. Used internally.
| 2     | Yes      | SERIAL_PARITY_ERROR        | A Parity Error has been encountered since the last time this was called. This clears that record.
| 3     | Yes      | SERIAL_FRAME_ERROR         | A Framing Error has been encountered since the last time this was called indicating baud mismatch. This clears that record.
|4,5=00 | No       | SERIAL_AUTOBAUD_DISABLED   | Bits 4 and 5 are combined to express autobaud conditions. 00 = autobaud not enabled
|4,5=01 | No       | SERIAL_AUTOBAUD_ENABLED    | 01 = Autobaud enabled. Nothing else to report
|4,5=10 | No       | SERIAL_AUTOBAUD_SYNC       | 10 = Autobaud enabled. The last character received was a sync character and we are now operating at a new baud rate.
|4,5=11 | Yes      | SERIAL_AUTOBAUD_BADSYNC    | 11 = Autobaud enabled. An inconsistent sync field has been received. If this is the case, we clear the error flag. <br/> Additionally, due to a widespread errata, we must also turn off RXEN and turn it back on to restore RX functionality. As the extent of this erratum is not clear, we do the workaround everywhere unless we learn that some parts are definitely not impacted.
| 6     | Yes      | SERIAL_OVERFLOW_RING       | Indicates that the *RING BUFFER* filled up and characters were dropped (because data was coming in, but application was not calling Serial.read() often enough to keep up). getStatus clears that record.
| 7     | Yes      | SERIAL_OVERFLOW_HARDWARE   | A buffer overflow at the hardware level has occurred; This happens when interrupts are disabled for too long while receivign data. getStatus() clears this record

Since getStatus also clears the errors, be sure to store the first value you get from it if you are looking for multiple errors.
In the case of autobaud, both sides should probably be using this - non-autobaud device would check for framing errors that indicate a need to sync, and then attempt to do so, while the autobaud device would need to watch out for ISFIF, which disables receiving until addressed.

### Loopback Mode
When Loopback mode is enabled, the RX pin is released, and TX is internally connected to Rx. This is only a functional loopback test port, because another device couldn't drive the line low without fighting for control over the pin with this device. Loopback mode itself isn't very useful. But see below.

### Open Drain Mode
In Open Drain mode, the TX pin will no longer drive high. The pin must not be set as OUTPUT. If you don't fiddle with it, this will be correctly handled for you. This is only occasionally useful on it's own, though it does provide a simple and inexpensive way to interface with something at a lower voltage level. This works fantastically on AVR Dx-series (except for the poor DA - first and worst), since they have INLVL, and can set I/O pins to use "TTL" levels - this allows you to communicate reliably (assuming you could without the voltage difference) as long as the HIGH gets to at least 1.8v, regardless of what voltage the chip is running at. However, where it really shines is in combinations.

### RS485 Mode
In RS485 mode, at initialization, the HWSerial class configures the XDIR pin as an OUTPUT. 1 bit-time prior to sending any data, the XDIR pin will be driven HIGH. If using RS485, this should be connected to the external line driver IC to enable transmit. XDIR will be lowered 1 bit-time after the last bit has been sent. If you require the opposite polarity, simply set PORTx.PINnCTRL |= PORT_INVEN_bm;
or use `pinConfigure()` [See Digital I/O Reference](Ref_Digital.md)

RS485 mode in combination with RX_ONLY will simply set the pin to an output, but never use it, because the TX module isn't enabled.

#### These options were meant to be combined
* Loopback + Open Drain - These two options, neither of which is earthshattering on it's own. Open drain is useful on it's own, but not terribly often. But when you combine loopback and open drain, you have a half duplex single wire serial port. This is quite common now. In fact I bet you've used or will use one within a few hours of reading this document: this is exactly how UPDI is implemented! (as far as I can tell, it's essentially a serial port with most bits hardwired, complete with the quirks of a normal serial port, like the implicit 2 byte RX buffer (actually makes a *big* difference when writing to a Dx-series - without that, uploads would need to be significantly slower - except instead of talking to the chip itself, it talks to some supervisor portion of the chip that has the power to force resets, write fuses and flash and so on. It also has a hardware debugging functionality, but they don't publicly release the protocol, so you're forced to use the official tooling). But in any event - you'll see implementations of half duplex UARTs all over the place, and sooner or later, you'll probably end up making one even when you control both ends of the connection, to cut the pincount.
* Loopback + Open Drain + RX485: In this mode, it will work perfectly for the case where there is an external line driver IC but it has only a single TX/RX combined wire and a TX_Enable pin (terminology may vary). This configuration is probablty more common than full duplex RS485 by a large margin. You almost never see more than 1 differential RS485 pair set up.

#### Half-duplex schemes change the behavior of Serial in important ways
Normally, RX functionality is not disabled unless the user specifically requests it. Bytes received at any time will be placed into the buffer by the USARTn RXC interrupt as long as it is not full. With loopback mode enabled, you receive all the characters you transmit. That's fine for just loopback - since TX is actively driven high when idle, you can't exactly receive data any other way. When Open Drain mode is also active, though, the stuff that you sent would end up intermixed with actual received data. This is not very helpful (It would be nice to check the received data for to ensure there were no collisions. However, this is challenging since you'd have to keep a record of what you sent to compare it to. There are almost 3 bytes of buffer (receive is double-buffered, plus the incoming shift register; but transmit is handled by the DRE interrupt, while receive is handled by the RXC interrupt, so it would need to implement ring buffer that both of these could access.... But those are written in assembly with n alarming small number of clockcycles after the end of the ISR before the next character has to be dealt with (see Appendix A). So under these circumstances behavior regarding RX is altered slightly. Rigorously, the condition is when both TX and RX are enabled, loopback mode is set:

```text
CTRLA: LBME
CTRLB: ODME and TXEN and RXEN
```

In this case, Any *write* will temporarily disable the RXC interrupt, and enable the TXC interrupt. When the TXC interrupt executes indicating that all data has been sent, it will read RXDATAL until the RXC flag is cleared (to flush out the characters you sent) discarding the values, and enable the RXC interrupt again. We considered the idea of checking the received data against the sent data, but that turned out to present profound difficulties.

That configuration will result from calling the two argument version of begin() with SERIAL_OPEN_DRAIN and SERIAL_LOOPBACK, or equivalently, SERIAL_HALF_DUPLEX, and neither SERIAL_TX_ONLY nor SERIAL_RX_ONLY.

### Inverted Serial
Rarely, one needs to have *inverted* serial, ie, idle line is low, the start bit is high, high bits are 0, low bits are 1 and the stop bit is low.) This can be achieved by inverting the port (either manually, `PORTx.PINxCTRL |= PORT_INVEN_bm;` or via pinConfigure() - [see Digital I/O Reference](Ref_Digital.md) . Generally, when one of the pins is inverted, the other one is to, so you probably want to invert both TX and RX, and you probably don't want the pullup on either of them, since they lines are idle LOW when inverted.

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
* Divide the desired baud rate by 8 or use the syncBegin()
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
#### MSPI options
* SERIAL_MSPI_MSB_FIRST
* SERIAL_MSPI_LSB_FIRST
* SERIAL_MSPI_MSB_FIRST_PHASE
* SERIAL_MSPI_LSB_FIRST_PHASE

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
Anyway - each frame starts with a "start bit": of normal length. This is always a 0. (so you can see it against the high idle state), then the data bits go out, starting with the smallest (this is the opposite of how I2C and most every other protocol works - when you look at the scope trace of a serial transmission on the 'scope the bits will be in reverse order - the leftmost edge of a frame is the start bit, followed by the LSB instead of the MSB). After all databits, the parity bit (if any) is sent, followed by the "stop bit[s]" which are always high. Because the stop bits are always high, the same as the idle state, using 2 stopbits is essentially the same as adding a 1-bit-period delay between characters. 2 stopbits should be used when specified by the documentation of what you are talking to, or when you find that 1 or 2 characters in a row are handled correctly, but strings of more than a few are not (if it takes strings of length comparable to the buffers to manifest, that's more likely the problem, but if the boundary between working and not working is on the order of just a few characters, adding another stop bit may help - it would imply that the receiver clock is running more slowly than the transmitter, and it is just barely working for a single character, but when a second frame immediately follows one, the receiver, because it thinks it should still be in the stop bit, doesn't detect the startbit as soon as it otherwise would have, which adds to the baud mismatch).

Receipt and transmission both depend on the two devices having agreed ahead of time on what baud rate and connection settings to use, and on both having reasonably accurate clock speeds, including that they be running at the speed they think they are. A 16 MHz modern AVR running 115200 might calculate the baud rate to an accuracy of 0.1% - but it will be 25% higher than that if the chip is actually running at 20 MHz (this can happen on tinyAVRs when using a bootloader. UPDI uploads will always set the fuse to the appropriate base oscillator speed, but there is no mechanism to prevent uploading a binary compiled for a 16-MHz-derived clock speed to a chip set for 20 MHz, and the chip cannot self program it's fuses. It is usually easy to figure out when this is what's going on once you know it's a possibility.

Note that framing errors cannot always be detected - it only knows a framing error has happened when it expects a stop bit but sees that the line is low.  In, for example, an autobaud situation, where one of the devices checks Serial.getStatus for framing errors, and tries to resync, sending several NULLs in a row (0x00) gives a pattern of 9 low bit periods with only the single stop bit between them. If the speeds are not matched, that is the pattern most likely to generate a framing error on the other device.

### Buffer Size
The hardware itself has a 2-byte buffer on both transmit and receive. When receiving, if both bytes in the buffer are full, a third byte is waiting to be transferred into them, and the start bit of a fourth is detected, data is lost. The core Serial class implements the RXC (Receive Complete) interrupt, and copies received data from the hardware RXDATA register to software implemented ring buffer - unless there is no room in that buffer, in which case data will also be lost. Hence two things will cause data to be lost: Keeping interrupts disabled (including by execution of another interrupt) for longer than the time it takes to receive more than 3 bytes, or allowing the ring buffer to fill up (not using Serial.read() even as Serial.available() reaches the size of the buffer). At very high baud-to-clock ratios, the first possibility becomes precarious. U2X permits baud rates as high as F_CPU/8, so 1 byte (8 bits + 2 framing bits) could come in every 80 clocks. As of the latest versions of the core, the receive complete interrupt, including getting to the interrupt and returning from it takes..... 75 clocks with the ASM RXC enabled. Without ASM_RXC, it's about 89 clocks if there's more than one serial port on the part (single port parts have always been comparable to the ASM RXC implementation - the assembly was used to bypass overhead associated with handling multiple ports without undue flash waste). Thus, as long as interrupts are never disabled while incoming data is arriving, the latest versions will be able to keep up with the maximum hardware-supported baud rate, but older versions or configurations not using the ASM RXC implementation are limited to under 90% of that rate. In both cases, it's far faster than normally encountered unless the clock speed is unusually low.

Similarly transmission is handled through the Data Register Empty interrupt (DRE) and a second ring buffer, on top of the 2 bytes of buffering provided by TXDATA. Unlike receiving, if the ring buffer is full, we can just wait until there is room. This sometimes surprises users who have used the slow 9600 baud (very common in examples) while using very verbose logging. They quickly fill the buffer, and then execution slows such that not more than 960 bytes of debugging information are printed per second. And they can't figure out why it's so slow, so they add more debugging print statements to try to figure it out... These are modern AVRs, there's no reason not to default 115200 baud, which pushes the amount of logging that triggers that sort of thing outside the realm of the normal (okay, no it doesn't, because people print absurdly verbose statements in arduinoland.

The sizes of the two buffers depends on the size of the memory and which core is in use, and apply to 2.5.0 and 1.4.0 and later; they were different in the past.

|   Part   |  RAM  |  Rx  |  Tx  | Notes                             |
|----------|-------|------|------|-----------------------------------|
| All      | >= 2k | 64b  | 64b  | 16k+ 1/2-series tiny. Dx, Ex.     |
| ~AVR EA~   | ~1k~    | ~64b~  | ~32b~  | ~8k EA-series parts~              |
| tinyAVR  | 1k    | 64b  | 32b  | 8k 2-series, 16k 0-series.        |
| tinyAVR  | 512b  | 32b  | 16b  | 4k 2-series and 8k 0/1-series.    |
| tinyAVR  | less  | 16b  | 16b  | 2/4k 0/1-series.                  |

AVR Dx seems to get 4k RAM at 32k Flash, 2k at 16k flash, and have never been announced in an 8k size.

AVR EA was originally announced with an 8k size, but it was canceled. I suspect that the AVR Czar at Microchip, cognizant of internal screwups and general difficulties regarding the two sizes for the interrupt vectors and the "small" chips has declared that they'll release another 8k part over his dead body.

The buffers are forced down to smaller sizes on the tinyAVRs. A tinyAVR 2xx (212/214/202/204) has just 128 bytes of ram.

### Data Rate
The data rate is the total number of bit times per frame: For the most common, 8N1 (8 bit, no parity, 1 stop bit) this is 10 bit times.

While there's always some dead time between bits, that is usually *very* small, and and isn't really relevant. Dividing the baud rate by the bits per frame, including overhead, is an effective any appropriate way to calculate data rate.


### How bad baud rate calculation used to be
This chart shows what baud rates will work at what system clocks, on classic AVRs and modern AVRs. the difference is shocking.
[AVR Baud Rate Accuracy Chart](https://docs.google.com/spreadsheets/d/1rzxFOs6a89jr69ouCdZp8Za1PuUdj1u1IoepTaHVFPk/edit?usp=sharing)

It was mentioned previously that one of most common places to encounter grossly inaccurate baud rates is classic AVRs. This example illustrates just *how bad* one of the most popular baud rate was on classic AVRs, namely 115200 baud. "Well it says the baud rate can be up to 1/8th the system clock, and I'm running at 8 MHz, no problem" you think "And see, it talks just fine to my other classic AVR". Nope. When you do this, you've dug a big hole, covered it with a tablecloth and waited until the sun went down. Adding a modern AVR or anything with a decent baud rate generator is then taking a late night stroll in the area of that covered hole. You're begging for trouble

To illustrate how crap the classic AVR baud generators were, imagine 4 ATmega328p's - one has a crystal and runs at 16 MHz, and three that all run at 8 MHz - one using a crystal, and the others the internal oscillator, which is 2% fast on one and 2% slow on the other - both easily within spec. The 8 MHz one with the w/crystal can talk to the other 8 MHz ones. The crystal-less ones are on the edge when they try to talk to each other due to the variation between them being about the limit - small temperature differences could push it either way, and since the maximum error isn't quite symmetric (it's easier to receive something if you're 4% too fast than if 4% too slow, according to the datasheet), sometimes the temperatures might conspire such that the fast one could receive from the slow one, but not the other way around (It doesn't quite depend on the "phase of the moon" but if one is near a window, it could depend on whether it's sunny out, and hence the one in the window is warmer). The slow crystal-less one can't even talk to a serial adapter, but the other three can, and the fast crystal-less one can also talk to the 16 MHz one, but neither of the other 8 MHz ones can.

Now, imagine you were to reconfigure the serial port on the 16 MHz one, so that it didn't run with U2X enabled. Suddenly, it goes from being 2% fast, to being 3.5% slow - Neither of these is correct, and both of them contribute to problems, but having gotten a net 5.5% slower, it can now talk to the 8 MHz devices no problem!

Now consider dropping a tinyAVR 1/2-series (ex, 1614) in there, and it's a fraction of a percent fast (they usually are 0-0.5% fast. So how far off are the baud rates here?
* The tinyAVR's internal oscillator is between 0.0 and 0.5 % fast; it's baud rate calculation error is negligible.
* The 16 MHz classic is running 2.12% fast assuming perfect clock due to calculation error with U2X - and 3.5% slow without it. Ironically, this will improve communication with the classic AVRs and hinder it with the modern AVR whose baud rate is closest to the "correct" value.
* The 8 MHz one with a crystal is 3.5% slow - but it will do worse than the 16 MHz one at receiving, even though they're both calculating a baud rate 3.5% low), because U2X reduces the allowable baud error. The faster of them is hence net 2.5% slow, while the other one is a mindnumbing 5.5% slow.
* The tiny will likely have no trouble talking the fast crystalless or the 16 MHz with crystal and U2X - though it may have trouble when U2X is turned off.
* The biggest surprise is probably that it probably can't talk to the 8 MHz one with the crystal!

Now, it would come as no surprise to the owner of that hypothetical quarrelsome quartet when their new tinyAVR couldn't talk to some of them. However, someone who had been using classic AVRs with crystals in an existing setup, and they were all talking only to other classic AVRs might be unaware that their "115200 baud" is actually 111111 baud and something with a nearly accurate baud rate, which is off slightly in the other direction, would have problems communicating with them. They would probably blame the modern AVR, and angrily open an issue on megaTinyCore. I've been through this argument several times. "No, sir, your classic AVRs are not communicating at 115200 baud. That is not possible at 8 MHz, see this chart, see, they're running at 111111 baud, which is more than 4% slow. There is nothing wrong with megaTinyCore; 115200 baud is simply not an appropriate baud rate for an 8 MHz classic AVR, their baud generator just sucks".

In the old days, people would often advise "lower the baud rate" as a solution. This would help baud calculation error, but it was of zero benefit if the oscillator accuracy was the limit, and if the speed was still high enough that calculation error was significant, could make it worse. On classic AVRs, it was not that unusual to find parts where, at certain unfavorable speeds, UART was flaky. What's surprising is that the incredibly popular 115200 and 57600 bauds were some of the worst for 8 and 16 MHz parts! For good baud rate accuracy (and hence successful communication with parts that get baud rates right in all cases) 76800 is nearly perfect on 8 MHz with U2X, and that or twice that are nearly perfect at 16MHz. It's a mystery to me why classic AVR arduino-land is so thick with inappropriate baud rates.

The point of this is to demonstrate by example just how large the baud rate error was on classic AVRs, due to the way they generated the baud rate from the system clock by integer division - and the only way to fix it on those parts is to use baud rates that that table shows come out nearly on target, or clock the chip from  a "UART crystal" such that the whole chip runs at some weird speed like 7.37 MHz or 9.21, and so on. This adversely impacts the runtime of `micros()` to a lesser extent, the accuracy of timekeeping in general.

#### Bringing this back to modern AVRs
So, **you should all be very, very thankful for the new fractional baud rate generators**, which are responsible for the charts linked below being a sea of sub 1% and mostly 0.1% error for modern AVRs. When there is an apparent baud rate mismatch when a modern AVR is talking to another device, the problem is not the baud rate calculation. It's usually not the oscillator either, which is rarely even 1% off on any modern AVR. No, your problem 9 times out of 10 is going to be that the device you're talking to generating an incorrect baud rate (for classic AVRs, the table lists them; for other devices you can measure it with a scope to see what their actual baud rate is. The path of least resistance (and no scope needed) with such legacy devices is crude but effective. Since the legacy device likely cannot be coerced to produce the correct baud rate, you can instead just nudge the baud rate the modern AVR up or down 2% (whichever fixes it - it's only 2 options to try and one of them will work). Make note that that the legacy devices' baud rate is off, that it should be replaced by newer hardware.

### Baud rate reference chart
See the [**AVR Baud Rate Chart**](https://docs.google.com/spreadsheets/d/1rzxFOs6a89jr69ouCdZp8Za1PuUdj1u1IoepTaHVFPk/edit?usp=sharing) in google sheets for a table of actual vs requested baud rates. (same chart as linked above)

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

That table is from the receiver's perspective - notice how baud rate does not appear there: It's all about the number of bits, and the magnitude of the mismatch (and if U2X is in use - it is favorable if you have larger baud rate calculation error without it, which is often the case on classic AVRs. Classic AVRs always used it on Arduino land... because they desperately needed anything that would lower the baud rate calculation error. On a modern AVR, however, U2X mode is only favorable if you can't otherwise reach the required speed). Now, obviously the baud rate changes the baud rate calculation error (at least on a classic AVR) but that is the only way that baud rate makes a difference. The common chorus of "Lower the baud rate" is not exactly accurate here, because among commonly used baud rates on classic AVRs, certain frequencies are very favorable and othrs are not - though if you lower it enough, you do get rid of all the calculation error. That may be why 9600 baud was so popular. You will notice that in the examples I now use a baud rate of 115200 baud. That's because we are no longer using legacy parts with crap baud generators, and the slow speed of transmission at 9600 baud resulted in frequent confusion when they were logging data faster than the serial port could output it, and wondering why it was so slow.

### No, actual final tables, the ones you care about "Does serial work at this baud rate, assuming an ideal receiver?"


Classic AVR, UART crystal. Not bad. It is somewhat challenging to set up the millis timer to keep proper count when the system clock is 14745600 Hz than when it's 16000000 Hz, imposing a performance penalty (I implemented this on old classic ATTinyCore)

 F_CPU   | 921k| 460k| 345k| 230k| 172k| 115k| 76k | 57.6k
 --------|-----|-----|-----|-----|-----|-----|-----|------
 7372800 |Super|Super| No  |Super| No  |Super|Super|Super
 9216000 | No  | No  | No  |Super| No  |Super|Super|Super
11059200 | No  |Super|Super|Super|Super|Super|Super|Super
12902400 | No  | No  | No  |Super| Bad |Super|Super|Super
14745600 |Super|Super| No  |Super| Bad |Super|Super|Super
18432000 | No  |Super| No  |Super| Poor|Super|Super|Super
20275200 | No  | No  | No  |Super| Poor|Super|Super|Super
22118400 |Super|Super|Super|Super|Super|Super|Super|Super

"No" = > 4% calculation error - hopeless.
"YES" indicates perfect baud matching
Anything else is as by the tables below


There are few cases I found in my quick review of a UART crystal at a realistic, "traditional" UART baud rate (only looked at 56.7k and up) where the crystal was able to make work a baud rate that was not dead on.

#### Classic AVR, clock speeds you actually want to use
In contrast, with actual common CPU speeds on classic AVRs it was also pretty ugly and I am amazed it worked as well as it did.

 F_CPU   | 1m  | 921k| 500k| 460k| 345k| 230k| 172k| 115k| 76k |56.6k|36.4k| ... | 300 |
 --------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 4000000 | No  | No  |Super| No  | No  | No  | Bad | No  | No  | Bad |Super|Super|Super|
 8000000 |Super| No  |Super| No  | Bad | No  | Bad | Bad |Super|Poor |Super|Super|Super|
12000000 |Super| No  |Super| No  | No  | No  | Bad |Great|Poor |Super|Super|Super|Super|
16000000 |Super| No  |Super| No  | Bad | Bad | Bad |Poor |Super| Good|Super|Super|Super|
20000000 |Super| No  |Super| No  | Bad | Good| Bad |Good | Good| Good|Super|Super|Poor |
24000000 |Super| No  |Super| No  | Bad |Super| Poor|Super|Super|Super|Super|Super| No  |

* Super - within 0.25%
* Great - within 0.75%
* Good - within 1.5%, likely to work
* Poor - Wirthin 3%, crapshoot.
* Bad - Within 4%, unlikely to work under most conditions

Note that because the interrupt handler was not under selective pressure (since the measurable thing, the baud rate that would work, was limited by hardware, not ISR performance), It is not performant in most cases. Do not expect a classic AVR to be able to receive more than a couple of characters in a row successfully at BAUD = F_CPU/8.

Depending on implementation, USART speeds may crap out on the low end at twice the spec'ed baud. If this occurs, that is a bug in the implementation of Serial.begin() and should be reported as such.

U2X mode is generally kept enabled at all times due to the significant improvement to baud rate calculation accuracy.

#### Modern AVRs like the ones covered in this document
And for modern AVRs
 F_CPU   | 1m  | 921k| 500k| 460k| 345k| 230k| 172k| 115k| ... | 2.4k|
 --------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 4000000 | No  | No  |Super|Great|Great|Great|Great|Super|Super|Super|
 8000000 |Super|Great|Super|Great|Great|Super|Super|Super|Super|Super|
10000000 |Super|Super|Super|Super|Super|Super|Super|Super|Super|Super|
12000000 |Super|Super|Super|Super|Super|Super|Super|Super|Super|Super|
16000000 |Super|Great|Super|Super|Super|Super|Super|Super|Super|Super|
20000000 |Super|Super|Super|Super|Super|Super|Super|Super|Super|Super|
     ... |Super|Super|Super|Super|Super|Super|Super|Super|Super|Super|
40000000 |Super|Super|Super|Super|Super|Super|Super|Super|Super| Poor|
48000000 |Super|Super|Super|Super|Super|Super|Super|Super|Super| No  |

Yeah, this is much simpler. Guaranteed under 0.75% baud calc error over all supported clock speeds. Same maximum baud rate in theory (F_CPU=8), except here it's *actually achievable at baud rates people want to use*. mTC and DxC use a very tightly optimized ISR which I believe is just barely fast enough for cyclic receive of up to the buffer length, so you CAN actually receive a block of characters at those baud rates.

U2X mode is only used for speeds above F_CPU/16 (where it must be used).

I think we can all be thankful for the fractional baud rate generator. Unless you happen to be a UART crystal manufacturer maybe.

Frankly the modern AVRs were less than rosy news for crystal manufacturers - not that it was exactly news that on-chip oscillators are getting good enough to replace crystals, nor that they (presumably) are die-space-hungry and so designers don't want to design in support for them (cause it makes the chip cost more to make), and the developers/customers designing products don't want to use a crystal if they don't have to. Crystals also kinda suck to use - they've got the fiddly loading caps which nobody can agree on a formula to calculate (I've seen at least two formulas, but using caps of those values doesn't work especially since this is megaTinyCore, and the tinyAVRs don't even *support*)

### Of couerse, there IS still clock skew
On tinyAVR, DA, or EB, you probably don't have an external oscillator, and you may not choose to put one on your DA/DB/DD/EA. In these cases, the internal oscillator error is added to the calculation error (which is thankfully near nil, as we just discussed.

In the past (classic AVR) making serial work without a crystal could be impossible, an adventure, or a cake walk depending on the specimen (not part number/species, specimwn - specific, individual part), and what it's talking to (Did you think that your USB serial adapter has a perfect oscillator in it? Look at it! It doesn't even have a bloody crystal on it! At least on the AVR side, you know what the calculation error is, and you can trim the oscillator and so on - that's a luxury you don't have on the USB-serial adapter's side. You ask for a baud rate, and it gives you... a baud rate. Usually it's pretty close to what you ask for. Most serial adapters allow you to select any baud rate and have a similar internal method of generating the baud rates. *Some do not* The CP2102 is one of the "do not" parts - you can get a flash utility to whisper words to it to change the baud rates it supports, but you only get a few at a time. This is not uncommon, and was far more common in the past. Certainly, since they're using a similar baud generation construction to get their baudrates, they're at risk of calculation error too, and that risk increases at higher speeds, and at weirdo baud rates. Even on a serial adapter that attempts any speed requested, it will still have some level of granularity, as it's taking a reference clock and dividing it to get the baud rate, or rather some multiple of the baud rate (probably 16 or 8) - and the divisor is going to be an integer, so there's going to be unavoidable granularity.

If you hadn't thought about it - as the target gets higher, th

UARTs sample their RX line 16 times per bit period (or optionally half that on AVR, an option that is almost always advantageous on classic and almost alwatys disadvantageous on modwen - so we don't have Serial.begin() turn that on unless you request a baud requires it. The three middle samples (either 8, 9, and 10, or 4, 5, and 6) then majority vote on whether the bit was a high or a low. As soon as the receiver has taken their last sample of the stop bit, and confirmed it to be such a thing, it it s ready to receive another,.k





`*` Aliexpress sucks this way. The first thing you do when you buy oscillators or crystals from them (which is very tempting, as the china discount on the most desirable packages - 4-pin SMD 5032 or 3225 packages is like 5 to 1, with better selection available. Tjthe china discount smaller on shittier packages like the collossal HC/49) is repack them and clearly label them). Oscillators are all that crystals are that we hate, cost as much as the MCU, only work in one voltage range (ie, 1.5-2, 3.3.6, 4.5-5.5), are polarity semsitive, and static senstivive, and current hogs. Ugh! (Don't buy oscillators on aliexpress - or at all if tyou can avoid it - the china discount is barely 2:1 and they don't even appear to *know* the part numbers... OR THE VOLTAGE OF OSCILLATORS! Given that the last oscillator rated for operation from 1.8-5v went out of production around when the DB came out, and that in fact, no single oscillator is good for more than one common voltage range since the discontinuation of that line from... was it epson? Not a company you think of when you think of frequency crystals. I'm annoyed I didn't stock up! I was distracted by the DBs. So you really have maybe a 1/3 chance assuming all voltage ranges are equally represented, unless they all hail from some no-name Chinese manufacturer who hasn't forgotten the lessons of - what 5 years ago? and still knows how to build wide voltage rannge oscillators? The alternative is that clueless vendors are selling oscillators to clueless buyers who will have a 1 in 3 chance of operating within specification assuming all voltage ranges are equally represented (I suspect 3.3 an4 5 are overrepresented). Oh and remember how I said the markings are often unreadable or absent (and when present, may be a code number useless without knowing the mfg, if it's public at all)? In some cases, that includes THE POLARITY MARKING. In case you haven't worked with these, when installed backwards, they instantly fail.

Anyway, enough about classic AVRs, let us look forward, not back!

### A word about baud rate mismatch
"I said moving on!" *You're not moving on without explaining "barely-working regime" that we constntly find ourselves in. Or **I** will!* "Considering the morbid examples you pick, fine."

So let's consider  1 USART frame (character), data 0bABCDEFGH (where uppercase letters are 1's or 0's)

1. USART line starts HIGH. It first sends the start bit, a 0 bit,
2. It then sends the rest of the databytes in the opposite order of how everything else does it: HGFEDCBA
3. It finally ends the frame with a stop bit, which bis a 1.
4. It returns to idle state.

Now the transmitter is timing these bits based on the baud rate you asked for, and assumes it's timebase is accurate. So is the receiver. The internal osciallators on modern AVRs really are good enough that you rarely need to worry about using a crystal in most situations, including this. but it deos raise the prospect of a specific and baffling behavior where **Short bursts of data work, but longer messages turn into gibberish partway through**, occurring in in one direction (from the faster to the slower device (eg, from a device a a percent or two fast to one a percent or two slow).

This is easy to understand if you imagine what is being output by the transmitter.


### Minimum and Maximum baud rates
Like Classic AVRs the maximum baud rate is F_CPU / 8, using the `U2X` mode, which is still the case.

Unlike classic AVRs, the gap between the theoretical maximum and the practical maximum is much smaller thanks to the fractional baud rate generator". On the older parts there were "gaps" between adjacent UART clock division values. Since the numbers used for standard baud rates don't resemble round numbers, people who needed accurate baud rates would use crystals with bizarre frequencies like 18.42 MHz, so it could be divided down to the match standard baud rates. That - unsurprisingly - led to slower, less accurate timekeeping, where it was supported by an Arduino core at all. Luckily, the days of UART crystals are over! Instead of supplying a whole number, the value passed to the fractional baud rate generator is in 64ths, so as long as it is within the supported range of baud rates, the farthest any two settings are from each other is 1/64, or 1.56%, so the highest baud rate error from the calculation is half that, comfortably within the limits of USART 0.78%. This corresponds to baud rates just below the maximum possible for a given system clock, which for typically used clock speeds is far above what would commmonly be used.

#### Maximums
The highest baud rates possible are listed below. In practice, below 1 mbaud, it is rare to see baud rates not based on UART clocks; 921600 is much more common that 1 mbaud and so on:
* 6 mbaud can be reached at 48 MHz  (Very few adapters can even do this, and care must be taken with the wiring, the FT4232HQ claims to support it)
* 5 mbaud can be reached at 40 MHz+ (Very few adapters can even do this, and care must be taken with the wiring, the FT4232HQ claims to support it)
* 4 mbaud can be reached at 36 MHz+ (Very few adapters can even do this, and care must be taken with the wiring, the FT4232HQ claims to support it)
* 3 mbaud can be reached at 24 MHz+ (FT232 maximum speed, and the highest baud rate achievable without overclocking the AVR.)
* 2 mbaud can be reached at 16 MHz+ (the maximum speed most serial adapters support; the CH340 claims to support 2 MBaud; I wasn't able to make anything above 1mbaud work, though)
* 1 mbaud and more importantly, 921600 baud can be reached at 8+ MHz.
* 500000 baud - and more importantly, 460800 baud can be reached at 4+ MHz
* 125000 baud - and more importantly, 115200 baud can be reached at 1+ MHz
* Even running from 32.768kHz, 4000 baud is achievable!

**Note that those are the absolute maximum that the UART peripheral can do**, in practice the speed of the print() method becomes a limiting factor for transmission This will limit continuous transmission to lower speeds. At high baud rates (relative to the CPU clock speed), `Serial.print()` may no longer use the buffer, as the relatively slow `print` class begins to take a similar amount of time as sending data down the wire (`Serial.print` - and all things like them are surprisingly slow - it's not just the speed limit imposed by the baud rate); there is no precise set point where this occurs - the buffer may be used for short stretches while sending a long string, and print takes different amounts of time for numbers, strings, and so on). On receiving, a continuous stream of data will be coming in at in at 1 byte per 80 clocks at the maximum speed assuming 8 bit characters. Just taking those bytes and stuffing them into the buffer will take almost 100% of available CPU time, but it *should* be able to keep up provided there aren't any long-running ISRs also firing and 8-bit characters are used. With 5-bit characters at maximum baud rate, you can never receive a continuous stream of data - you have only 56 clocks per character. and the ISR requires

Combined with a highly accurate internal oscillator with virtually no voltage dependence, you are essentially guaranteed that UART will work without even resorting to autobaud, and cases where such measures are required will be the fault of the other device being way off of what they advertise. The most likely situation which would encounter problems is when trying to communicate with.... a classic AVR operating near it's limits, such as an 8 MHz ATmega328p at 115200 baud. The ATmega328p will actually be speaking 111111 baud even with a crystal due to the clock division described above, which is a difference of 3.55%, which is just on the edge of working. In situations like this, since you know which direction the other device is off, and by approximately how much you can just nudge the clock speed down a bit. If you *don't* know what direction it is off by, only that it is not quite working, if the other device can talk to a serial adapter, you only need to check two options: 2% higher than the nominal baud rate and 2% lower, since the accuracy tolerance of UART serial is around 4%. You will rarely encounter devices where the UART baud rates are so far off that they're on the edge of not working (other than classic AVRs running at 8 MHz and 115200 baud, which is a particularly bad speed for them) but which are not all wrong in the same direction - anything off by more than a few % is probably wrong because of integer-math like classic AVR, rather than oscillator inaccuracy.

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
|    153600 |               Rare |
|    172800 |          Unheardof | You would expect this - but its not actually used anywhere |
|    230400 |           Uncommon | The first non-exotic speed higher than 115200           |
|    256000 |            V. Rare |  Looka like the math would be easy. Nope. Not really.   |
|    345600 |             Exotic | 1.5x 230400, continues the pattern. DxCore uses it <br/> in SerialUPDI, (460.8 exceeds the write speed limit) |
|    460800 |           Uncommon | The next to last of the standard usart speeds.          |
|    512000 |            V. Rare | As with 256k above, it looks more convenient than it is |
|    921600 |               Rare | You don't see this one in use much, that's for sure <br/> Hard to generate from most devices, and very close to 1mbaud |
|   1000000 |           Uncommon | The highest commonly used baud rate. more popular than <br/> 921.6kbaud easier to make by dividing common clocks |

These are some of the most common; Most - but not all - adapters can generate nearly arbitrary baud rates. The CP2102 has a preprogrammed list, though the vendor has a utility that can change them. The HT42B534 has no mechanism for changing the supported baud rates baud rates, etc. Low-end serial adapters usually cap out at 1.5, 2. or 3mbaud. The highest I have seen is 6 mbaud, available only on FTDI's top-end parts, and which is fast enough that the wire's electrical properties that we normally neglect would become relevant; physics would make communication fiddly.

*fun project idea:* Make a true "autobaud" system that monitors serial at an unknown baud rate, figures out what it is, and then configures the Serial appropriate. You'd want the RX line of the serial port, and probably a event input pin piped to a type B timer, such that you could keep measuring to see what the shortest time between two transitions that you see is. That's 1 bit period. 1-over-period = baud rate.

## Waking from sleep on USART
To do this you must set the SFDEN bit in the USART immediately before sleeping, and you must not set the SFD interrupt. The chip will still be woken when a character is received. As of 2.6.0, the RX routine will clear SFDEN before reading the character, so this becomes viable. In previous versions this would not work due to the ubiquitous errata.


## Appendix A: Notes on the ISR implementation
In order to keep up with the baud rate (to handle the perfectly valid, continuous data streams in both directons), the ISRs for DRE and RXC **must** be execute very fast. The "stock" implementation in the Nano Every / Uno Wifi Rev. 2 was not very fast. The compiler rendered the C code so inefficiently that near the maximum baud rate, neither TX nor RX worked correctly. Not only that, but a combination of a race condition and misuse of chip feature would leave the chip stuck in an infinite loop near the maximum Tx speed. Just below that speed, it was too slow to keep up - you might have asked for 1 mbaud, but would end up with characters transmitted at 1 mbaud with a pause between them. So the effective speed was lower. For Rx, a continuous datastream would simply miss characters. The former was absolutlely unacceptable (especially since it was undocumented and dependent on timing), the latter resulted in an artificially low maximum baud. To address these, as well as the wasted flash cause by 1 copy of each ISR per serial port (particularly problematic on, say a 424), a complicated series of inline assembly blocks was used with a few particularly unusual "tricks": First, all the ISRs are "naked" - with no prologue or epiloge, the blob of inline assembly. It pushes a single register, and neither changes nor saves SREG. A value of twice the USART number is loaded into the register that was pushed, and then it jmps/rjmps to a shared handler function. This function is also declared `naked`, as well as `used` to keep the compiler from optimizing it away. It knows which register contains twice the USART number, used to find the start of the Serial instance and from there, fi, and it is there that the full prologue, epilogue, and reti are located (along with, of course, . The
