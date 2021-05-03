# Datasheet Links
The most recent versions of the datasheets for the tinyAVR product line are combined versions across different pin counts. This page collects links to the most recent versions of the datasheets as of May 2021, after they reorganized their website *AGAIN*

"Silicon Errata" is a euphemism for "design flaw". Their behavior is more akin to a software bug than a traditional hardware flaw (they are sometimes called "silicon bugs"). Whatever you call them, these parts have em in spades. Most of them are documented in the corresponding Errata and Data Sheet Clarification document, assuming Microchip is aware of it and accepted that it was a bug (as of early 2021, things are still being added to the tinyAVR errata sheets). This is a stark contrast to the classic AVRs, where the peripherals were simpler (less room for bugs to hide) and had been in production for so long that any issues had been worked out long ago. Not coincidentally, that's the same reason why parts with better peripherals were introduced other product lines that left AVR in the dust... It's to be expected that the first few part families with new peripherals will have a few issues. What isn't clear though is to what extent they are actually planning on fixing these in existing products (or even new ones - the AVR Dx product line had a lot of our old friends on it). On the other hand, the tinyAVR 2-series errata lists are very short indeed. It's hard to be sure though, if that's because everything is working, or because they just haven't been out long enough to find the bugs.

Both sorts of documents tend to be grouped by flash size, instead of pincount (which is what most people would categorize microcontrollers by). The fact that errata follows the flash size rather than the pincount implies that all the parts of a given flash size and product family use the same die, and the difference is just a matter of how many bonding wires they connected, and what they told the chip that it was during the factory calibration. Automotive parts (part numbers end in -VAO, and are almost never available for sale to the general public - though I have found no indication that there are substantial differences beyond the oscillator always working at 16 MHz, not 20...

## 2-series
* [ATtiny1624/1626/1627 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1624-26-27-DataSheet-DS40002234A.pdf)
* [ATtiny1624/1626/1627 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1624-26-27-SilConErrataClarif-DS80000902B.pdf)

* [ATtiny424/426/427/824/826/827 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny424-426-427-824-826-827-DataSheet-DS40002311A.pdf)
* [ATtiny424/426/427/824/826/827 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny424-426-427-824-826-827-SilConErrataClarif-DS80000955A.pdf)

## 1-series
* [ATtiny3216/3217 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny3216-17-DataSheet-DS40002205A.pdf)
* [ATtiny3216/3217 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny3216-17-SilConErrataClarif-DS80000887A.pdf)

* [ATtiny1614/1616/1617 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf)
* [ATtiny1614/1616/1617 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-SilConErrataClarif-DS80000886B.pdf)

* [ATtiny417/814/816/817 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny417-814-816-817-DataSheet-DS40002288A.pdf)
* [ATtiny417/814/816/817 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny417-814-816-817-SilConErrataClarif-DS80000934A.pdf)

* [ATtiny212/214/412/414/416 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny212-214-412-414-416-DataSheet-DS40002287A.pdf)
* [ATtiny212/214/412/414/416 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny212-214-412-414-416-SilConErrataClarif-DS80000933A.pdf)

* [ATtiny3216/3217 Automotive Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny3216-17-Auto-DataSheet-DS40002212A.pdf)
* [ATtiny3216/3217 Automotive Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny3216-17-Auto-SilConErrataClarific-DS80000890B.pdf)

* [ATtiny1614/1616/1617 Automotive Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-1616-1617-Auto-DataSheet-DS40002021C.pdf)
* [ATtiny1614/1616/1617 Automotive Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-Auto-SilConErrataClarif-DS40002129C.pdf)

* [ATtiny416/417/814/816/817 Automotive Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny416-417-814-816-817-Auto-DataSheet-DS40002014C.pdf)
* [ATtiny416/417/814/816/817 Automotive Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny416_417_816_817-Auto-Errata-Clarification-DS40002122B.pdf)

* [ATtiny212/214/412/414 Automotive Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny212-14-412-14-Automotive-DS40002229A.pdf)
* [ATtiny212/214/412/414 Automotive Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny212-214-412-414-Auto-SilConErrataClarif-DS80000893B.pdf)


## 0-series
* [ATtiny804/806/807/1604/1606/1607 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny804-06-07-1604-06-07-DataSheet-DS40002312A.pdf)
* [ATtiny804/806/807/1604/1606/1607 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny804-06-07-1604-06-07-SilConErrataClarif-DS80000951A.pdf)

* [ATtiny202/204/402/404/406 Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny202-204-402-404-406-DataSheet-DS40002318A.pdf)
* [ATtiny202/204/402/404/406 Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny202-204-402-404-406-SilConErrataClarif-DS80000956A.pdf)

* [ATtiny202/204/402/404/406 Automotive Data Sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny202-04-402-04-06-Auto-DataSheet-DS40002159A.pdf)
* [ATtiny202/204/402/404/406 Automotive Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny202-204-402-404-406-Auto-SilConErrataClarif-DS80000864B.pdf)
