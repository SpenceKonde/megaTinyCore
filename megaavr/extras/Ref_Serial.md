# Hardware Serial reference (AKA UART or USART)
All of the supported parts contain USART serial ports. We all use them every day, but not all of us are familiar with the details of these. While USART, SPI, and TWI are all serial protocols, "Serial" without qualification almost universally refers to a USART. These are the only protocol for which USB adapters are widely available and ubiquitous. SPI and I2C USB adapters exist - but they aren't standard and generally require some measure of drivers. And while there are countless applications that will act as a serial console, software for I2C and SPI, is usually vendor specific if it has been written at all.

On all supported devices, where the appropriate pins are present, they can be pin-swapped, and on DA and DB parts, each port except PORTD gets a USART, which defaults to pins 0, 1, 2 and 3 (TX, TX, XCK and XDIR), and 4, 5, 6 and 7 when pinswapped. This is configured using the Serial.swap() or Serial.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling `Serial.begin()`

## Background - quick overview of how serial is different
In addition to being by far the most popular interface for Arduino users, rthere are several things that really set it apart... and these have consequences, for better and worse. First the vital stats:
* It uses 2 wires, transmit, and receive. Transmit of one connects to receive of the other (that is, they cross over. Sometimes connectors are marked relative to the device, other times to the device you connect to it; because of this, devices often extend "TX" and "RX" to "TXO" (Transmit Out) and "RXI" (Receive In).
* It is operated at a selectable baud rate, from incredibly slow speeds like 300 baud from early teletypes up to several megabaud, but in practice, USART is normally used betwwwn 9600 baud and 1mbaud.
* *There is no clock* - timing is used to differentiate bits. That presents some problems, as *the two sides must have agreed on the baud rate* and have clocks that agree.
  * Mismatched baud rates are generally interpreted as gibberish with lots of non-printing characters. Sometimes, but not always, you can examine the bit patterns of the gibberish and figure out whether it's too fast or too slow. Simply trying different speeds is usually much more efficient.
* It's not just the baud either that needs to be agreed on. The number of "stop" bits (in other words, how long the pause between bytes should be; it can be 1 or 2 stopbits for an AVR - but other devices sometimes support 1.5 varies from 1 to 2. Not even the number of bits to a byte is universally agreed upon! Anywhere between 5 and 9 can be supported by the hardware, but dealing with 9-bit characters is not supported by this implementation of serial.
* It is active low, readily lending itself to open drain multidrop networks
* Running a single combined TX and RX line is not uncommon, particularly when RS-485 is in use.
* There are a great many protocols with a different PHY layer but the same format - RS485, LIN, and RS232 are common examples.


### USART vs UART
UART us "Universal Asynchronous Receiver Transmitter"

USART is "Universal Synchronous/Asynchronous Receiver Transmitter"

A UART can only communicate in asynchronous mode (with TX and RX) a USART adds a third pin, XCK, that can be configured as a clock.

The synchronous mode isn't as flexible as you might think, largely because of the whole start/stop bit thing. Another reason is is less frequently used is the availability of adapter hardware for desktop compters, or rather the lack thereof. It is, if anything, even harder to find support for than a SPI.

Note that, unlike the MSPI mode, Synchronous Mode does support operation as either slave or master; the MSPI mode, which drops the start and stop bits, and works largely like a proper SPI interface is master only.

Neither of the synchronous modes are supported by the core; a third party library must be used, and may not exist.

### Terminology (non-standard)
* "Pin set" - synonym for "pin mapping option" and other descriptive terms, consisting of all the pins that could be enabled with a given PORTMUX configuration and their roles,
* "Mux Code" - For a given pinset, the mux code is the value written to the appropriate bits of the PORTMUX register controlling USART pin selection. It is a value of 0, 1, 2, or 3 leftshifted some number of places. For the same pinset, for the same peripheral, with the same swap level, different parts may have a different mux code.
* "Swap Level"  - The swap level is the logical number of the pinset - the default mapping is always 0, swap level n is the option named as ALTn in the datasheet. Microchip has never articulated any policy of keeping pinsets consistent between parts, all indications are that the Dx, Ex, and megaAVR 0-series will have the same pinsets, though some parts have different swap levels.

For example, TX = PC4, RX = PC5, XCK = PC6, XDIR = PC7 is pinset 1 for USART1.
It's swap level is 1, and it's mux code is is 0x04 (on DA/DB parts - it will presumably be 0x08 on the EA-series, assuming it stays like the product brief describes, with the DD-series mux options).

## Serial in DxCore and megaTinyCore
We have tweaked the Serial classes for greater flexibility, efficiencty, and performance.
It implements the standard Serial API, plus a small number of additional options.

### Serial.swap(mux option)

### Serial.pins(tx, rx)

These two methods both change the pins connected to the device. Swap takes a number - that's 0 for the "default" 1 for "ALT1" and so on. It is more efficient and is the preferred method. Serial.pins does exactly what you would think - sets the TX and RX pins of that port to the TX and RX pins given by the arguments. Assuming of course, that the pins are valid mux options. Either way, both of these functions return a boolean, true for a successful change of the pins, and this always must be called before Serial.begin() (or after Serial.end() but before reopening it with Serial.begin() again). If the swap or pins given are invalid, it will return false, and reset the pins to the default value. If the compiler can determine at compile time that it will never work (generally, constant arguments that aren't valid, like calling `Serial.swap(21)` instead of `Serial.swap(2)`) we will refuse to compile and throw a `badArg()` error.

