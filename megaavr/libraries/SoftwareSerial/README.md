# SoftwareSerial
This is the same code included with the official "megaAVR" core. It sucks just as much on these parts. Software serial, by nature, will always suck, and this implementation is not especially good or anything - it is designed for flexibility and portability, not performance or maximum usable baud rates. I may at some point write a better one; I mocked up the ISR to do it, and it should be possible to receive at 460800 at 20 MHz system clock.....

SoftwareSerial is lucky to get 1/10th of that. Which is not unexpected when you look at the implementation. It is written in C, which just does not give the required level of control.

## Included examples

### SoftwareSerialExample
The most basic demonsration - it echos characters received on serial to the hardware serial port and vise versa.

Notice how the baud rate used is far lower than is typical for modern AVRs. It doesn't take much to overwhelm SoftwareSerial, especially implementations like this. Receiving requires that you make it to the actual the interrupt quickly enough that you're still in the start bit, and that the guess of how long it took is accurate, so slower baud rates give more margin for that. AttachInterrupt works directly counter to that requirement.

### TwoPortReceive
We recommend against the use of multiple software serial ports. On DxCore and megaTinyCore we recommend using no more than zero (0) software serial ports at any given time; One (1) at the most. They are flaky one at a time.

## So what should I do if I need more USARTs?
* Use a part with more hardware serial ports. 48-pin AVR Dx-series parts are not very expensive and give you 5 serial ports; they are cheaper than any classic megaAVR better than a 328p (which was pretty near the bottom of the barrel). 2-series tinies have 2 instead of the single one that 0/1-series tinyAVR had, though unfortunately it shares it's pins with the alt pinset of the other port - but they're also cheap. This isn't like the bad old days where just the chip with 4 USARTs cost over $10 (mega2560 chip alone) and $49.95 for an Arduino Mega! AVR128DA/DB64 is like $2.50 for the chip, and bare breakout boards can be had for a few bucks (I sell them! tindie.com/stores/drazzy ). USARTs are not the limited resource they used to be.
* Using multiple pinpositions with a hardware serial port, and swapping to the one you want to listen to. Nothing keeps you from writing PORTMUX registers while the peripheral is enabled
* By combining those - that gives you 3 on 20/24-pin tinies, 2 of which can receive at a time, or up to a dozen on an 64-pin Dx-series with 6 serial ports.

### It is possible to do significantly better
With more work and ingenuity you could get the CCL and a hardware serial port to do amazing things. Port in event RX mode, using the CCL to combine multiple pins into the one event channel the serial port is using. That could combine 3 pins into a single serial port, and then they would all work at the same time. With a clever ISR you could probably rig up things so that you both knew where each byte came from, and could block other ports from fighting with it (ex, a 1k resistor in series with the incoming data lines, and a port interruopt (probably wouldn't work with attachInterrupt - not as well at least) you could detect which pin went low with the incoming start bit, and write the port direction of the others to output... ). If you unset the direction of all the ports when it was idle for more than a couple of milliseconds (could be done with a TCB in timeout check mode, based on the same event channel as the serial port is ingesting) you'd basically never have a situation where they'd work with software serial but not in that mode, but you could run at higher baud rates and without all the misery and flakiness of software serial. If you needed more different baud rates than you had hardware serial ports, if you could accept losing the first byte, the aforementioned interrupt could totally turn off the RX half of the USART, change the baud rate, and reenable it before the first character has finished arriving.

The CCL can also be used to reroute TX to the default or alternate LUT output pin (one of the inputs is the state of the TX pin), allowing bidirectional communication in a scheme like the one described above.

### If none of those will do, there has GOT to be a better software serial library
You can get *much* better results if the software serial port calls a function to wait for an incoming char, and can thus poll the pin instead of using an interrupt. These can actually get reasonably normal baud rates to work - you can poll a serial port in a 4-clock loop. The catch is that you'd need to know when data was coming for ideal reasults.

A software serial library that defined the ISR directly instead of using attach would work much better than one that relies on attachInterrupt, like this one does. The overhead of making a function call, regardless of to what, from within an ISR is shockingly large; It's a fairly straight
