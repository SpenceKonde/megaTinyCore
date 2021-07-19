# Contents
The tools directory contains tooling written in the python language for uploading code. Currently, it is all related to the serial-updi (UPDI over a simple serial adapter)


## SerialUPDI
Arduino <--> pymcuprog Bridge
By Spence Konde and Quentin Bolsee
An Arduino IDE friendly launch script for pymcuprog, including performance enhancements .

### Required Hardware
There has been some debate and questions have been raised over whether the recommendations below are the best. More study is required.
1. A USB serial adapter These can be had for as low as $1 on ebay and aliexpress based on the CH340G, slightly more for CP210. Ideally, you want to dedicate a serial adapter to this purpose for ease of use, rather than havign to connect and disconnect things every rtime you want to use it.
1. a fast signal schottky diode (recommended) or a resistor (see below to figure out value)). Schottky, not silicon! Silicon are on the edge of working, when the whole point of using a schottky diode instead of a resistor is to move the system thoroughly into the working regime, instead of running on the edge between working and not working. The BAT-85 is a readily available through-hole part.
1. 1 resistor, a few hundred ohms - 220 to 1k, even 2k is fine (may not be needed).
2. A few jumper wires.

### Serial adapter requirements
Almost any serial adapter can be used for pyupdi style programmer, as long as you take care to avoid these pitfalls:
1. The FTDI FT232, (both the genuine ones, and the fakes) are by default configured to use less CPU time by polling the adapter less often. For UPDI programming, the performance implications are severe: Even with the minimum latency timeout, it's a large portion of the time taken for a Dx-series part is spent in latency, not transferring data, and on the tinyAVR parts, latency dominates the write process. With the default timeout, it is 16 times slower. Fortunately, it is very easy to configure You can easily fix this (at least on windows - I'm not sure if the problem even happens on Linux): Open device manager, under Ports (COM and LPT), locate the FTDI adapter. Right click -> properties. Click the Port Settings tab, and then the Advanced button. In the middle of left hand side, there is an option called "Latency timer", likely set to 16ms. Set it to 1ms). Click OK enough times to leave the dialog. you'll see the adapter disappear and reappear as the change is applied. Configured properly, the FT232RL has spectacular performance. Configured improperly, the performance is downright abysmal.
2. MOST SERIAL ADAPTERS ALREADY HAVE THEIR OWN RESISTOR in series with Tx! Typically between 1k and 2.2k; If using the recommended diode method, you need only add the diode, not the second resistor. If you aren't going with the diode method for some reason, use a resistor such that the total is 4.7k (you'll need to measure it with multimeter unless you can follow traces to the resistor and read out the code (the three number codees - first two numbers are the most significant digits, and the third is the number of zeros they are followed by - 222 is 2200 - 2.2k, 471 is 470, (which of course means that 101 means a 100 ohm resistor, and 100 means a 10 ohm one!)).  There are three approaches to measuring this; all wuith one end of multimeter clipped to Tx. Either a) look up the pinout of the serial adapter chip online and measure resistance to that pin, b) check all the pins on the chip. Only one will have a value of a few k or less or c) just go for the ends of the resistors, expecting one to have a 1-2.2k to Tx on one side, and 0 on the other.
The TX resistor is ubiquitous on CH340-based adapters. On FT232 adapters, it is not usually present. I suspect this difference is due to the fact that the CH340 can output true 5v TTL serial, while most others are 5v tolerant, but only go to 3.3v on their own.
3. Some serial adapters have a dedicated LED to indicate Rx. While some fancy chips have an I/O pin that drives the RX led (the FT232, for example), a cheap adapter with an RX LED may have just put an LED and resistor on the RX line (in fact that's what that above mentioned green boards with switch and microusb port did).. The load from an LED on the UPDI line will overwhelm any signal from the target and prevent communication (a LED on TX is fine - the adapter has plenty of drive strength.)
4. Devices that do not output a 5v HIGH level sometimes will work at low speed but not higher speeds. A 10k pullup between the UPDI/RX line and 5v will usually sort this out.

I recommend the CH340 as the go-to serial adapter for several reasons:
* They are dirt cheap and readily available on ebay/aliexpress/amazon. 7/$5 shipped on aliexpress for the most basic design (the black kind with the stupid voltage jumper on the end. The wider ones with rounded corners and ENIG surface treatment are no better electrically, though the build quality is better (if you buy the black ones described above, it is recommended to solder the tabs ofg the USB connector in place - their wave soldering process fails to solder these most of the time), and are significantly bulkier. The better kind are what you find on ebay when you search for "CH340 6pin" - you want the black ones with the voltage switch. Until, that is, I bring my own design into production - it'll have voltage switching done *right* and a micro USB connector instead of a stuipid full sized USB-A, full-sized 1117 3.3v regulator, and a color-coded power light to indicate the current voltage setting, and a general absence of gross design flaws.
* The bare chips are dirt cheap and readily available - and also dead simple to design with. Anyone, even the clown who designed those green boards I've been badmouthing, can do it (it looked like someone's first board, frankly). Heck, a working CH340G serial adapter was the first board I designed, too (at least I didn't try to sell my first version!).
* They are - at most - a half second slower to write a full flash image with. For the price of one real FT232RL adapter, you could have half a dozen CH340's.
* They generally work at the maximum speed with just the addition of the diode

And because most of the alternatives have problems:
* The CP2102 doesn't support the top speed for Dx-series (345600 baud) - or, if you configure it to support that with the utility from Silicon Labs, you lose support for 460800 for tinyAVR.
* The PL2303 you have to downgrade the drivers for, because they're all counterfeit versions of an ancient version.
  * One PL2303 adapter I got had virtually no capacitance on Vcc - there was 0.5v ripple on the 3.3v line. Adding a capacitor fixed that, but seeing as they aren't evenh cheaper, there's no reason to use them.
* The FT232 requires you to change the timeout setting to get acceptable performance.
  * Most FT232RL's are said to be counterfeit; the "FTDI-gate" driver debacle in far behind us, but this may matter to you for other reasons.
* As noted above, very few serial adapters output an actual 5v HIGH level, and these are sometimes flaky at higher speeds, and need an added pullup on the UPDI line. This is not needed for CH340.

* The HT42B534 has a variety of unusual attributes, some good, some bad.
  * It doesn't support arbitrary baud rates - no 345600 for Dx-series, or anything between 256k and 460.8k. This kind df sucks.
  * On the one hand, it uses standard windows CDC drivers; that's not a bad thing. (Not that I've ever found keeping serial drivers installed for other adapters to be a terrible burden.) Except when the standard drivers don't work very well. Setting it to a speed it doesn't like not only doesn't have the desired effect, but the attempt blocks rather than terminating gracefully. serialupdi has a fairly short timeout builtin and errors; many terminal programs will just hang until you restart them or unplug the serial adapter and plug it back in. Sometimes it stays behind in device manager after unplugging it, particularly after it hangs.
  * Modem control inputs are wrong: All show off until any one changes state, then all show opposite of actual value. Inverted behavior continues until next reset.
  * The chip (though not the available adapters) have a VDDIO pin to set output voltages, ie, a built-in level shifter. CH340 can only do 3.3V-5V, HT42 can do 1.8-5v. It would be a very nice feature if it was exposed in breakout boards;
  * They are almost CH340 cheap.
  * The latency is significantly lower than other serial adapters. Earlier during the spring 2021 optimization effort, after optimization for the "simpler case" of writing to a Dx-series (it is simpler in that there are larger pages, hence larger blocks of time spent, the speed of uploads was determined in large part by latency, rather than raw speed. This version performed significantly better on the HT42B534. Subsequent optimization allowed the speed of the other adapters to nearly equal that of the HT42B534, without the rest of it's quirks.
  * Unfortunately as a consequence of that, at higher baud rates it fails to program tinyAVR parts because it starts sending the next before a page write cycle is completed. while the other adapters have so much latency that they give the chip time for it's page write by default. This issue occurred with a number of other adapters as well and is fixed in SerialUPDI 1.2.0

### A note on breakout boards
Some tinyAVR and other UPDI-based part breakout boards have an on-board resistor. Sometimes this is a 4.7k one. That is NOT appropriate. I was part of the problem for a while. I think the original mistake came from people conflating the pyupdi resistor with a generally appropriate one. When I started megaTinyCore, my early collaborator was making hardware with a 4.7k resistor; I assumed he was doing it right. While this does work with dedicated programmers, including jtag2updi, it doesn't work with serial UPDI. It will work with dedicated programmers like jtag2updi, as long as they don't have their own resistor. Suffice to say, for a time it was a very common belief. I use 470 ohms now, but I can't find fault with a design over it not having the resistor. Expecting the programmer to be able to provide series resistance is not an unfair expectation,

### Connections:
* Vcc, Gnd of serial adapter to Vcc, Gnd of target
* Add either a resistor or schottky diode between Tx and Rx (in the case of unmusual serial adapters without their own TX series resistor, an external one is needed. See the charts below.
* Many adapters have a built-in 1k, 1.5k, or 2.2k resistor in series with Tx.
  * If yours does, you can simply install a diode from Rx to Tx, (band towards Tx). My top pick here is the BAT54C; it's in a tiny SOT-23 package (it's 2 diodes both weith the "band" towards the pin that's alone on one side) Why? Because, assuming your serial adapter has the pins on 0.1" header, and TX and RX are next to each other (both extremely common) the the diode fits right in between them. and with no lead that could later fatigue and break the result is less likely to be damaged by rough handling. Then if I want to more obviously mark it as a UPDI programmer, I might cut off the Tx, DTR and CTS pins. Serial adapters go for under for a buck a piece on ebay, so it's worth making dedicated UPDI programming ones.
  * If your adapter does *not* have a resistor in series with Tx, put a resistor (470-2.2k) in series with your diode - this is to protect both sides in the event of a total loss of synchronizatiopn where the target and programmer drive the pin in opposite directions, as well as damage if the adapter is plugged in backwards to an externally powered board (ie, with board Vcc connected to programmer serial lines - part of the protocol to deal with a target that's nonresponsive involves a very long "double break", though I think even unprotected, most adapters would survive a lot of double-breaks).
  * If you don't have any small schottky diodes on hand you can instead use a resistor - you want a total  of 4.7k resistance, counting your resistor and the one on the adapter, between Tx and Rx.
* Rx of adapter to UPDI pin of target. A small resistor (under 1k - like the 470 ohm one we generally use) in series with this on the target board is fine.


```
USB Serial Adapter
With internal 1-2k resistor on TX
This is the case in 90% of USB serial adapters.


Ideal:
internal resistor in adapter: 2.2k >= Ra

--------------------                                 To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI---\/\/---------->
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd    470 ohm (100 ~ 1k)
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'             If you make a 3-pin connector, use this pinout
--------------------

or

--------------------                                 To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI----------------->
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'
--------------------



Also works great, convenient if still using jtag2updi without resistor built into it. Resistorss should sum to less than 4.7k, preferalby much less

--------------------                                   To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI----\/\/\/---->
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd     < 2.2k
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'           Series resistor between header and chip UPDI pin on target PCB
--------------------                                I use 470 ohm resistors.

Yes internal resistor on adapter
Yes resistor on target: several k or more:
This will often by 4.7k: it must be bypassed, replaced with a smaller one or shorted out.


--------------------                                   To Target device
                DTR|               ,----------------------------------.
    internal    Rx |--------------/                  | UPDI----\/\/\/--*-
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd       4.7k
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .'          |__________________
                Gnd|--------------------'
--------------------



No internal resistor on adapter.
Yes resistor on target >= 100 ohms and not more than a few k.

--------------------                                   To Target device
                DTR|                                  __________________
 No resistor?   Rx |--------------,------------------| UPDI----\/\/\/------>
  Are you sure? Tx |----|<|------`          .--------| Gnd     > 100
 This is rare!  Vcc|---------------------------------| Vcc     < 2.2k
                CTS|                     .`          |__________________    Resistor of around a few hundred to a few k
                Gnd|--------------------'
--------------------


No resistor on target OR adapter:

Include A resistor in ONE of the three places shown below, whichever is easier to wire in,
the first and second positions, with nothing between Rx and target UPDI, are slightly preferable

Value of resistor should be a few hundred ohms, I'd default 470.

No internal resistor
--------------------                                   To Target device
                DTR|                                  __________________
 No resistor?   Rx |---------------------,--/\/\-----| UPDI---------------->
  Are you sure? Tx |--/\/\---|<|----\/\/'        .---| Gnd
 This is rare!  Vcc|---------------------------------| Vcc
                CTS|                          .`     |__________________
                Gnd|-------------------------'
--------------------


Resistor-based schemes - these have a narrower window of parameters under which they work reliably.
If you look at the UPDI line on a 'scope while it is malfunctioning, you will see that sometimes
the voltage is not going all the way down to ground when one side tries to assert it.

They are not recommended unless there is something keeping you from using a diode configuration.


The PyUPDI classic:

                         4.7k resistor
No internal resistor
--------------------                                   To Target device
                DTR|                                  __________________
 No resistor?   Rx |--------------,------------------| UPDI---------------->
  Are you sure? Tx |--/\/\/\/\---`          .--------| Gnd
 This is rare!  Vcc|---------------------------------| Vcc
                CTS|                     .`          |__________________
                Gnd|--------------------'
--------------------

Very much like the classic, except for the possiility of a resistor on the target. Must be 470 or under on the target.

Resistance should sum to 4.7k
--------------------                                   To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI----\/\/\/------>
  Tx---/\/\/\---Tx |--/\/\/\/\---`          .--------| Gnd     =< 470
    resistor    Vcc|---------------------------------| Vcc         \If resistor present, not more than 470 ohms.
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'
--------------------


If the resistor on the target is much more than 470 ohms, you're going to want to bypass it. Alternately,
it may be easier to replace it with a 0 ohm resistor or bridge it with a piece of wire or even
just a blob of solder, and do the classic pyupdi. Note that using a diode will work with resistances oin the target
that are too much for it to work using a resistor.


The resistor (if any) in serial adapter, and the one you add should total 4.7k.
--------------------                                   To Target device
                DTR|                       ,--------------------------.
    internal    Rx |--------------,-------'          | UPDI----\/\/\/- *--->
  Tx---/\/\/\---Tx |--/\/\/\/\---`          .--------| Gnd       >470
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________     \resistor of more than around  470 ohms - must be bypassed, replaced, or shorted.
or no resistor  Gnd|--------------------'
--------------------

If there's no resistor in the serial adapter and the target happens to have a 4.7k resistor, you can do it without
any extrta components, though you've got 4 wires involved instead of 3:

--------------------                                   To Target device
                DTR|              .-----------------------------------.
 No resistor?   Rx |-------------'      ,------------| UPDI----\/\/\/--*---->
  Are you sure?-Tx |-------------------'     .-------| Gnd       4.7k
 This is rare!  Vcc|---------------------------------| Vcc
 OR resistor    CTS|                      .'         |__________________
  bypassed      Gnd|---------------------'       Excessively - but conveniently - sized resistor.
--------------------                        These were (incorrectly) popularized for the first few years of UPDI



```

Why do we want a little bit of resistance (a few hundred) when doing the diode method? The classical response is "In case target and programmer are completely out of sync and try to drive the line in opposite direcctions" Note that on tinyAVR this isn't as much of a concern, as the output drivers on the UPDI/reset pin are so weak that it seems unlikely that they are capable of exceeding the maximum current per pin specification. Besides that, though, there's another reasonn: So that when (not if) you plug the connector in backward, but the target has external power too, there isn't a risk of the TX pin of adapter being damaged from trying to drive the positive supply rail low;

### Software

Choose a Serial-UPDI option from the Tools -> Programmer menu of a core which supports this upload method (megaTinyCore, DxCore, and soon, MegaCoreX). The tinyAVR core offers 57600 baud, 230400 baud and 460800 baud options. DxCore offers 57600, 230400, 345600 and a "special" 460800 w/chunking for use with the HT42B534 serial adapter, which can't do 345600 baud (and any other adapter runs far more slowly with this option) and select the Serial Port from the Tools -> Port menu.

Note that this does not give you serial monitor - you need to connect a serial adapter the normal way for that (I suggest using two, along with an external serial terminal application). This technique works with those $1 CH340 serial adapters from ebay, aliexpress, etc. Did you accidentally buy some that didn't have a DTR pin broken out, and so weren't very useful with the $2 Pro Minis you hoped to use them with? They're perfect for this. Although the CH340 parts have the lowest performance, the difference between parts is now quite small (this was not the case prior to the optimization leading up to the 2.3.2 release, before which the speeds ranged from 125 bytes per second to just over 1k.


### Upload and verify performance

    BAUD    |    FT232RL    kb/s   |     CP2102    kb/s |     CH340    kb/s  |    HT42B534    kb/s  |
------------|----------------------|--------------------|--------------------|----------------------|
115200      |     8.7 W /    8.8 R |     8.7 W /  8.8 R |  8.4 W /  8.5 R    |     9.0 W /  9.1 R   |
230400      |    16.6.W /   16.4 R |    16.3 W / 16.5 R | 14.6 W / 16.6 R    |    17.7 W / 17.9 R   |
345600*     |    24.3 W /   23.4 R |    23.2 W / 23.0 R | 22.5 W / 22.1 R    |        UNSUPPORTED   |
460800**    |             N/A      |                N/A |             N/A    |     24.7W / 32.7 R   |
** HT42B534 was run using a 32-byte block size, running with finite block size resulted in successful transfers for other parts, though the threshold block size varied - but a massive decrease in overall speed, similar to 115200 baud., as one will outrun the NVM controller writing at 460800 baud - I just had to see how it compared to the FT232RL. Both of them are running right up at the limit of the chip's ability to write data to the flash - and the FT232RL doesn't need any special measures taken and works with the tinyAVR parts too. On the other hand, the HT42B534 leads the pack at the (new as of 1.3.6) default of 230400 baud, and is dirt cheap (CH340-level prices).

For comparison, on the Dx-series parts (which are easier to use as test subjects since they have more flash, so uploads take longer and are easier to time. These numbers were taken using a 128k test image, which is an optimal situation.

Programmer      |  Read    | Write    | Notes                                    |
----------------|----------|----------|------------------------------------------|
jtag2updi       | 6.6 kb/s | 5.9 kb/s | Running on 16 MHz Nano                   |
Curiosity Nano  | 5.9 kb/s | 3.3 kb/s | Via avrdude - which is likely (hopefully!) not ideal  |
Optiboot Dx     |10.6 kb/s | 6.9 kb/s | 115200 baud as supplied by DxCore        |

Because of the smaller page sizes, ATtiny parts are slower to program; the smaller the pages, the slower the data rate - but the time for programming the entire flash is still less for smaller parts because there is less data to write. The two write numbers are for parts with 64 byte and 128 byte pages, respectively.

    BAUD      |     FT232RL    kb/s |     CP2102* kb/s   |     -   CH340    kb/s  |    HT42B534    kb/s       |
--------------|---------------------|--------------------|------------------------|---------------------------|
115200        | 4.6, 6.2 W /  8.8 R | 4.3,6.2 W / 8.8 R  | 3.6,   5.2 W  /  8.5 R |     3.6,  7.2 W  /  9.1 R |
230400        | 7.5,10.0 W / 16.5 R | 7.1,9.6 W / 16.4 R | 4.9,   7.8 W  / 15.6 R | Requires SerialUPDI 1.2.0 |
345600*       | 9.1,13.6 W / 23.4 R |8.5,13.2 W / 23.1 R | 5.6,   9.5 W  / 22.0 R |   345600 baud unsupported |
460800        |10.4,15.6 W / 28.2 R |        Not tested  | 6.0,  10.4 w  / 26.8 R | Requires SerialUPDI 1.2.0 |
* The CP2102 does not, by default, support any speeds between 256kbaud and 460800 baud - but a free configuration utility from Silicon Labs enables customization of the baud rates in each range of requested speeds (though unfortunately, you can't define those ranges). I reconfigured mine for 345600 baud for development of with the Dx-series parts, which don't work at 460800, and did not bother to set it back to factory settings just to fill in the table; I would expect to see approximately 10kb/s and 15kb/s write speeds and around 26kb/s read speed.

##### Note - those numbers were taken with SerialUPDI 1.1.0
Versions of the cores with later versions (megaTinyCore 2.4.0 and DxCore 1.4.0) will have slower write performance on tinyAVR due to changes made in order to guarantee that the part is given enough time to write each page. This is not enabled for TURBO mode; on Windows, an FT232 on minimum lastency timer and a CH340G have enough latency to do it without explicitly "waiting" for a page write to complete. Python's delay function, time.sleep() has a fairly coarse granularity, and is not well suited for this. We will continue to explore ways to reclaim as much of that lost speed as possible.

#### Observations on Speed
The following is based on the numbers above, test data not included, as well as examination of oscilloscope trace. Note that the 460800 baud data for the Dx-series is not directly comparable as it uses blocksize to artificially slow the writes to prevent overrunning the buffer on the target (which is barely a buffer) which the chip empties at a constant rate.

Approximately, write time can be modeled as:
T = ceil(size / pagesize) * C + size / (baud in bits/sec / 12 bits/byte)
where C in turn can be modeled as:
C = (c * n) + d/(baud / 12)

12 bits to 1 byte because the UPDI protocol uses 1 parity bit and 2 stop bits.

The same relationship holds - with a different constant - for read and write with the size of blocks read substituting for page size.

`d` is the overhead in bytes involved in setting up each page; it depends on the series of the part in question.

`c` is a charachteristic time depending on the serial adapter - the USB Latency. This is not actually a single consistent, constant value; potential values of it appear to be quantized and certain usb latency pauses within the upload process are of different durations.

`n` in a the number of separate USB transactions required to set up each page write, a function of the procedure used by the programming tool. Most of the optimization involved reducing this number; it is 2 for reads and Dx-series writes. For tinyAVR and megaAVR it is 4 when writing.

Monitoring the transactions on a scope, one can pauses with small exchanges p
(small overhead)->(latency) repeated n times per page, followed by a write lasting a period of time proportional to (pagesize + d)

Within the region of interest, baud is large relative to d, so C is nearly constant

At low baud rates, `T` tends towards a inverse relationship with baud.
At high baud rates, `T` tends toward a function of size, NVM version, and serial adapter only. Overcoming this limit requires reducing the number of USB transactions per page written, or block read.

T<sub>read</sub> is the same on all parts; an unimplemented change could push it a bit lower by bringing N all the way down to 1 from 2. The practical impact, however, is very small; after the end of the performance enhancement push, T<sub>read</sub>, reading in 512b blocks is dominated by the second term.

For the Dx-series parts as shown in the above table, with the full suite of optimization, T<sub>read</sub> = T<sub>write</sub> (the same change is likely possible here; the improvement is maybe 10%)

For tinyAVR, T<sub>read</sub> is the same as Dx, but T<sub>write</sub> is far worse - not only is pagesize smaller but `n` 4 instead of 2 so `C` is twice as high. At 460800 baud, the second term is about a third the magnitude of the first for write (and the tinies will accept writes at 691.2 kbaud). The important thing to keep in mind is that these chips still program in seconds - and the page write is being committed during the time that one of these pauses is ongoing.