On DA/DB parts, this was pretty straightforward. Every PORT except the middle one (PORTD) got a USART (PORTD is the "analog-focused" port), and in ascending number, the USARTs are associated with PORTA, PORTC, PORTF, PORTB, PORTE, PORTG - very systematic. Default on pins 0-3 TX to XDIR, alternate option on 4-7, TX to XDIR. That is, in fact, identical to the megaAVR 0-series. The DD has made things a lot more complicated/exciting with regards to pin mapping, but nothing benefited more than USART, which now has up to 5 mapping options.

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


Can be combined as in:
`Serial1.begin(115200,(SERIAL_8N1 | SERIAL_OPENDRAIN | SERIAL_RS485 ))`

If you use the two argument form of Serial.begin() be certain to remember to pass the constant, not just a modifier.

### Loopback Mode
When Loopback mode is enabled, the RX pin is released, and TX is internally connected to Rx. This is only a functional loopback test port, because another device couldn't drive the line low without fighting for control over the pin with this device. Loopback mode itself isn't very useful. But see below.

### Open Drain Mode
In Open Drain mode, the TX pin will no longer drive high. The pin must not be set as OUTPUT. If you don't fiddle with it, this will correctly handled for you. With just openDrain mode enabled, a multidrop network becomes very practical.

### RS485 Mode
In RS485 mode, at initialization, the HWSerial class configures the XDIR pin as an OUTPUT. 1 bit-time prior to sending any data, the XDIR pin will be driven HIGH. If using RS485, this should be connected to the external line driver IC to enable transmit. XDIR will be lowered 1 bit-time after the last bit has been sent. If you require the opposite polarity, simply set PORTx.PINnCTRL |= PORT_INVEN_bm;
or use `pinConfigure()` [See Digital I/O Reference](Ref_Digital.md)

#### These three options can be combined, and probably should
* Loopback + Open Drain - This results in a half-duplex single serial interface. This is fairly common, and in fact is often used in.... RS485 configurations.
* Loopback + Open Drain + RX485: In this mode, it will work perfectly for the case where there is an external line driver IC but it has only a single TX/RX combined wire and a TX_Enable pin (terminology may vary).

