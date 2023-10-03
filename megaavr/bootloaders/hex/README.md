# Here live the bootloader hex files

## .lst and .elf files
We include .lst files, and the .elf files in case you for some strange reason need them, these are in listings.tar.bz2 and elves.tar.bz2.

## megaAVR 0-series parts
There is a tar'ed bzipped archive containing bootloaders for the mega0-series built to the same specifications

## Build logs
Build logs are now included - both normal output and error output.

## Filename encoding
The filenames consist of three or four sections, separated by an _

* "optiboot" - identifies the bootloader
* A string identifying the chip or board:
  * "txyz": Any 0/1-series tinyAVR except the 8-pin ones.
  * "txy2": Any 0/1-series tinyAVR with 8 pins.
  * "tx2z": Any 2-series tinyAVR
* If it is set to use anything other than USART0 on default pins, a third parameter appears here:
  * (omitted): USART0, default mapping.
  * "alt": USART0, alternate mapping.
  * "u1alt": USART1, alternate mapping.
  * There is no option to use USART1 on default mapping, because the default USART1 mapping is the same as the alternate USART0 mapping.
* Finally the entry conditions are described:
  * "extr": Upon external, software, or UPDI reset (UPDI reset means you just uploaded the bootloader (most likely) so you'd like to see the bootloader run so you know it's there).
  * "extr8sec": As above, but wait 8 seconds for upload.
  * "extronly": Only upon external or UPDI reset. Note that this requires that you configure fuses to have a reset pin or it won't be much fun.
  * "poronly8sec: POR or UPDI reset only, and wait 8 seconds (for plug/unplug to enter bootloader)
  * "all8sec": Maximum ease of bootloader entry. Enter bootloader on any reset, except for a WDR (which is impossible for technical reasons), or a BOR (indicative of an unstable power supply. That's not when you want to be writing to flash is it?), 8 second wait.
  * "swronly8sec": Only upon a software or UPDI reset, 8 second timeout. Note that this fails to adhere to the prime directive. An application which does not fire a software reset will not enter the bootloader after the first time it was programmed, however, there are a surprising number of people who are doing weird stuff like that, and are okay with the possible need to take drastic actions (some of them have failsafes in their sketch)


## What is that technical reason?
Optiboot exits using the WDR to make sure everything is cleanly reset for app - since only the RSTRF persists across reset and we can only clear the bits never set them, there is no way to tell if a WDRF came from itself or the app, and so, since assuming it was from the app would result in the bootloader never exiting.

## Related to above
Since a startup *without* a hardware reset is guaranteed not to work (GNTW), and means that the prime directive of a bootloader is violated ("Survive any behavior that the application could manifest, no matter how perverse."), because an application that smashes the stack, or fires an interrupt that doesn't exist, or manually jumps to 0 would end up at 0, and the bootloader (in order to fit in flash) assumes the chip is in reset state (ie, that it is clocked at 3.33 MHz or 2.66 MHz, and that the USART is not currently enabled and so on). Hence we have no choice but to ensure that we can detect that case and fire an SWR to reset cleanly (likely into the bootloader) when we start up with no reset flags. Jumping to 0x0000 is prohibited.

The minimum needed to avoid GNTW behavior without removing any functionality is what we implement:
* Either in the bootloader or if none is used, the start of the application, we must read the reset flags.
* If they are zero, we must issue a software reset.
* If it's not an entry condition, we clear the flags, write the value they held to GPIOR0 (so the app can see them without hving to fish something our of r2, which cannot be guaranteed to even be possible for any given sketch; this method minimizes the cost, and since we never *read* GPIO0, you may freely trash the value in it if you don't need the reset cause flags.)
