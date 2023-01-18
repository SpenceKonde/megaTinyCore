# Optiboot_x bootloader technical details

This directory contains the Optiboot small bootloader for modern tinyAVR devices, as distributed with megaTinyCore

This version may diverge from the official version of Optiboot (I know the handling of reset flags is different)

Optiboot is more fully described here: [http://github.com/Optiboot/optiboot](http://github.com/Optiboot/optiboot)

## Credit where credit is due
Optiboot is the work of **Peter Knight** (aka Cathedrow), building on work of **Jason P** **Kyle**, **Spiff**, and **Ladyada**.

More recent maintenance and modifications are by **Bill Westfield** (aka WestfW), including the optiboot_x version itself for the megaAVR 0-series and tinyAVR.

Further modification, most notably in handling of reset flags (always clear before jumping to app and stash in GPIOR0)

This is intended for developers; For a user's perspective, see [the Optiboot reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Optiboot.md).

## Reporting issues
This is being maintained as part of megaTinyCore, so issues be reported to:
[https://github.com/SpenceKonde/megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)
* This document has been converted into markdown for easier viewing on github.
* For some reason, the dummy app is supposedly required. This is *intensely annoying*, because avr-size includes it, and you thus get inaccurate numbers shown during the compile process. Since the binary size is of overwhelming importance when you're as close to the edge as we are here, we needed a better way. It seems that avr-size does not have a way to limit itself to arbitrary sections. To address this, the display mode of avr-size has been amended to use the -A option, showing the memory of all sections individually; this in turn means that there's a bunch of junk sections shown, but at least the three sections we care about are shown individually instead of being mashed together with the app. Sadly, windows lacks all the command line tools that linux has, so there isn't something we can easily pipe that output to which would turn
```text
section          size      addr
.data               0   8417280
.text             476         0
.application       42       512
.spmtarg            4       506
.version            2       510
.comment           17         0
.debug_aranges     48         0
.debug_info      2955         0
.debug_abbrev     724         0
.debug_line       937         0
.debug_frame      156         0
.debug_str       2004         0
.debug_loc        833         0
.debug_ranges     104         0
```
into what we'd really like:
```test
.text             476         0
.spmtarg            4       506
.version            2       510
```
Or, better:
```text
Size: 476+4+2
```
Or even better:
```text
Size: 482
```
If anyone reading this knows a way to do that, it would make me very happy if you would share that technique with me.

## Known issues
There are no known issues at this time (other than the fact that there is no EEPROM support That is because it does not fit. It might fit if we didn't need to buffer pages and could write data as it came in, but because we don't know we're getting a program page command until the fire hose of data has been turned on, we can't get rid of that so easily. It was a design decision to not lock in a 1024 byte bootloader section just to get EEPROM write capability; and the consequences are particularly serious on modern AVRs which cannot tolerate )

LED Pins
* 8-pin: PA3
* 14-24-pin: PA7
* Microchip boards: Whichever pin the LED is connected to

### Other resources

Arduino-specific issues are tracked as part of the Arduino project
at [http://github.com/arduino/Arduino](http://github.com/arduino/Arduino)


There is additional information in the wiki - however, the wiki does not reflect the changes specific to Optiboot DX
[https://github.com/Optiboot/optiboot/wiki](https://github.com/Optiboot/optiboot/wiki)


## To set up this environment in windows
This is how the files DxCore ships with are built. I can provide no guarantee that any other method will still work after what I've done to the makefile.
1. Download and extract the Arduino 1.0.6 .zip package.
* Make sure it is not in a "protected" location. I normally put them in the root of C:.
* Rename the folder so you remember what it's for; I have it in `C:\arduino-1.0.6-7.3.0compiler` (this location will be used in the examples below)
* You may delete everything except the hardware and tools subdirectories
2. Download [the updated toolchain`https://spencekondetoolchains.s3.amazonaws.com/avr-gcc-7.3.0-atmel3.6.1-azduino6-i686-w64-mingw32.tar.bz2`](https://spencekondetoolchains.s3.amazonaws.com/avr-gcc-7.3.0-atmel3.6.1-azduino6-i686-w64-mingw32.tar.bz2)
3. Extract the toolchain, and locate the top level 'avr' folder inside.
4. Copy the AVR folder to the `C:\arduino-1.0.6-7.3.0compiler\hardware\tools` directory in the 1.0.6 install. Be careful not to put it inside the avr folder that is already there (if you make a mistake, delete the mess and go back to step 1 - trying to unpick the mess is hopeless, because there are like 4 nested folders named avr, 2 folders in different locations named bin, and several 'include' folders. Don't look at me, I didn't choose that directory structure!
5. Copy the `optiboot_dx` folder from `(core install location)/megaavr/bootloaders` to `C:\arduino-1.0.6-7.3.0compiler\hardware\arduino\bootloaders`.
6. You should now be able to open a command window in `C:\arduino-1.0.6-7.3.0compiler\hardware\arduino\bootloaders\optiboot_dx` and run `omake <target>`, or `build_all_dx.bat` to build all the binaries packaged with the core.

This can be done the same way in linux exempt you can expect make to actually be there, ignore omake, and just use the shell scripts =nstead of batch files which If i had time to work on, could be mde a lot smoother, but that's not a priority (I think the main thing we'd want to to is pass the avr-size output throguh grep or something, but I don't know makefile or shellscripting :-) )


## Previous build instructions
These may or may not work. I am not versed in the ways of makefiles and am not competent to fix any issues that may be encountered. Thus, if you want to use one of these methods, you'll have to fix the makefile yourself (sorry - my brain uses 16-bit addressing. You'll notice I never talk about working with an ATmega2560 - that would require an extra bit, ya see? So figuring out how to wrangle a makefile on a 64-bit desktop computer is well beyond my ability). I will gladly accept pull requests that fix these things - as long as they do not break the method detailed above, as that would leave me unable to build DxCore bootloaders without toolchain wrangling, which as I said, is outside my zone of competence. If you find that these do in fact work as written, please let me know so I can update this file.

### Building optiboot for Arduino

Production builds of optiboot for Arduino are done on a Mac in "unix mode"
using CrossPack-AVR-20100115.  CrossPack tracks WINAVR (for windows), which
is just a package of avr-gcc and related utilities, so similar builds should
work on Windows or Linux systems.

One of the Arduino-specific changes is modifications to the makefile to
allow building optiboot using only the tools installed as part of the
Arduino environment, or the Arduino source development tree.  All three
build procedures should yield identical binaries (.hex files) (although
this may change if compiler versions drift apart between CrossPack and
the Arduino IDE.)


### Building Optiboot in the Arduino IDE Install

Work in the .../hardware/arduino/bootloaders/optiboot/ and use the
`omake <targets>` command, which just generates a command that uses
the arduino-included "make" utility with a command like:
    `make OS=windows ENV=arduino <targets>`
or  `make OS=macosx ENV=arduino <targets>`
On windows, this assumes you're using the windows command shell.  If
you're using a cygwin or mingw shell, or have one of those in your
path, the build will probably break due to slash vs backslash issues.
On a Mac, if you have the developer tools installed, you can use the
Apple-supplied version of make.
The makefile uses relative paths (`../../../tools/` and such) to find
the programs it needs, so you need to work in the existing optiboot
directory (or something created at the same "level") for it to work.



### Building Optiboot in the Arduino Source Development Install

In this case, there is no special shell script, and you're assumed to
have "make" installed somewhere in your path.
Build the Arduino source ("ant build") to unpack the tools into the
expected directory.


## Programming Chips Using the `_isp` Targets
This does not work on modern AVRs, and there are no plans to change this. It *could* be changed, but it was never enough of a hassle to just manually bootload the parts to make that unwieldy - the bootloader is, let's face it, pretty bloody simple, since just about everything but the bare essentials has been ripped out.

## Standard Targets
Being designed for megaTinyCore - there are 7 combinations of entry condition and timeout duration. There are additionally seven base configurations: txyz/txyzalt - tiny 0/1 on default or alt pins, txy2/txy2alt (8-pin 0/1-series), and tx2z/tx2zalt, tx2zu1alt
parameter, while parenthesis indicates a mandatory one - one of the options must be used. This gives 49 combinations for the generic bootloaders.
* **14+ pin tinyAVR 0/1:** `txyz[alt]_(one of: all_8sec, poronly_8sec, swronly_8sec (*), extronly (**), extr_8sec (**), extr (**))`
  * Supports all 14-24 pin 0 and 1-series parts, with the LED pin set to PA7
  * `*` swronly_8sec relies on the software issuing a software reset.
  * `**` extr versions enter the bootloader when an external reset is received. However, the external reset signal only can be benerated if there is a reset pin. But the pin that's nominally reset can be fused as either UPDI (the usual) or GPIO. Unless it is fused as reset, there can be no external reset signal, and hence these bootloader entry options should NOT be used. If the UPDI pin is programmed to act as reset, then be aware that you must use a tinyAVR compatible HV programmer to reprogram via UPDI.
* **8-pin tinyAVR 0/1:**`txy2[alt]_(one of: all_8sec, poronly_8sec, swronly_8sec (*), extronly (**), extr_8sec (**), extr (**))`
  * Supports all 8 pin 0 and 1-series parts, with the LED pin set to PA3
  * All other caveats as above.
  * Technically `txyz_alt` and `txy2_alt` are interchangible except for the different LED pin. The same is not true of txyz and txyz2
* **tinyAVR 2-series**`tx2z[alt or u1alt]_(one of: all_8sec, poronly_8sec, swronly_8sec (*), extronly (**), extr_8sec (**), extr (**))`
  * Supports all 2-series parts.
  * `*` as above.
  * `**` as above on 14-pin parts.
  * `**` On 20 and 24 pin parts, PB5 can be used as an alternate reset pin, making these entry modes far more useful. megaTinyCore defaults the alternate reset fuse setting for parts that have it, and our breakout boards provide a way to connect autoreset to this pin.
  * `alt` indicates a build for use with the
* For the Microchip boards
  * These differ in the location of the LED pin, and the serial pins are fixed as the ones connected to debugger serial. There are 6 of these that can have 4 possible options, and 1 than can have 7 for 31 binaries
  * `curiosity3217 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*))`
  * `curiosity1607 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*))`
  * `curiosity1627 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*), extronly (**), extr_8sec (**), extr (**))`
  * `xplainedpro3217 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*))`
  * `xplainedpro817 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*))`
  * `xplainedmini817 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*))`
  * `xplained416 (one of: _all_8sec, _poronly_8sec, _swronly_8sec (*))`
  * `**` external reset entry conditions only provided where the part can have an alternate reset (ie, on the curiosity 1627), because the debugger on these boards cannot generate the HV pulse to override a reprogrammed GPIO and would be harmed if an external HV programmer were used (and if if you've cut the strapping pins, you don't have one of these boards anymore and should use the generic )
* There are hence a total of 80 binaries

### The megaAVR 0-series is also supported by this codebase
Running make_all_mega0.bat in a suitable build environment will build all 42 binaries - 7 entry condition/timeout combinations times 2 (default or alt pins) times 3 USARTs = 42.

### All binaries are either 470 or 472 bytes in size, leaving 20 words free
* There is at least one pathological push-pop

## There are six entry condition options available
These control when Optiboot will "run" and how long it will wait for a new sketch when it does. Note that a WDT reset will *never* run the bootloader (we use a WDT reset to exit the bootloader, and we can't tell whether that was our own WDT reset or something else's), nor will a BOD reset not accompanied by something else ("weak or malfunctioning power supply" is something other than the ideal time to try to upload new firmware)
* `extr` - Bootloader will run on reset pinreset and software reset, with a 1 second timeout. For uploads with autoreset.
* `extr8sec` - Bootloader will run on reset pin reset and software reset, with an 8 second timeout. For uploads with manual reset.
* `all8sec` - Bootloader will run on everything except a WDT reset, 8 second timeout.
* `poronly8sec` - Bootloader will run only after a power on reset (POR), 8 second timeout. For manual power-cycle uploads when you're using reset as an input.
* `extronly` - Bootloader will run on a reset pin reset - but not a software reset - with a 1 second timeout. For uploads with autoreset. Will not work unless reset is enabled one way or another.
* `swronly8sec` - Bootloader will run only on a software reset, with 8-second timeout. Be aware that if your application does not perform a software reset, and does not smash the stack and crash, the bootloader will never run (since smashing the stack triggers a dirty reset, which in turn causes the bootloader to issue a software reset, that will run the bootloader).
* `porswr8sec` - Bootloader will run only on a software reset or power on reset, with 8 second timeout.

A UPDI reset (which will only occur normally when bootloading) is always an entry condition so you can use the flashing LED to ensure that the bootloader was written correctly

## The bootloader **clears the reset flags** and stashes it's value in GPIOR0 before jumping to the app
This is required in order to ensure that the entry conditions are honored, and to ensure that we don't try to run after a dirty reset without forcing a clean reset. With the reset flags cleared, a dirty reset is detected by a value of 0x00 in the reset flags.  In that case, we have no idea how the peripherals are configured, and since execution may have careened about the flash, whatever configuration the app had could be altered in arbitrary ways as well. Or it could have been caused by an interrupt with no ISR firing, in which case LVL0EX is still set and interrupts can't be enabled. Hence, the only action that can be expected to restore functionality is to issue a software reset.

Note that the same thing is done for non-optiboot configurations, where this step is instead done by the sketch in .init3 (ie, as soon as the stack pointer is initialized and r1 is cleared. The behavior is the same - reset flags cleared and stashed in GPIOR0.

## Making new entry conditions
This is very easy - you need only pass ENTRYCOND_REQUIRE=0xXX. This is compared to the reset flags.
The logic is simple:
1. If the the WDRF (WDT reset flag) is set, we always jump to the app in order to avoid an infinite loop: The bootloader exits by triggering a WDT reset, so a WDT reset means that either a WDT reset got us here, or the app triggered a WDT reset. If you want to start the bootloader from the app, trigger a Software Reset instead.
2. Otherwise, if the bitwise and of ENTRYCOND_REQUIRE and the reset flags is non-zero, we run the bootloader. Otherwise, we jump to the app.

## How come there are so few binaries needed compared to DxCore?
DxCore ships with 315 binaries to support the same entry conditions. The reason for this nauseating preponderance of binaries is that the Dx-series parts have FAR more base configurations - for the simple reason that they have so many more serial options.
* mTC supports 40 parts (4 8-pin parts, 12 14-pin parts, 12 20-pin parts and 12 24-pin parts) with 80 binaries.
* DxC supports 36 parts, but needs 315 binaries to do it.
* ATTc for classic AVRs supports a mere 14 parts with optiboot... but requires a mindblowing 586 bootloader binaries to do so, because of the dependence of the bootloader binary on the clock speed.
  * That doesn't count the additional 288 micronucleus bootloaders - 144 normal and 144 update binaries, which support just 8 parts!

## Used and unused working registers
The register pressure in Optiboot_x is markedly lower than the already low register pressure in optiboot_dx - less than half of the working registers are in use, though most of the unused registers are less useful lower registers. An upper register would allow you to save 1 word for every time you'd otherwise need to load a value into a register with LDI after the first (unless that register must be used because of the ABI), whereas a lower register can only start saving flash from every time you can replace an LDI with a register holding a constant value after the *second* occurrence (because you need to ldi -> mov it, and that - that is assuming you don't need to use immediate values on it).
```
  r0 - UNUSED
  r1
 r2-r12 - UNUSED
 r13
 r14 - UNUSED
 r15
 r16
 r17
 r18
 r19
 r20
 r21 - UNUSED
 r22
 r23 - UNUSED
 r24
 r25
 r26 - UNUSED
 r28
 r29
 r30
 r31
 ```

## Sorted assembly listing
As with Optiboot_dx, this is a highly atypical instruction distribution. Because these devices have more slack in the flash, less attention was paid to the annotations compared to the DX-series
```text
   0: 01 c0         rjmp  .+2             ; 0x4 <main>
   c: 05 c0         rjmp  .+10            ; 0x18 <main+0x14>
  1e: 03 c0         rjmp  .+6             ; 0x26 <main+0x22>
  a2: d3 cf         rjmp  .-90            ; 0x4a
  ba: da cf         rjmp  .-76            ; 0x70
  b2: e1 cf         rjmp  .-62            ; 0x76
  9c: ef cf         rjmp  .-34            ; 0x7c
  e8: d6 cf         rjmp  .-84            ; 0x96
 15a: 9d cf         rjmp  .-198           ; 0x96
  dc: dd cf         rjmp  .-70            ; 0x98
 116: c0 cf         rjmp  .-128           ; 0x98
  c4: e9 cf         rjmp  .-46            ; 0x98
 13e: ac cf         rjmp  .-168           ; 0x98
  cc: fa cf         rjmp  .-12            ; 0xc2
 160: bc cf         rjmp  .-136           ; 0xda
 166: b9 cf         rjmp  .-142           ; 0xda
 11a: f0 cf         rjmp  .-32            ; 0xfc
 142: f8 cf         rjmp  .-16            ; 0x134
 130: 07 c0         rjmp  .+14            ; 0x140
 16e: fc cf         rjmp  .-8             ; 0x168 <putch>
 17c: fc cf         rjmp  .-8             ; 0x176 <getch>
 188: 01 c0         rjmp  .+2             ; 0x18c <getch+0x16>
 194: fc cf         rjmp  .-8             ; 0x18e <watchdogConfig>
 1aa: ff cf         rjmp  .-2             ; 0x1aa <verifySpace+0xa>
 1ae: dc cf         rjmp  .-72            ; 0x168 <putch>
 1bc: f1 cf         rjmp  .-30            ; 0x1a0 <verifySpace>
   2: dd c0         rjmp  .+442           ; 0x1be <do_nvmctrl>
  30: e7 c0         rjmp  .+462           ; 0x200 <app>
  96: 68 d0         rcall .+208           ; 0x168 <putch>
  9a: 66 d0         rcall .+204           ; 0x168 <putch>
 14e: 0c d0         rcall .+24            ; 0x168 <putch>
 154: 09 d0         rcall .+18            ; 0x168 <putch>
 138: 17 d0         rcall .+46            ; 0x168 <putch>
  d2: 51 d0         rcall .+162           ; 0x176 <getch>
  d6: 4f d0         rcall .+158           ; 0x176 <getch>
  ee: 43 d0         rcall .+134           ; 0x176 <getch>
  f0: 42 d0         rcall .+132           ; 0x176 <getch>
  f4: 40 d0         rcall .+128           ; 0x176 <getch>
  fc: 3c d0         rcall .+120           ; 0x176 <getch>
 120: 2a d0         rcall .+84            ; 0x176 <getch>
 122: 29 d0         rcall .+82            ; 0x176 <getch>
 126: 27 d0         rcall .+78            ; 0x176 <getch>
 1a0: ea df         rcall .-44            ; 0x176 <getch>
 1b4: e0 df         rcall .-64            ; 0x176 <getch>
  7c: 7c d0         rcall .+248           ; 0x176 <getch>
  82: 79 d0         rcall .+242           ; 0x176 <getch>
  2e: af d0         rcall .+350           ; 0x18e <watchdogConfig>
  6a: 91 d0         rcall .+290           ; 0x18e <watchdogConfig>
 1a8: f2 df         rcall .-28            ; 0x18e <watchdogConfig>
 164: 14 d0         rcall .+40            ; 0x18e <watchdogConfig>
 12a: 3a d0         rcall .+116           ; 0x1a0 <verifySpace>
  86: 8c d0         rcall .+280           ; 0x1a0 <verifySpace>
  da: 62 d0         rcall .+196           ; 0x1a0 <verifySpace>
 148: 2b d0         rcall .+86            ; 0x1a0 <verifySpace>
 106: 4c d0         rcall .+152           ; 0x1a0 <verifySpace>
  e4: 65 d0         rcall .+202           ; 0x1b0 <getNch>
  c2: 76 d0         rcall .+236           ; 0x1b0 <getNch>
  76: 0d e9         ldi   r16,    0x9D    ; 157
  a6: 2e ec         ldi   r18,    0xCE    ; 206
  10: 21 e0         ldi   r18,    0x01    ; 1
  1a: 30 e0         ldi   r19,    0x00    ; 0
  a8: 36 e5         ldi   r19,    0x56    ; 86
  c0: 84 e1         ldi   r24,    0x14    ; 20
  ca: 85 e0         ldi   r24,    0x05    ; 5
  e2: 84 e0         ldi   r24,    0x04    ; 4
  e6: 80 e0         ldi   r24,    0x00    ; 0
 12c: 86 e4         ldi   r24,    0x46    ; 70
 162: 81 e0         ldi   r24,    0x01    ; 1
 1a6: 81 e0         ldi   r24,    0x01    ; 1
 1ac: 84 e1         ldi   r24,    0x14    ; 20
  78: 83 e0         ldi   r24,    0x03    ; 3
  88: 81 e0         ldi   r24,    0x01    ; 1
  8e: 89 e0         ldi   r24,    0x09    ; 9
  94: 83 e0         ldi   r24,    0x03    ; 3
  52: 81 e0         ldi   r24,    0x01    ; 1
  58: 83 e0         ldi   r24,    0x03    ; 3
  62: 80 ec         ldi   r24,    0xC0    ; 192
  68: 8b e0         ldi   r24,    0x0B    ; 11
  6e: 87 e0         ldi   r24,    0x07    ; 7
  98: 80 e1         ldi   r24,    0x10    ; 16
  9e: 83 e7         ldi   r24,    0x73    ; 115
 1c2: 8d e9         ldi   r24,    0x9D    ; 157 <__PROTECTED_WRITE_SPM
  2c: 80 e0         ldi   r24,    0x00    ; 0
  46: 8c e5         ldi   r24,    0x5C    ; 92
  48: 90 e0         ldi   r25,    0x00    ; 0
 196: 98 ed         ldi   r25,    0xD8    ; 216 <__PROTECTED_WRITE
  a0: 90 e0         ldi   r25,    0x00    ; 0
   e: 98 ed         ldi   r25,    0xD8    ; 216 <__PROTECTED_WRITE
  7a: d8 2e         mov   r13,    r24
 128: f8 2e         mov   r15,    r24
  84: 18 2f         mov   r17,    r24
  f2: 18 2f         mov   r17,    r24
 124: 18 2f         mov   r17,    r24
  18: 28 2f         mov   r18,    r24
  d4: c8 2f         mov   r28,    r24
 1b2: c8 2f         mov   r28,    r24
  d8: d8 2f         mov   r29,    r24
 1d4: fc 01         movw  r30,    r24
  8a: 12 38         cpi   r17,    0x82    ; 130
  90: 11 38         cpi   r17,    0x81    ; 129
 1be: 68 30         cpi   r22,    0x08    ; 8
  bc: 82 34         cpi   r24,    0x42    ; 66
  c6: 85 34         cpi   r24,    0x45    ; 69
  ce: 85 35         cpi   r24,    0x55    ; 85
  de: 86 35         cpi   r24,    0x56    ; 86
  ea: 84 36         cpi   r24,    0x64    ; 100
  f6: 86 34         cpi   r24,    0x46    ; 70
 11c: 84 37         cpi   r24,    0x74    ; 116
 144: 85 37         cpi   r24,    0x75    ; 117
 15c: 81 35         cpi   r24,    0x51    ; 81
 1a2: 80 32         cpi   r24,    0x20    ; 32
  42: 81 30         cpi   r24,    0x01    ; 1
  7e: 81 34         cpi   r24,    0x41    ; 65
 100: 21 96         adiw  r28,    0x01    ; 1
 136: 21 96         adiw  r28,    0x01    ; 1
  b4: 21 50         subi  r18,    0x01    ; 1
  70: 81 50         subi  r24,    0x01    ; 1
  b6: 31 09         sbc   r19,    r1
 102: 11 50         subi  r17,    0x01    ; 1
 13a: 11 50         subi  r17,    0x01    ; 1
 1b6: c1 50         subi  r28,    0x01    ; 1
  fa: d0 58         subi  r29,    0x80    ; 128
 118: dc 5e         subi  r29,    0xEC    ; 236
 132: d0 58         subi  r29,    0x80    ; 128
 140: dc 5e         subi  r29,    0xEC    ; 236
   4: 11 24         eor   r1,     r1
  22: 23 2b         or    r18,    r19
  20: 2d 7f         andi  r18,    0xFD    ; 253
  40: 83 70         andi  r24,    0x03    ; 3
 112: 83 70         andi  r24,    0x03    ; 3
 1ce: 83 70         andi  r24,    0x03    ; 3
  32: a8 95         wdr
  74: a8 95         wdr
  aa: a8 95         wdr
 18a: a8 95         wdr
 18c: 08 95         ret
 19e: 08 95         ret
 1d2: 08 95         ret
 1d8: 08 95         ret
 174: 08 95         ret
  34: 22 9a         sbi   0x04,   2       ; 4
  36: 2a 9a         sbi   0x05,   2       ; 5
  6c: 07 9a         sbi   0x00,   7       ; 0
  a4: 17 9a         sbi   0x02,   7       ; 2
  fe: 88 83         st    Y,      r24
 1d6: 40 83         st    Z,      r20
  12: 94 bf         out   0x34,   r25     ; 52
  2a: 8c bb         out   0x1c,   r24     ; 28
 108: 04 bf         out   0x34,   r16     ; 52
 198: 94 bf         out   0x34,   r25     ; 52
 1c4: 84 bf         out   0x34,   r24     ; 52
  14: 20 93 41 00   sts   0x0041, r18     ; 0x0041
  26: 80 93 40 00   sts   0x0040, r24     ; 0x0040
  38: 10 92 01 02   sts   0x0201, r1      ; 0x0201
  4a: 80 93 08 08   sts   0x0808, r24     ; 0x0808
  4e: 90 93 09 08   sts   0x0809, r25     ; 0x0809
  54: 80 93 0b 08   sts   0x080B, r24     ; 0x080b
  5a: 80 93 07 08   sts   0x0807, r24     ; 0x0807
  5e: 10 92 05 08   sts   0x0805, r1      ; 0x0805
  64: 80 93 06 08   sts   0x0806, r24     ; 0x0806
 10a: d0 92 00 10   sts   0x1000, r13     ; 0x1000
 170: 80 93 02 08   sts   0x0802, r24     ; 0x0802
 19a: 80 93 00 01   sts   0x0100, r24     ; 0x0100
 1c6: 60 93 00 10   sts   0x1000, r22     ; 0x1000
 134: 88 81         ld    r24,    Y
   6: 80 91 40 00   lds   r24,    0x0040  ; 0x0040
  3c: 80 91 82 12   lds   r24,    0x1282  ; 0x1282
  ac: 90 91 04 08   lds   r25,    0x0804  ; 0x0804
 10e: 80 91 02 10   lds   r24,    0x1002  ; 0x1002
 14a: 80 91 00 11   lds   r24,    0x1100  ; 0x1100
 150: 80 91 01 11   lds   r24,    0x1101  ; 0x1101
 156: 80 91 02 11   lds   r24,    0x1102  ; 0x1102
 168: 90 91 04 08   lds   r25,    0x0804  ; 0x0804
 176: 80 91 04 08   lds   r24,    0x0804  ; 0x0804
 17e: 90 91 01 08   lds   r25,    0x0801  ; 0x0801
 182: 80 91 00 08   lds   r24,    0x0800  ; 0x0800
 18e: 90 91 01 01   lds   r25,    0x0101  ; 0x0101
 1ca: 80 91 02 10   lds   r24,    0x1002  ; 0x1002
 1b0: cf 93         push  r28               <pathological
 1ba: cf 91         pop r28                 <pathological
 1c0: 48 f4         brcc  .+18            ; 0x1d4 <do_nvmctrl+0x16>
 15e: 09 f0         breq  .+2             ; 0x162
 1a4: 19 f0         breq  .+6             ; 0x1ac <verifySpace+0xc>
  24: 31 f4         brne  .+12            ; 0x32 <main+0x2e>
  44: 61 f5         brne  .+88            ; 0x9e
  72: c1 f4         brne  .+48            ; 0xa4
  80: e9 f4         brne  .+58            ; 0xbc
  8c: 21 f0         breq  .+8             ; 0x96
  92: 09 f0         breq  .+2             ; 0x96
  b8: c1 f7         brne  .-16            ; 0xaa
  be: 19 f4         brne  .+6             ; 0xc6
  c8: 11 f4         brne  .+4             ; 0xce
  d0: 31 f4         brne  .+12            ; 0xde
  e0: 21 f4         brne  .+8             ; 0xea
  ec: b9 f4         brne  .+46            ; 0x11c
  f8: 79 f4         brne  .+30            ; 0x118
 104: d9 f7         brne  .-10            ; 0xfc
 114: e1 f7         brne  .-8             ; 0x10e
 11e: 91 f4         brne  .+36            ; 0x144
 13c: d9 f7         brne  .-10            ; 0x134
 146: 51 f4         brne  .+20            ; 0x15c
 1b8: e9 f7         brne  .-6             ; 0x1b4 <getNch+0x4>
 1d0: e1 f7         brne  .-8             ; 0x1ca <do_nvmctrl+0xc>
 17a: 87 ff         sbrs  r24,    7
  1c: 83 fd         sbrc  r24,    3
  b0: 97 fd         sbrc  r25,    7
 16c: 95 ff         sbrs  r25,    5
 186: 92 fd         sbrc  r25,    2
 192: 90 fd         sbrc  r25,    0
   a: 81 11         cpse  r24,    r1
 12e: f8 12         cpse  r15,    r24
 ```