#### Half-duplex schemes change the behavior of Serial in important ways
Normally, RX is not disabled unless the user specifically requests it. Bytes received at any time will be placed into the buffer by the USARTn RxC interrupt as long as it is not full. With loopback mode enabled, you will receive all the characters you transmit. That's fine for just loopbnack - since TX is actively driven high when idle, you can't exactly receive data any other way. When Open Drain mode is also active, though, the stuff that you sent will inevitable end up intermixed with actual received data. This is not very helpful.
SO, whenever the following bits are set:

CTRLA: LBME
CTRLB: ODME, TXEN, RXEN

In this case, Any *write* will temporarily disable the RXC interrupt, and enable the TXC interrupt. When the TXC interrupt executes, it will disable itself, read RXDATAL until the RXC flag is cleared, and then turn off the TXC interrupt and enable the RXC interrupt again. This prevents you from getting junk characters mised in with the ones you were sending.

That configuration will result from calling the two argument version of begin()  with SERIAL_OPEN_DRAIN and SERIAL_LOOPBACK, or equivalently, SERIAL_HALF_DUPLEX, and neither SERIAL_TX_ONLY nor SERIAL_RX_ONLY,

### Event RX - documentation pending testing and verification

## More indepth background

### Character size
baud is bits per second. But those are not data bits. USART data is sent in "frames". These frames start with a "start bit" and end with 1 or 2 stop bits (rarely, 1.5) may or may not have parity, and while the byte is almost ubiquitous now, it hasn't always been, leading the the proliferation of character lengths supported. The hardware supports 5, 6, 7, 8 or 9 bit characters. Since the common datatypes on the AVR are 8 or 16 bits, it is awkward to efficiently handle such larger size units of data. DxCore and megaTinyCore only support 5-8 bit characters,  9-bit serial is vanishingly rare.

### Framing and levels
The idle UART is held high by either pullupos on the receiving side, o active drive on the transmit side. An Open Drain configuration involves the USART transmitter in an open drain configuration, asserting the pin low, and relying on ppullups to bring it high. The advantage of this is that multiple devices can use one set of wires to communicate, though the top attainable speed is lower. This is how I2C works, too.
Anyway - each frame starts with a "start bit": of normal length. This is always a 0. (so you can see it against the high idle state), then the data bits go out, starting with the smallest (this is the opposite of how I2C and most every other protocol works).. After all databits, the parity bit (if any) is sent, followed bty the "stop bit[s]" which are always high, so they blend into the background idle state.

Receipt and transmission both depend on the two devices having agreed ahead of time on what baud rate and connection settings to use, and on both having reasonably accurate clock speeds, including that they be running at the speed they think they are. A 16 MHz modern AVR running 115200 might calculate the baud rate to an accuracy of 0.1% - but it will 25% higher than that if the chip is actually running at 20 MHz.

### Data Rate
The data rate the total number of bit times per frame: For the most common, 8N1 (8 bit, no parity, 1 stop bit) this is 10 bit times.

While there's always some dead time between bits, that is usually *very* small, and and isn't really relevant. Dividing the baud rate by the bits per frame, including overhead, is an effectivee any appropriate way to calculate data rate.


### Extremes of baud rate and clock problems

[AVR Baud Rate Accuracy Chart](https://docs.google.com/spreadsheets/d/1uzU_HqWEpK-wQUo4Q7FBZGtHOo_eY4P_BAzmVbKCj6Y/edit?usp=sharing)

It was mentioned previously that one of most common places to encounter grossly inaccurate baud rates is classic AVRs. This example illustrates just *how bad* one of the most popular baud rate was on classic AVRs, namely 115200 baud. "Well it says the baud rate can be up to 1/8th the system clock, this is less than a quarter if this, no problem" one might think - but nope.  Imagine 4 ATmega328p's - one has a crystal and runs at 16 MHz, and three that all run from 8 MHz - one using a crystal, and the others the internal oscillator, which is 2% fast on one and 2% slow on the other. The 8 MHz w/crystal can talk to the other 8 MHz ones. The crystal-less ones are on the edge when they try to talk to each other due to the variation between them being about the limit - small temperature differences could push it either way, and since the maximum error isn't quite symmetric (it's easier to receive something if you're 4% too fast than if 4% too slow, according to the datasheet), sometimes the temperatures might conspire such that the fast one could receive from the slow one, but not the other way around. The slow crystal-less one can't even talk to a serial adapter, but the other three can, and the fast crystal-less one can also talk to the 16 MHz one.

Now, imagine you were to reconfigure the serial port on the 16 MHz one, so that it didn't run with U2X enabled. Suddenly, it goes from being 2% fast, to being 3.5% slow - Neither of these is correct, both of them contribute to problems, but in the opposite direcitons!

Now consider dropping a tinyAVR 1/2-series (ex, 1614) in there, and it's a fraction of a percent fast (they usually are 0-0.5% fast. So how far off are the baud rates here?
* The tinyAVR's internal oscillator is between 0.0 and 0.5 % fast; it's baud rate calculation error is negligible.
* The 16 MHz classic is running 2.12% fast assuming perfect clock due to calculation error with U2X - and 3.5% without it. Ironically, this will improve communication with the classic AVRs and hinder it with the modern AVR whose baud rate is closest to the "correct" value.
* The 8 MHz one with a crystal is 3.5% slow - but it will do worse than the 16 MHz one at receiving, even though they're both calculating a baud rate 3.5% low). , because U2X reduces the allowable baud error;. The faster of them is hence net 2.5% slow, while the other one is a mindnumbing 5.5% slow.
* The tiny will likely have no trouble talking the the fast crystalless or the  16 MHz with crystal and U2X - though it may have trouble when U2X is turned b
* The surprise though is that the tiny might have trouble with the 8MHz one with a crystal. Classic AVRs had a large error of the way they generated the baud rate from the system clock by integer division.

It would come as no surprise to the owner of that hypothetical quarrelsome quartet when their new tinyAVR couldn't talk to some of those. However, someone who had been using classic AVRs with crystal in an existing setupt to talk to each other might be unaware this, and blame the modern AVR part (I have had that discussion a few times)

In the old days, people would often advise "lower the baud rate" as a solution. This would help baud calculation error, but it was of zero benefit if the oscillator accuracy was the limit, and sometimes would make it worse  On classic AVRs, it was not that unusual to find parts where, at certain unfavorable speeds, USART was flaky.

#### Bringing this back to modern AVRs
So, you should all be very, very thankful for the new fractional baud rate generators, which are responsible for the charts linked above being a sea of sub 1% and mostly 0.1% error at most. Here, your problem isn't the baud rate calculation. It's usually not the oscillator either, which is rarely even 1% off on any modern AVR. No, your problem 9 times out of 10, is going to be that the device you're talking to generating an incorrect baud rate, more often than not a classic AVR . The path of least resistance with such legacy devices? They probably aren't worth trying to find a way to improve the baud accuracy on.  Instead, just nudge the baud rate the modern AVR up or down 2%, and make a note that those roles in your system should be replaced with a new  - that'll either make it much worse, or solve the problem; if it's in the worse category, instead use a baud rate 2% lower than expected, and you should be good.


### Final table of this section
**Total Error vs Data Frame Size**
First three columns are normal mode, last three are U2X, which is used if you are within a 2:1 speed increase of the limit of the hardware for your clock speed. U2X is never used any other time by these cores; (It is always used by default on classic parts where the clock calculation accuracy is the limiting factor)

| Data bits per frame |Minimum|Maximum| Recommend |U2XMin |U2XMax |  Recommend  |
|---------------------|-------|-------|-----------|-------|-------|-------------|
|              5 bits | -6.80 | +6.67 |    +/- 3% | -5.88 | +5.66 |    +/- 2.5% |
|              6 bits | -5.88 | +5.79 |  +/- 2.5% | -5.08 | +4.92 |    +/- 2.0% |
|              7 bits | -5.19 | +5.11 |  +/- 2.0% | -4.48 | +4.35 |    +/- 1.5% |
|              8 bits | -4.54 | +4.58 |  +/- 2.0% | -4.00 | +3.90 |    +/- 1.5% |
| (8+1 parity) 9 bits | -4.19 | +4.14 |  +/- 1.5% | -3.61 | +3.53 |    +/- 1.5% |
|(9+1 parity) 10 bits | -3.83 | +3.78 |  +/- 1.5% | -3.30 | +3.23 |    +/- 1.0% |

That table is from the receiver's perspective - notice how baud rate does not appear there: *baud rate only has larger than marginal impact error when it changes baud division calculations*. Otherwise, it's all about the number of bits, and the magnitude of the mismatch (and if U2X is in use - it is favorable if you have larger baud rate calculation error, or if you just plain can't achieve that speeed without it. You need to work to find something that needs a baud rate where that becomes a problem on modern AVRs though.

### Minimum and Maximum baud rates
Like Classic AVRs the maximum baud rate is F_CPU / 8, using the `U2X` mode, which is still the case.

Unlike classic AVRs, the gap between the theoretical maximum and the practical maximum is much smaller thanks to the fractional baud rate generator". On the older parts there were "gaps" between the closest options. Since the numbers used for standard baud rates don't resemble round numbers, people who needed accurate baud rates would use crystals with bizarre frequencies like 18.42 MHz, so it could be divided down to the match standard baud rates. That - unsurprisingly - led to slower, less accurate timekeeping, where it was supported by an Arduino core at all. Luckily, the days of UART crystals are over! Instead of supplying a whole number, the value passed to the fractional baud rate generator is in 64ths, so as long as it is within the supported range of baud rates, the farthest any two settings are from each other is 1/64, or 1.56%, so the highest baud rate error from the calculation is half that, comfortably within the limits of USART 0.78%. This corresponds to baud rates just below the maximum possible for a given system clock, which for typically used clock speeds is far above what would commmonly be used.

The highest "commonly used" baud rates (almost none of which are commonly used) possible are:
* 6 mbaud can be reached at 48 MHz  (this is faster than any normal serial adapter, and too fast for casual wiring)
* 5 mbaud can be reached at 40 MHz+ (same as above)
* 4 mbaud can be reached at 36 MHz+ (same as above)
* 3 mbaud can be reached at 24 MHz+ (the maximum speed I have seen advertised as supported by a serial adapter)
* 2 mbaud can be reached at 16 MHz+ (the maximum speed most serial adapters support)
* 1 mbaud and more importantly, 921600 baud can be reached at 8+ MHz.
* 460800 baud can be reached at 4+ MHz
* 115200 baud can be reached at 1+ MHz

**Note that those are the absolute maximum that the UART peripheral can do**, in practice the speed of the Serial class will limit continuous transmission to lower speeds. At high baud rates (relative to the CPU clock speed), `Serial.print()` may no longer use the buffer, as the relatively slow `print` class begins to take a similar amount of time as sending data down the wire (`Serial.print` - and all things like them are surprisingly slow - it's not just the speed limit imposed by the baud rate); there is no precise set point where this switches - the buffer may be used for short stretches while sending a long string, and print takes different amounts of time for numbers, strings, and so on); the ISR takes similar measures for receiving: after reading the byte and putting it into the buffer, it checks the flag to see if there's any more before returning. At some point below this maximum, the overhead of taking the received character and storing it in memory will take longer than it will for the next character to arrive, at which point characters will be lost if attempting to receive a continuous stream of data at that speed. Use a faster system clock if this becomes a problem; it is unlikely to be encountered for reasonable baud rates except at very low system clock speeds.

Combined with a highly accurate internal oscillator with virtually no voltage dependence, you are essentially guaranteed that UART will work without even resorting to autotune, and cases where such measures are required will be the fault of the other device being way off of what they advertise. The most likely situation which would encounter problems is when trying to commuinicate with.... a classic AVR operating near it's limits, such as an 8 MHz ATmega328p at 115200 baud. The ATmega328p will actually be speaking 111111 baud even with a crystal due to the clock division described above, which is a difference of 3.55%, which is just on the edge of working. In situations like this, since you know which direction the other device is off, and by approximately how much you can just nudge the clock speed down a bit. If you *don't* know what direction it is off by, only that it is not quite working, if the other device can talk to a serial adapter, you only need to check two options: 2% higher than the nominal baud rate and 2% lower, since the accuracy tolerance of UART serial is around 4%. You will rarely encounter devices where the UART baud rates are so far off that they're on the edge of not working (other than classic AVRs running at 8 MHz and 115200 baud, which is a particularly bad speed for them) but which are not all wrong in the same direction - anything off by more than a few % is probably wrong because of integer-math like classic AVR, rather than oscillator inaccuracy.

But nothing is free - on classic AVRs, the slowest baud rate possible would (with `U2X` off) be when the register was at it's maximum of 4096 (F_CPU / 4096 * 16) = 1/65536th of the system clock, so 16 MHz would have a minimum of 244 baud. On modern AVRs the BAUD register can take a full 16-bit value (2<sup>4</sup> times larger than the 12-bit value of classic parts), but needs to be 64 times (2<sup>6</sup>) higher for the same baud rate and system clock. Hence, the minimums are 4 times higher (2<sup>(6-4)</sup> = 2<sup>2</sup> = 4), or 1/16384th of the system clock. This is still a very wide range, but whereas the classic AVR almost never ran into the minimum baud rate, occasionally one encounters this on the modern parts. The maximum clock speeds that can use the following "commonly used" (to the extent that any of these are common anymore) baud rates are:
* 300 baud will work only when F_CPU is no higher than 5 MHz.
* 600 baud will work only when F_CPU is no higher than 10 MHz.
* 1200 baud will work as long as F_CPU is no higher than 20 MHz.
* 2400 baud will work as long as F_CPU is no higher than 40 MHz.

*fun project idea:* Make a Serial scanner that monitors serial at an unknown baud rate and figures out what it is. You'd want the RX line of the serial port, and probably a event input pin piped to a type B timer, such that you could keep measuring to see what the shortest time betweentwo transitions that you see is 1 bit period. 1-over-period = baud rate.

See the [**AVR Baud Rate Chart**](https://docs.google.com/spreadsheets/d/1uzU_HqWEpK-wQUo4Q7FBZGtHOo_eY4P_BAzmVbKCj6Y/edit?usp=sharing) in google sheets for a table of actual vs requested baud rates.

Common Baud Rates

| Baud Rate |         Popularity | Notes
|-----------|--------------------|---------------------------------------------------------|
|      1200 | rare today, Legacy | A modern AVR cannot generate this above 10 MHz          |
|      2400 | rare today, Legacy | A modern AVR cannot generate this above 20 MHz          |
|      4800 | rare today, Legacy | A modern AVR cannot generate this above 40 MHz          |
|      9600 | V. high in Arduino | V. common default, so slow (1ms/char() that it causes new <br/> problems just because of how slow it is)  |
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
|    345600 |             Exotic | 1.5x 230400, continues the pattern. Only DxCore uses it <br/> in SerialUPDI, 460.8 exceeded write speed limit |
|    460800 |           Uncommon | The next to last of the standard usart speeds.          |
|    512000 |            V. Rare | As with 256k above, it looks more convenient than it is |
|    921200 |               Rare | You don't see this one in use much, that's for sure <br/> Hard to generate from most devices, and very close to 1mbaud |
|   1000000 |           Uncommon | The highest commonly used baud rate. more popular than <br/> 921.2kbaud easier to make by dividing common clocks |

These are some of the most common - most - but not all - adapters can generate nearly arbitrary baud rates. The CP2102 has a preprogrammed list, though the vendor has a utility that can change them. The HT42B534 has no mechanism for changing the baud rates, etc. Low-end serial adapters usually cap out at 1.5, 2. or 3mbaud. The highest I have seen is 6 mbaud, which is fast enough that the wire's electrical effects would be relevant and communication would be fiddly and fussy.
