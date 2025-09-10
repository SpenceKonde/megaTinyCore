# Reference: All about Resets
Various reset sources are available, these conditions trigger a "hardware reset" with every register (except for the reset flag register, discussed shortly) reset to it's default value. Thus far all modern AVRs have had the same six reset sources. All are frequently encountered, and are important to understand. It's also important to be aware of the possibility that your code could reset - or appear to - without the chip actually resetting. I refer to this as a "dirty reset", and it is a Bad Thing. Frequently that is what is happening when you see an AVR get into a bad state after some adverse event, and not work until powercycled or reset (since the dirty reset detection went in, there should be very few of these). megaTinyCore will catch "dirty resets" pretty well, and reacts with a softweare reset (SWR) through the reset controller - in order to do this, however, at startup either the core, or the bootloader (if building for optiboot) will copy the reset flag register to GPIOR0 (so your code can use it) and then clear it (that way, when it next checks and sees that the register is zero, it knows the reset was dirty, the state of the hardware cannot be assumed. The least bad thing that can be done in the general case at that point, is to issue a proper reset, and that is what the core does. Because the only register that is specified to retain state across resets.

Note that a necessary consequence of the dirty-reset guard is that a duplicate guard will break, because the only way any dirty reset can be detected is if the reset flag register has been cleared before the dirty reset occurs. So in order to catch any dirty reset that could occur in bad user code, we must reset the reset flags first. Since checking and resetting the reset flags is essentially not present in the wild, despite being a basic and simple safeguard against the most severe and difficult to understand bugs, while properly the responsibility of an application, is performed by the core. Three behaviors have been used by various bootloaders. It is the opinion of the author of this document and the majority of this core that only one is correct.
1. The bootloader may do nothing. This is clearly a bug on classic AVRs (the bootloader takes longer than 16ms to run and, having neither reset the WDT nor turned it off, resets endlessly - this is an odd course of action for a bootloader to take, since the bootloader frequently exits by using), but was present on the original nano and pro mini bootloaders, which is why these boards are generally bootloaded as if they are Unos. On modern AVRs, this does not cause overt misbehavior, however neither does it prevent it. It is fully up to the app to detect and mitigate dirty resets (which as noted is essentially unhearof, and I do not expect to upgrade the wetware of every arduino user, especially when, as in this case, the correct response is always the same - You issue a software reset, because you are in an unmitigated error condition. This error condition occurs specifically because of something unforeseen and unintended occurring in code.
  a. Intentionally triggering a dirty reset was dangerous and inadvisable practice on classic AVRs to workaround the bug in early bootloaders that failed to handle the WDRF properly; the correct course of action is to bootload those devices as Unos or using MiniCore, and use the WDT to reset, not to trigger dirty resets directly by jumping to 0x0000.
2. The bootloader clears only WDRF. If a classic AVR, it turns off the WDT. Other reset flags left in place. This was used by Optiboot classically, and is, for example, what the Uno does. This achieves minimum functionality but because only WDRF is cleared, it is still left to user code (which can be relied upon not to do it) to clear RSTFR in order not only to detect dirty resets (which as far as I am aware was not done) and decide upon entry to the bootloader. That is to say, on a classic or modern AVR, the bootloader entry conditions are honored only if the user code clears RSTFR. Most users are unaware of this register, and hence do not reset it, and thus once the reset button has been pressed once, EXTRF remains set, until a BOR or POR occurs. Since EXTRF is the entry condition for the bootloader, once the reset button has been pressed once, it will alwaysrun. If PORF is the entry condition, if the user doesn't clear it, every restart will trigger the bootloader, even if the user specifically stated that thy wanted (say) only reset buttomn resets, or only software resets, to enter the bootloader. I consider this to be a bug, as it relies on people alwaystaking actions that hardly any users even know about. Not acceptable.
3. The bootloader checks the reset flags. If the flags are a non-entry condition (either because WDRF is set, or no entry condition flags are) clear them all, stash their value in GPIOR0, and then jump to the app. This guarantees a clean restart if something causes a dirty one, ensures that entry conditions are always honored, and also puts the results in a stable and sane part of memory (a register that is reset on reset, not reset while transitioning from bootloader to app, can be accessed without inline asm (unlike the "stash it in r2" approach. That'd be just dandy except you can't fish it out of there without writing assembly and then stuffing that assembly into a .init function or callback called from within one) (by the time you're at setup() it's entirely possible that the compiler decided to store something else in the (as far as it knows, unused r2.))). The GPIORs were the only ones that seemed suitable. I also needed somewhere to store two bits that indicated runtime errors from init_clock(). Thus, GPIOR0 gets data stored to it after a reset **but only right after a reset**.

**Note regarding Optiboot** All of these reset cause flag manipulation and stashing of GPIORs normally happens in optiboot - if you are using it. If you're not, all of the same functionality is bundled into init_three_stuff() which runs as soon as zeroreg and the stack pointer are initialized). The six low bits of GPIOR0 get written then. *They are never written or read by the core again*. You can read them if you care, and/or write them with simple assignment (`GPIOR0 = 0;`) before your first use of the register. If your first interaction with GPIOR0 is going to be to write a value with simple assignment, that's enough.

The two high bits of GPIOR0 may be written by the init_clock() function to signify error or conditions, particularly on megaTinyCore if you just can't resist overclocking, and on Dx/Ex for external crystals because the new driver is less vigorous than on classic AVRs (I think in the service of power efficiency). The GPIOR registers are never written by the core after this. In no case are they ever read by the core (indeed, since the bits are set one at a time with |=, and GPIOR0 is in the low IO space the core never even looks at the value - all it does is assign a value to it consisting of the reset flags, and potentially flipping one or both of the high bits to 1). Once init_clock has finished (on tuned configurations), or after .init3 (if untuned or external clock is used), the core will neither read nor write GPIOR0 and if all is well, you can assign a 0 to it in setup. Or grab the value from it first and then clear it, or if you don't care about GPIOR0, you don't have to do anything, since that register doesn't do anything except occupy a location in the low I/O space. (hence, there are 256 opcodes, each of which sets on of the 8 bits in one of the 32 registers in low I/O. and another 256 that clear them, for the bit tests. 1/64th of the instruction space (ie, available opcodes) are for bitlevel access to just 32 bytes.
```c
// .init0-.init.9 combined, in order
//
// Boot order:
// .inits
//   .init3 has onPreMain() callback available after doing the above listed things)
// main() {
//   onBeforeInit() callback - in this callback, if you wrote something to the top two bits of GPIOR0, on on a tinyAVR and were using a (tuned) internal setting}
//                             if there was a problem with the tuning configuration, bits might be replaced with 1's.
//   init() {
//     init_clock() { (note applies to megaTinyCore ONLY, and ONLY with tuned clock selected)
//          Here, when using a (tuned) clock options, there are two critical flags we need to make available to get the user started debugging. We need to let them know
//          if there is NO tuning data on the chip at all (hence, the speed may be off from requested speeds. Tune the bloody chip FFS) - this condition is indicated
//          by GPIOR0 | 0x40; If you get garbage from serial on a chip with tuned speed selected, you might try wiring an LED from Vdd through resistor to, say, PA6
//          and PA7, then: (if GPIOR0 | 0x40) {VPORTA.DIR |= 0x40;} if (GPIOR0 | 0x80) {VPORTA.DIR |= 0x80} at the top of setup. Since the chip was just reset, it
//          should be safe to assume that the pins have not been written high (that's why I said wire the LEDs between Vdd andtrhose pins, not ground ;-) )
//          the tuning process found that the target speeds requested just aren't possible on that specimen at that voltage. If it's a low frequency, the scillator was
//          too fast, if it's at the high end, either the oscillator wasn't running that fast even with the maximum speed cal or the chip was found to become unstable
//          during tuning before we got here.]
//       }
//       // Several other init functions are for various peripherals.
//       if(!onAgfterInit()) {sei();} // if user defines onAfterInit,
//   }
```

3. When the bootloader entry conditions are not met, the reset flags are read. If they are 0, indicating no reset has occurred, we immediately fire a software reset. If the entry conditions are not met, but the reset flags are non-zero, or if the entry conditions are met and the bootloader runs (and either times out or receives a new application), before jumping to the app the flags are cleared, and stashed someplace where a user of average skill can retrieve them (ie, NOT r2). A do-nothing register like GPIOR0 is well suited to this, and this is what we use the lower 6 bits of that register for (the remaining two are used to report issues with the tuned clock settings).

## Reset Sources

The reset sources are listed below. Note that for brevity and familiarity to those who used older AVRs, the reset sources are referred to by the parenthecised abbreviations and the reset flag is referred to without the underscores or the part before and after them, ex PORF. The first four reset flags also existed on classic AVRs and with the exception of WDRF, (see above) behaved the same way. Unlike a classic AVR, all resets are equal, except for their effects on `RSTCTRL.RSTFR`, with the sole exception of of the power on reset, which is different by nature, and cannot behave the same way (all volatile memory is lost in a POR, and hence everything is reset, including a small number of registers that are missed, often in error, by the normal reset routine).

The reset sources are listed below. Note that for brevity and familiarity to those who used older AVRs, the reset sources are referred to by the parenthecised abbreviations and the reset flag is referred to without the underscores or the part before and after them, ex PORF. The first four reset flags also existed on classic AVRs and with the exception of WDRF, behaved much the same way (WDRF was special on classic AVRs and had to be cleared in order to turn off the WDT, that is not the case anymore.

Update: Some new parts (in the Sx-series, which are going all-in on safety-critical) will have two additional bits, these are listed, with the note that they are only present on the SD, which would be under the aegis of DxCore (it's better than a DD, worse than a DB, but has a million safety features including two processor cores that eecute the same instructions, so it can throw an error if they don't agree; development effort would be needed to defang all of the wa. Needless to say I think that these parts will see little interest in the hobby and enthusiast community. I suspect that they will be pricey too). I'm not planning to (I foresee zero demand from arduino people), but if I ever did support it, there would not be a conflict over the two high bits of GPIOR, as they get used for tuning situations only, and only tinyAVR has oscillators with wide compliance
| Reset Source         | Flag name         | Flag bit |   .  | Set during:       | Clrared by:                | Notes
|----------------------|-------------------|----------|------|-------------------|----------------------------|-------
| Machine Check Reset  | RSTCTRL_MCRF_bm   |  1 << 7  | 0x80 | H/W sanity fail   | POR, BOR (by hw), nanually | Sx-series only. This is set when the internal hardware appears to be malfunctioning (lockstep cores disagreeing, ECC errors on RAM )
| ERRCTRL Reset        | RSTCTRL_ECRF_bm   |  1 << 6  | 0x40 | ERRCTRL causes rst| POR, BOR (by hw), manually | Sx-series only. They have a byzantine error-controller that manages all kinds of hardware errors and interrupts for them
| UPDI Reset           | RSTCTRL_UPDIRF_bm |  1 << 5  | 0x20 | UPDI programming  | POR, BOR (by hw), manually | With Optiboot bootloaders, this is a jump straight to app.
| Software Reset (SWR) | RSTCTRL_SWRF_bm   |  1 << 4  | 0x10 | When SWR used     | ..                         | Optiboot as supplied WILL run if SWRF is set. Thus, you can choose whether to reset to bootloader or app.
| Watchdog Reset (WDR) | RSTCTRL_WDRF_bm   |  1 << 3  | 0x08 | When WDT expires w/out wdr instruction<br/>wdr executed before window in windowed mode| POR, BOR (by hw), manually | With Optiboot, we cannot tell WDR from an exit from previous execution of Optiboot. Thus, this triggers reboot to the app.
| External Reset       | RSTCTRL_EXTRF_bm  |  1 << 2  | 0x04 | When reset pin is brought LOW | POR, BOR, Manually |
| Brownout Reset (BOR) | RSTCTRL_BORF_bm   |  1 << 1  | 0x02 | When Vdd lower than BOD threshold and BOD enabled. | POR, manually. resets all flags except POR.
| Power on reset (POR) | RSTCTRL_PORF_bm   |       1  | 0x01 | On power on (Vdd goes above V<sub>POR</sub> from below V<sub>PORR</sub>) | Manually. Resets all other reset flags

* All of these resets restore the peripheral registers [(SFRs)](https://github.com/SpenceKonde/AVR-Guidance/blob/master/Glossary.md#sfr) to their power on state - with the exception of the reset flag register, amd a small number of things that ONLY POR resets:
  * Some parts fail to reset the BOD/VLM registers except on POR (see errata if you're using VLM).
  * If an HV pulse was applied to the reset/UPDI pin successfully to force it into UPDI mode only POR will reset the pin to be GPIO or reset.
  * All reset flags are cleared by a POR (except for PORF itself which is set)
* The WDT does not turn itself back on when it causes a reset on modern AVRs. This is cause for celebration! That had been a source of constant annoyance in the past. It now defers to the fuses, which may or may not turn it back on. Which is obviously sensible in a way that forcing it on in a more aggressive setting than the user might ever wanted to use isn't. And back then that was the only way to software reset. The only thing that saved people was that optiboot cleared the WDRF andturned off the WDT anyway, because that's how it exited after timeout. Now it is far less of a pain to use it to generate a software reset and behaves in a more intuitively natural way. .
* But we also have a true software reset! So we can have the software working as intended and choose to reset, or detect a hang and reset itself *and tell the difference between those cases* subsequent to the reset event (these have special meanings to Optiboot as supplied by the core.
  * Optiboot exits using the WDT. Thus, a WDT reset causes it to jump to the app, on the assumption that either it just exited thebootloader and we sure don't want to run it again, or the app just did a WDT reset. A WDT reset from the app can be used to reset the app from software without making the bootloader run and wait for a timeout (if any).
  * See [the Optiboot reference](Ref_Optiboot.md) for information about available options for how to react to possible entry conditions.
* A BOR will clear all other reset flags except PORF, and set BORF.
* A WDRF or SWRF should never occur at the same time as another reset if the flags are being reset.
* If you are not either letting the core reset the flags and stash them in GPIOR0, make sure you do it right. I think my solution is about as small graceful as it gets.
  * Crashes that turn into dirty resets that aren't caught and handled, turn into hangs. Next thing you know, you're out on ladder at a customer site in a snowwstorm, with a clipboard to block the snow in one hand, a flashlight on your forehead, trying to open the cover and pry the battery out of contact for a few seconds (solar charging, yasee), in the hopes of regaining control of the StuffMizer-9000 you sold them.
  * Therefore, *either use the provided solution as is, or do it right, or fault intolerance is to be expected and sucks*
* UPDIRF may co-occur with other flags if if those other reset conditions occurred during UPDI mode, even when flags are being reset properly.

| WDRF | UPDIRF| SWRF | EXTRF | PORF | BORF | Meaning, Suggested response
|------|-------|------|-------|------|------|-------------
|    0 |     0 |   0  |    0  |    0 |    0 | (Unless you are reinventing this wheel) You are reading RSTFR, but the core already read that and cleared it; we leave the value in the 6 LSB of `GPIOR0`.
|    0 |     0 |   0  |    0  |    0 |    0 | (No wheel reinvention, `GPIOR0 & 0x3F == 0` at start of setup()) - Should be impossible without disabling reset cause clearing (see "Note"), otherwise create a new issue showing code that reproduces this issue; this should not occur and constitutes a serious bug.
|    0 |     0 |   0  |    0  |    0 |    0 | (Reinventing the reset cause clearing wheel, and found `RSTFR = 0` after apparent reset). Reset is dirty, hardware in unknown state, "guaranteed not to work". Fire software reset immediately.
|    X |     1 |   X  |    X  |    X |    X | The most recent reset was caused by exiting UPDI programming mode. All other reset flags should probably be ignored. Start per application requirements as a "hot start"
|    1 |     0 |   0  |    0  |    0 |    0 | Code was previously executing. WDT was enabled, and the timeout expired without wdr being executed (or executed wdr too soon in windowed mode). React per application requirements as a "hot start".
|    0 |     0 |   1  |    0  |    0 |    0 | Code was previously executing, until a software reset was issued. If not expected, likely resulted from dirty reset; investigate as such. If expected, react per application requirements as a "hot start".
|    1 |     0 |   0  |    1  |    0 |    0 | As above. While WDR triggered, reset pin asserted and released. Likely WDR is retriggering repeatedly, and the reset button fixed nothing. If powercycling then fixed, suspect "hot start" issue.
|    X |     0 |   1  |    X  |    1 |    X | Can't happen unless reset cause clearing has definitely been disabled. PORF and WDRF or SWRF can only be set if resets aren't clearing reset flags.
|    X |     0 |   1  |    X  |    0 |    1 | Can't happen unless reset cause clearing is broken badly - After the BOR, the other flag should have been cleared
|    1 |     0 |   X  |    X  |    1 |    X | Can't happen unless reset cause clearing has definitely been disabled. 2 entries above.
|    1 |     0 |   X  |    X  |    0 |    1 | Can't happen unless reset cause clearing is broken badly - After the BOR, the other flag should have been cleared
|    1 |     0 |   1  |    X  |    0 |    0 | Implausible but suspected possible with precise timing and luck  - but more likely reset cause clearing is broken badly. See "Note"
|    0 |     0 |   1  |    1  |    0 |    0 | Implausible but possible with precise timing - but more likely reset cause clearing is broken badly. See "Note"
|    0 |     0 |   0  |    X  |    X |    X | power on, reset pin, or brown out reset. If more than one is set, see the smaller table below.
|    X |     X |   X  |    X  |    1 |    X | If POR fired, Vdd must have been below V<sub>POR, fall</sub> or V<sub>PORR</sub> since the last time POR was triggered, then Vdd exceeded V<sub>POR, rise</sub> or V<sub>POR</sub>).

| PORF | EXTRF | BORF | Meaning, Suggested Response
|------|-------|------|----------
|    1 |     0 |    0 | Vdd ramped quickly, and BOD did not have a chance to engage and react (fast rising power) or BOR not enabled. This is a typical successful startup (or if problems at startup, perhapse you should be using BOD or longer SUT)
|    0 |     0 |    1 | Code was executing, Vdd fell below V<sub>BOR</sub>, triggering BOR. Vdd stayed above V<sub>POR, fall</sub> or V<sub>PORR</sub>, and later increased to exceed V<sub>BOR</sub>, and appropriate brownout recovery steps taken, if needed,
|    1 |     0 |    1 | Vdd ramp was slow. Likely while still in SUT, the BOD awoke, saw the still insufficeint voltage (Vdd < V<sub>BOR</sub> , and asserted a BOR, from which it revived when Vdd exceeded V<sub>BOR</sub>
|    1 |     1 |    X | The reset pin voltage rose significantly more slowly than VDD, and hence upon release of POR. EXTR remained engaged for some time longer. Excessive capacitive loading on reset? Insufficient pullup?

Note: These states indicate that you've done something to defang the reset cause detection and dirty reset detection. That is something one does not do by accident, so presumably you have something in mind. Uh, well, whatever it is, if it's supposed to be working, it's not, cause here we are with reset flags that cannot occur unless you're failing to reset the reset flags on startup. Anyway, until you fix that, neither bootloader nor application will be able to accurately determine reset cause and dirty resets will usually hang.
Possible responses (pick one):
a. Run in circles, scream, and shout.
b. (Not recommended, work) Correct your code to check and clear the reset cause at the earliest point in the boot process, and if none is reported handle the fact that you are in an unknown state, likely by firing a software reset. Like we do.
c. Come to your senses, accept that the wheel was invented long ago, with little fundamental improvement, and stick to current version of init_three_stuff(). Retrieve the reset cause from the 6 LSB of GPIOR0, if you care. Set GPIOR0 to 0 if you use GPIOR0 in your sketch. The core does not use it at any other time. None of the lines on the table which refer to this note should occur using stock init_three_stuff().

Where I say reset cause detection is "disabled", notice that PORF is set, implying that the reset flags were never cleared because you have overridden our routine with something of your own design, **which means that we (and you) cannot detect when a dirty reset has occurrd, and thus, we will have no way of knowing that the chip state is unknown and arbitrarily deranged**

When the combination of reset cause flags is inconsistent with both making no attempt to clear flags - PORF was set at poweron, and is no longer set - but has somehow failed to clear other flags, that is a sign that something is severely wrong with your flag checking routine (ours don't do that!)

In some cases, with unusually good timing, it is likely possible to trigger two reset causes close enough together that you end up with two flags (for example, code is spamming SWR repeatedly, you could hit reset at the right moment and maybe get both set at once)



`**` - Few plausible routes to this state exist except for a) wiring fault, intermittent power connection. b) power supply circuit defect or unforeseen behavior. c) Power supply circuit expected behavior due to, for example, a device with a battery being used until it stopped working due to the battery voltage sinking below V<sub>BOR</sub>; depending on the design, it is entirely plausible that the battery voltage will continue to be seen by the microcontroller - so then when it is plugged into a USB power cable to recharge, and the voltage rises above V<sub>BOR</sub>. This results in it starting up with BORF alone set. (This is a useful thing to check when the system gets into bad states, particularly ones not solved by a reset button press but solved by a power cycle - typically the result of an external device with different reset thresholds, which has not reset, or reset uncleanly (some devices, due to hysteresis in the reset mechanism, have a range of voltages in which they are above the nominal POR/BOR mechanism threshold, but not above the device's power on reset voltage.  The AVR's have a V<sub>PORR</sub>, the voltage t

`*` Possibly due to loading and insufficient pullup strength on the reset pin (You should immediately suspect that there is an issue with autoreset if you are using that - Perhaps a different (larger) value capacitor was used? Could the pullup be disconnected?). This has the visible consequence that the bootloader runs on startup. Even when a sketch is present and the reset button was never pressed.  While ideally it should not occur, it is also not indicative of the sort of "drop everything and find this bug" situation, whereas a starting with no flags set indicates a critical failure occurring, and ought never be tolerated, but rather investigated promptly.
**Also note that the diode to Vdd is mandatory, you must prevent Vrst from rising above the threshold voltage totrigger HV programming mode when it releases DTR**
While tinies do permit 2x VDD on RST, Dx and Ex now prefer around 2.5 vols above VDD, absolute maximum of 9 volts. That would mean that no, it would not be a good idea to use your hommeade 12v tinyAVR HV programmer with a DD=series.


### The power of two software resets
The fact that there are now two ways of resetting from software changes things. There is an obvious way that both features can be used (an application uses the WDT to detect that it has gotten hung up somewhere and reset itself, and issues a software reset when some set of conditions is met or per user command). This is likely the most common use of them outside of Arduino. But there is also a second way that these can be used, and this is how we use it by default - Use SWRF as a "reset and run bootloader" while you can trigger resets without SWRF by the standard method of setting the WDT for minimum time and then going into forever loop which will trigger the reset.

It is not recommended to set windowed mode and continualy send WDRs. You're doing something strange. When someone else reads it, or you read it later, you will likely have to come back and read either this document or the datasheet to understand how continually WDRs which normally would prevent the WDT from resetting, but for windowed mode. That would be okay - an ugly hack if it would result in eliminating the 8 ms wait - but it just replaces it with a slightly slower one - WDRs, like everything related to the WDT, have a 2-3 WDT clock sync delay. The WDT clock is around 1 kHz derived from the internal 32 kHz ULP. Once you write the settings to the WDT register, it will take 2-3ms. Then the timer starts up. Now you can execute WDR, and it's no longer a no-op. The first WDR goes in - but you can't queue up a second WDR - WDR will again be a no-op unil, 2-3 ms later, the first WDR finishes syncing; it will reach the WDT and reset the timer - only after this first WDR is the window used, and only after that WDR is synced will a WDR instruction do start syncing a WDR. 2-3 ms later, it will finally slam into the closed window. Now a total of 3 sync periods have happened, for a total of 6-9 ms If we had not done this and simply used the shortest timeout, we would reset in 10-11 ms (2-3 ms sync before it turns on, minimum setting of 8 WDT ticks as WDT period. We could save as little as 1ms or as much as 6ms.

No matter what, triggering a reset with the WDT hence takes far longer than other methods., but at 11 ms, still a very short period of tme by human standards.

## Determining reset cause
The cause of reset is recorded whenever any reset takes place. These flags are normally only cleared manually. Because of the importance of verifying that there is always a reset cause when the sketch restarts (to prevent dirty resets, as of 2.5.0, both optiboot and the normal sketch will check for blank flags, reset if it sees them, and otherwise, clear them. It will save the reset cause to the `GPIOR0`  and clear all of the flags. The 6 lower bits are used to record UPDI reset, Software Reset, Watchdog Reset, External Reset, Brownout Reset and Power-on Reset, in that order. Multiple can be set at once when multiple reset conditions occur. A BOD reset often happens as the power supply voltage is ramping up.

When you see unexpected resets, check the reset cause! If it's showing as software reset and nothing else, and you don't do a software reset, you've got a bug leading to a dirty reset. You can override init_reset_flags() to turn on LEDs or something like that to indicate how you got there - but you are forced to work under very painful conditions: Using Serial, any arduino timekeeping functions are out of the question, etc. The system is not so hosed that you can't turn on a pin, though, and you can generate delays with delay_us() and delay_ms() in `<util/delay.h>` (provided they are compile time known constant delays) and use that to blink a LED, thought the rate at which it is blinking may be very different than you intended). The best way to avoid this problem is to preempt it. Test often. If suddenly it starts throwing dirty resets, well, you know what you were working on before this started happening. Did you add some code to write to an array? Maybe you wrote past the end of it. Did you add an interrupt? Are you sure the vector is spelled right and you're enabling the right interrupt? (see below for more causes of a dirty reset). Dirty resets are BY FAR the most common cause of hangs and Arduinos getting into a "bad state"; the recent changes should make these far less likely to result in anything more than a surprise reset. Which is a very bad failure, but not as bad as the part hanging until the batteries die or you climb up the tree you mounted it in to press the reset button.

## The Reset Pin
When the reset pin is acting as reset - which unfortunately is uncommon on tinyAVRs because the pin can't be used for UPDI programming if set to act as reset (on the 20 or 24-pin tinyAVR 2-series, PB4 can be configured to act as reset instead, but that is not an option on other parts), it requires HV reprogramming to write to them over UPDI afterwards; that is beyond the scope of this particular document). When acting as reset , the pullup is always enabled. If the pin is ever brought to a logic LOW, it will reset the chip. This is pretty much the same behavior as on classic AVR's (contrary to some posts on forums, you do not *need* to have an external pullup on reset - though it is recommended when the pin is used for reset.) Just like on the classic AVRs, you should never jumper an output pin to reset and drive it low to reset. (We have two great ways to reset from software now!).


### AutoReset
Normal Arduino boards autoreset when the serial port is opened. It does this through the "DTR autoreset circuit". AVRdude, like any program that doesn't override the default behavior will set DTR and RTS lines low while the serial port is open. This circuit converts the transition into a pulse, allowing the chip to leave reset too. (your finer serial terminals let you control these two pins. Generally speaking DTR and RTS are interchangeable, and you use whichever one the serial adapter makes available more easily).

Obviously this design depends on having the pin set as reset. **Don't connect an autoreset circuit if you are programming through the same pin via UPDI**, as those components will prevent all UPDI programming. The same circuit can be used with the alternate reset pin on the larger 2-series parts, or on older models through "Ersatz reset" - otherwise known as a LOW interrupt on a pin, with the ISR consisting of a software reset call. Unlike normal reset pin resets, ersatz reset is not bulletproof (you can make it better by elevating the priority of the interrupt, and by setting it up early).
It will reset the chip when serial connection is opened like on typical Arduino boards. When using a bootloader, this is very convenient and allows for "normal" uploads without manually resetting it. When no bootloader is in use, it is still often useful to ensure that you see all the output from the start of the sketch.

On the other hand, it is sometimes preferable to have one adapter connected to the serial port continually with a tool outside Arduino (I recommend never using the pitiful excuse for a serial monitor they give us in the IDE. On Windows, "hterm" is my preferred tool) . Occasionally (for example, if you have it connected to a computer controlling a long running task, but the computer has turned itself off and gone into power save mode) autoreset is your enemy. Reestablishing a connection would trigger autoreset in that case, while if it were not present, that isn't an issue. So you don't *always* want autoreset.

The internal pullup on Reset is always enabled as long as the pin hasn't been turned into a normal input. This internal pullup, like all the others, is fairly weak. Adding a 10k resistor to Vcc, even if not using the full autoreset circuit, can provide improved noise rejection, though it is not required. If you aren't using autoreset at all, but noise on the reset pin is expected to or has been found to be a problem, combine the resistor with a 0.1uF cap between Reset and Ground, which will not interfere with a reset button, but will prevent plausible sources of electrical noise from causing and unwanted reset. A capacitor between reset and ground will defeat autoreset (though in order to be guaranteed to be effective, it must be at least 4 times the value of the autoreset capacitor to be certain of preventing a reset, though one as small as 1/9th the value of the autoreset capacitor could potentially interfere (the classical method of using a capacitor of 10 uF to disable autoreset on an Uno with a 0.1uF autoreset cap greatly exceeds what is necessary, and was clearly designed to work in spite of numerous mistakes being made). You can of course also just use a jumper or solder bridge tro enable anddisable autoreset, like my breakout boards do. With 3 pins instead of 2, arranged such that jumping one pair enables autoreset, and the other ties the nominally-dtr side of the cap to ground would provide a  way to select either noise rejection or autoreset.

A representative schematic is shown on the left below.

* 1 Small signal diode (specifics aren't important, as long as it's approximately a standard or schottky diode some big honkin thing meant to take amps. Schottky is or silicon diodes are fine here)
* 1 0.1uF Ceramic Capacitor
* 1 10k Resistor

If you aren't using autoreset, but the pin is still reset, it is suggested to connect that capacitor and resistor as shown on the right.


![Autoreset circuit and non-autoreset circuit](ResetCircuits.png "Reset Circuit Examples")


If the reset pin is still causing spurious external resets with that sort of countermeasure, after verifying that the resets are actually external resets, rather than some other kind, you should question whether the reset line is somehow unintentionally connected to a data line or similarly mis-wired. Overcoming a 10k resistor and 0.1uF capacitor takes an unusually large amounts of EMI, and is not plausible except in the most extreme circumstances. Whatever the reason, it likely points to a design flaw somewhere. However, unlike classic AVR parts you can disable reset without causing any problems reprogramming these parts

## Triggering a reset from software
These parts support a native software reset operation; on classic AVRs, the only way was to enable the watchdog timer, and then wait for it to time out. With the modern AVR devices, there are two ways to trigger a reset from software: A watchdog timer reset (as before), and the native software reset. Unlike classic AVRs, after a WDT reset, the watchdog timer is not forced on with the minimum timeout (on classic devices, this is why doing a WDT restart with very old bootloaders instead hung the board - the bootloader wasn't smart enough to turn it off before it was reset by it).

These two methods of resetting the chip allow you a crude means of signaling to the application or bootloader what sort of condition triggered the reset.

Additionally, while the bootloader, if used (see below) will run after a software reset, it will NOT run after a watchdog reset (well - it will run, but only long enough to read the reset flag register and see that it was restarted by the WDT: That means that either the bootloader just ran, finished, and reset the device (If we didn't jump to the app in this case, we'd just sit in the bootloader doing WDT resets forever), that the application suffered from a WDT timeout due to a bug or adverse conditions (that's not the bootloader's business to get involved in) or that the application intentionally triggered a WDT reset. None of those are "entry conditions" for the bootloader, so it just stashes the reset flags, clears them, and jumps to the app).This allows the application a convenient mechanism to restart itself without having to potentially wait through the bootloader attempting to communicate with whatever is connected to the serial port.

Note: While the windowed mode would at first seem to suggest that you could trigger a WDT reset much faster by setting it and then executing `WDR` until it resets from missing the window, you don't gain nearly as much as you'd think. First, the initial WDR needs to be synchronized - 2-3 WDT clocks, ie, 2-3 ms. Additional WDRs executed while this is happening are ignored. Only when the second WDR makes it to the watchdog timer domain will it reset the system. So the overall time to restart is 6-9ms. Instead 10-11 ms (sync delay + minimum timeout).

```c++
void resetViaWDT() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}

```c++
void resetViaWDTFaster() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_WINDOW_8CLK_gc | WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout, minimum window.
  while (1) __asm__ __volatile__ ("wdr"::);
  // execute WDRs until reset. The loop should in total take 3 clocks (the compiler will implement it as wdr, rjmp .-4)
  // but because of the sync delay described above, it will run thousands of times before the first premature (from the
  // WDT's perspective) wdr finally makes it to the WDT domain and slams into the closed window.
}

void resetViaSWR() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}
```

The two recommended methods, resetViaWDT() and resetViaSWR() are provided by DxCore.h or megaTinycore.h as appropriate.

## Using watchdog to reset when hung
If you only worked with the watchdog timer as an Arduino user - you might not even know why it's called that, or what the original concept was, and just know it as that trick to do a software reset on classic AVRs, and as a way to generate periodic interrupts (which it can't do on a modern AVR - though the much more powerful and full features RTC can be used instead). The "purpose" of a watchdog timer is to detect when the part has become hung - either because it's wound up in an infinite loop due to a bug, or because it wound up in a bad state due to a glitch on the power supply or other adverse hardware event, has been left without a clock by an external clock source failing, went to sleep waiting for some event which doesn't end up happening (or without correctly enabling whatever is supposed to wake it up) - and issue a reset. It is often anthropomorphized as a dog, who needs to be "fed" or "pet" periodically, or else he will "bite" (commonly seen in comments - the latter generally only when we are expecting it, as in `while (1); //wait for the watchdog to bite`).

You would first initialize the WDT like:
```c++
_PROTECTED_WRITE(WDT.CTRLA, settings); //enable the WDT
```

To configure the WDT to reset the device after a period of time, replace `settings` above with the desired WDT timeout period from this table. If is getting stuck somewhere that causes it to repeatedly reset the WDT or you are concerned that it might, you can configure it in window mode to reset if an attempt is made to reset the watchdog timer within the specified period. To do this, bitwise OR the two, eg: `_PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8KCLK_gc | WDT_WINDOW_16CLK_gc );` would set the WDT to reset the device if two attempts to reset the watchdog were ever made within 16 milliseconds of the previous one (before the "window" opens), or if no reset was performed in the 8 seconds after that (when the window closes). If no window period is included, there will be no delay before the window opens after the last wdr instruction. Note that in all cases, multiple wdr instructions fired in very rapid succession (2-3 ms) will be ignored: A wdr does nothing if the AVR is in the process of syncing a WDR already. The window mode only starts opening and closing the window after the first wdr reaches the timer after being configured.
A configuration with a window setting but no period has behavior that is not defined. This should not be used.


| Timeout | WDT period name      | WDT Window name      |
|---------|----------------------|----------------------|
|  0.008s | WDT_PERIOD_8CLK_gc   | WDT_WINDOW_8CLK_gc   |
|  0.016s | WDT_PERIOD_16CLK_gc  | WDT_WINDOW_16CLK_gc  |
|  0.032s | WDT_PERIOD_32CLK_gc  | WDT_WINDOW_32CLK_gc  |
|  0.064s | WDT_PERIOD_64CLK_gc  | WDT_WINDOW_64CLK_gc  |
|  0.128s | WDT_PERIOD_128CLK_gc | WDT_WINDOW_128CLK_gc |
|  0.256s | WDT_PERIOD_256CLK_gc | WDT_WINDOW_256CLK_gc |
|  0.512s | WDT_PERIOD_512CLK_gc | WDT_WINDOW_512CLK_gc |
|  1.024s | WDT_PERIOD_1KCLK_gc  | WDT_WINDOW_1KCLK_gc  |
|  2.048s | WDT_PERIOD_2KCLK_gc  | WDT_WINDOW_2KCLK_gc  |
|  4.096s | WDT_PERIOD_4KCLK_gc  | WDT_WINDOW_4KCLK_gc  |
|  8.192s | WDT_PERIOD_8KCLK_gc  | WDT_WINDOW_8KCLK_gc  |

### Resetting the WDT
A typical use of this is to have the main loop (generally loop() in an Arduino sketch) reset the watchdog at the start or end of each loop, so when a function it calls ends up hung, we can use:

```c
// As a function
void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}
```

Or

```c++
// as a macro (which is all that wdt.h does)
#define wdt_reset() __asm__ __volatile__ ("wdr"::)
```

### Disabling WDT
In some cases you may only want the WDT enabled when certain routines prone to hanging due to external conditions, and then turn it off again.
```c++
_PROTECTED_WRITE(WDT.CTRLA,0); //Yeah, that's it.
```

At the other extreme you may want it to be impossible for code, even the cleverest bugs, to turn it off. You can lock the WDT in it's current configuration by writing the WDT_LOCK bit in WDT.STATUS to 1 - only a system reset will unset the bit.

```c++
_PROTECTED_WRITE(WDT.STATUS, WDT_LOCK_bm); // call after setting WDT to desired configuration.
```
For even more protection (and more nuisance in keeping the WDT from biting at all times). you can set the WDTCFG fuse via UPDI programming, using the included tools menu.
### Summary and mini-example
So overall, if you wanted your sketch to reset if you ever spent longer than 8 seconds between loop() iterations and also detect when a WDT reset just occurred and take special actions in setup you might do this
```c++
void wdt_enable() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8KCLK_gc); // no window, 8 seconds
}

void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}

void wdt_disable() {
  _PROTECTED_WRITE(WDT.CTRLA,0);
}

/* If you at some point plan to put the chip to sleep you need to turn off the WDT or it will reset you out of sleep... */
void goToSleep() {
  wdt_disable();
  sleep_cpu();
  wdt_enable();  // turn the WDT back on promptly when we awaken.
}

void NotifyUser() {
 Serial.println("The system reset via WDT after hang...");
}

void loop() {
  wdt_reset(); // reset watchdog.
  /* <snip - rest of loop goes here. make sure it doesn't
}

void setup() {
  wdt_enable(); //we're super paranoid, so we turn on WDT immediately.
  Serial.begin(MYBAUDRATE); // we use this down in NotifyUser().
  uint8_t resetflags = GPIOR0; // Optiboot or the core stashes the reset flags here before clearing them to honor entry conditions
  // GPIOR0 = 0; // no need to clear because this is reset at startup to 0 either way, and then gets the reset flags stashed in it - unless you need it later..
  if (resetflags == RSTCTRL_WDRF_bm) { //means it was a WDT reset. Notice the use of == - we only want to detect it when WDRF is the only reset flag; On Optiboot configurations, you will see WDRF along with another bit set if optiboot considered that to be an entry condition.
    NotifyUser();
  }
  <snip - rest of your setup goes here>
}

```


## The WRONG way to reset from software
I have seen people throw around `asm volatile("jmp 0");` as a solution to a need to reset from software. **Don't do that** - all compiled C code makes assumptions about the state from which it is run. Jumping to 0 from a running application violates several of them unless you take care to avoid those pitfalls (if I were to add a comment after that line, it would read something like `// Reset chip uncleanly to produce unpredictable results`. Resetting with a jump to 0 was always risky business and should never be done on any part, ever (certainly not without taking a bunch of precautions and knowing exactly what you're getting into (which nobody who did this ever seemed to). Now that we have a right way to do software reset, there is absolutely no excuse for intentionally triggering a dirty reset like this.

## The danger of "dirty" resets
Now that intended resets have been covered, we need to confront the other kind of resets: unintended ones that happen unexpectedly, those aforementioned dirty resets. Both Optiboot and the init() function (which runs before setup to make the chip run at the speed you asked, start millis, and so on) assume that everything starts in it's reset configuration - a great deal more code would otherwise be needed. So does the vast majority of user code. Making the assumption that the sketch starts with everything in the reset configuration a very helpful simplifying assumption, which is safe to make as long as the application is functioning correctly. It is not true under a variety of error conditions, though, and after Bad Things happen, it's possible for the the program counter to reach 0x0000 again, as if it were reset, but for no hardware reset to have occurred. The registers could contain take any value (but will generally be whatever they were set to immediately before the Bad Thing finally ran far enough off the rails that it wound up at 0x0000. This may leave them at the value they had before the reset - or the Bad Thing may have scribbled over one or more registers. The consequences could range from unnoticeable to catastrophic, but usually tend towards the latter. Depending on the nature of the incident and the code that runs after a "reset", very different things could end up happening. You may notice only an unexpected reset, or it may hang until manually reset, or get into some strange tight bootlooop. This core preventes the worst outcomes by firing a software reset (which is why we need to stash the flags in a different register) to restore the chip to it's startup state.

Most of us have seen an an AVR get into a "bad state" somehow, where it's hung despite having a clock signal. Many times people reset and forget about it. If you are trying to make a more robust product, suitable for use in production, or one which would be a serious inconvenience were if it failed suddenly (temporarily or permanently), be sure you've taken all the precautions against this. Most of the time, with the new automatic check and clear of reset flags, *this won't happen* It will reset cleanly. However, this just changes a hang or more serious broken behavior into an unexpected reset. Robust code should be on the lookout for suspicious resets.

Example 1: Reading out the reset cause on serial it to print a message about the reset cause.
```c++
void setup() {
  Serial.begin(115200);
  if (reset_flags & RSTCTRL_UPDIRF_bm) {
    Serial.println("Reset by UPDI (code just upoloaded now)");
  }
  if (reset_flags & RSTCTRL_WDRF_bm) {
    Serial.println("reset by WDT timeout");
  }
  if (reset_flags & RSTCTRL_SWRF_bm) {
    Serial.println("reset at request of user code.");
  }
  if (reset_flags & RSTCTRL_EXTRF_bm) {
    Serial.println("Reset because reset pin brought low");
  }
  if (reset_flags & RSTCTRL_BORF_bm) {
    Serial.println("Reset by voltage brownout");
  }
  if (reset_flags & RSTCTRL_PORF_bm) {
    Serial.println("Reset by power on");
  }
}
```
Example 2: Override and indicate that it's a recovery from dirty reset by turning on an LED during the WDT timeout - this can be used to aid in debugging when you don't understand why it is resetting. You can check additional things in this case, like CPUINT.STATUS (low two bits in particular - these indicate if it thinks an interrupt is running at this point. If they are not 0, you're firing an interrupt that doesn't exist (and you're also not reading the warnings!)
```c++
  uint8_t reset_flags;
  void init_reset_flags() { //override the stock init_reset_flags() - this runs very early, before anything else is initialized. You can write pins here, but that's about it.
    reset_flags = RSTCTRL.RSTFR;   // Read flags. Declare variable as local or global as needed.
    if (reset_flags == 0){                    // if no flags, reset wasn't clean - reset cleanly.
      _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_2KCLK_gc); // Set the watchdog to bite in 2 seconds.
      pinModeFast(LED_BUILTIN, OUTPUT);       // This turns LED on if it's active low - but it's unlikely to be.
      digitalWriteFast(LED_BUITIN, HIGH);     // The above line is key because it sets the direction to output. Now we can write it high, and get the output we want. Comment out if LED is active LOW
      if (CPUINT.STATUS != 0) {               // We can also turn on a second LED if the cause was either a bad interrupt, or a bug in an ISR
        //pinModeFast(LED_BUILTIN2, OUTPUT);  // most boards don't have a second LED - connect a LED to another pin and replace LED_BUILTIN2 with that pin
        //digitalWriteFast(LED_BUITIN2, HIGH);//
      } else {
        // pinModeFast(LED_BUILTIN2, INPUT);   // What why? Because you have no idea what the state of the hardware is if there's a dirty reset!
        // It could have been high for some other reason, and since no reset occurred, it still would be. This way, you know that if both LEDs are on...
        // that was a dirty reset resulting from an interrupt without a defined handler executing, or a very bad bug in an ISR.
      }
      while(1);                               // wait for timeout
    }
    RSTCTRL.RSTFR = reset_flags;              // Clear the flags by writing the value back to it.
  }
```
### Causes of dirty resets
So how can this happen?

#### Power supply or clock issues
Rarely, hardware problems can cause a dirty reset. Are the decoupling caps in place? Does it coincide with a change in load that might cause the power rail to depart from it's nominal voltage? If you are using an external clock, are you sure that it is working properly? If you're overclocking, try using a speed that is within spec, you may just be overclocking too hard for that specimen.

#### Interrupt enabled while ISR not defined
Amazingly, it's not an error to do `ISR(tcb0_vecter) {...}` `*` (instead of `ISR(TCB0_INT_vect) {...}`) - it gets a *warning* for misspelled vector, but not an error (one of the reasons we force warnings on is because of things like *THAT* which are only warnings, not errors). Enabling a interrupt without an ISR defined at all and letting it trigger will achieve the same thing: BAD_ISR handler is called. The default implementation is to jump to 0. No flags are cleared - but neither is a reti fired - The default `BAD_ISR` implementation just jumps to 0... And since these parts track whether the code is supposed to be in an interrupt (see CPUINT.STATUS LVLnEX bit), you end up outside of an interrupt, with the hardware still thinking your're in one and only elevated priority interrupts can fire. Don't enable an interrupt without the ISR defined, and the misspelled vector warnings virtually always indicate that this is going to happen.

#### Interrupt enabled while ISR does not exist
Amazingly, it's not an error to do `ISR(tcb0_vecter) {...}` (instead of `ISR(TCB0_INT_vect) {...}`) - it gets a *warning* for misspelled vector, but not an error (one of the reasons we force warnings on is because of things like *THAT* which are only warnings, not errors). Enabling a interrupt without an ISR defined at all and letting it trigger will achieve the same thing: BAD_ISR handler is called. The default implementation is to jump to 0. No flags are cleared - but neither is a reti fired - The default `BAD_ISR` implementation just jumps to 0... And since these parts track whether the code is supposed to be in an interrupt (see CPUINT.STATUS LVLnEX bit), you end up outside of an interrupt, with the hardware still thinking your're in one and only elevated priority interrupts can fire. Don't enable an interrupt without the ISR defined, and the misspelled vector warnings virtually always indicate that this is going to happen.

#### Smashing the stack
Local variables that exceed what will fit in the working registers get created on the stack. The stack is also where the return address of a function call is stored. If we meant to be accessing a local array that's on the stack, but wrote off the end of it, we would likely overwrite the return address. When we later executed a "return" we would end up at an address based on that instead of where we were prior to the call. This sort of stack corruption bug is often called "smashing the stack". So we're jumping off into a nonsense location... 0 is probably statistically more likely than most values to get written over the return address - this directly lands you at reset without a reset - but more frequently this will point to empty space, and execution will skid along the 0xFFFF's of empty flash (this is an invalid instruction, but is treated as a skip-if-bit-in-register-set instruction, 0xFFF7, so either every one, or every other one will be processed, depending on the state of that working register - specifically, 0xFFFF looks at the high bit of the last register, r31 - the high byte of the Z pointer), which continues at 1 word per cycle because skips take an extra cycle if skipping, until reaching the end and wrapping around. At that point, if the skip-if's aren't skipping, you'll land on the reset vector. Otherwise, if even words are being skipped, you land on the second half of the reset vector and execute that mess as an instruction. Unless it happens to have the same numeric value as an rjmp or rcall (rare, but possible depending on how much your have stored in PROGMEM, or even less likely, looks like the start of a 32-bit jump or call) then hit the NMI interrupt vector, which you almost certainly don't use; the BAD_ISR implementation jumps to 0 and there's your dirty reset.

Note that you don't always have empty flash after your sketch - specifically, if using the included bootloader, you won't if the previous program was bigger. That code should remain unreachable, except in this kind of situation. The results are likely to be bizarre, confusing, and obviously incorrect, but very hard to rationalize.

#### Writing to the SP (stack pointer)
If you you don't *really* know what you're doing, let the compiler manage the stack pointer. Any mistake is likely to result in a dirty reset. The stack pointer gets written by compiler generated code at startup to make it point to where it should (even though the hardware guarantees that it will be set there to begin with - assuming there wasn't a "dirty reset"), and is modified directly by the push/pop and by call/ret type instructions (including the variants of call). Because it holds function return addresses, corruption here leads to dirty resets when the function finally returns. Your code should not, under normal circumstances, be reading it, much less writing it - it has little utility except in debugging of ram related issues (eg, finding out how big the stack is at a given point in execution if you suspect you're suffering a stack-heap collision not long after that).

#### A stack-heap collision
Where the local variables on the stack crash into the ones in the heap or ones allocated with malloc (ie, allocation of ram at runtime). This will lead to both getting corrupted, likely quickly resulting in a a bogus value at the top of the stack (located now somewhere in the heap) being returned to. This can also corrupt a function pointer leading to situation below.

#### Bad inline assembly
If you're writing assembly you should know this.... but be certain that you use the same number of push and pop instructions, and be careful to give the right constraints so the compiler can't assume that a value your changing is constant. It's very easy to forget that, say, that pointer you're passing in and reading or writing with postincrement (or predecrement) - that is not read only, and cannot be given as an input opperand - you're changing it! It should be a read-write output opperand. When you lie to the compiler and say you're not, unpredictable bad things happen. **These specific bugs only show up in assembly, not C**

#### Bad function pointers
If a function pointer doesn't point to a function, when it gets called, Bad Things happen - it can act as a jump to any other part of the program. A null pointer, or a function pointer to a location after the end of program will take the path described above under "Smashing the Stack" to reach the reset vector. This is just another way of that the program counter can end up pointing somewhere outside of the program. Like that case, if it doesn't get there directly and lands at some incorrect place within the program, it will follow an incorrect execution path, likely eventually ending up with a dirty reset when it eventually 'returns' to either 0 or a location after the end of the code.

#### Bad pointers in general
Speaking of bad pointers... A null pointer is universally understood to be an error condition on most platforms. A desktop computer doesn't have anything located at address zero. That's not the case on an AVR - Writing to or reading from 0x0000 is perfectly valid - it's the pin direction control for PORTA, via the VPORT registers. Addresses up to 0x0B are valid on tinyAVR, and everything up to 0x1B may be valid on AVR Dx and AVR Ex-series (depending on pincount), and addresses 0x1C-1F are the GPIORs and are always valid. Note that the high I/O space is not used on these parts with only a few exceptions, and those addresses ignore all writes and read as 0, but 0x34 is the CCP register (used for timed writes), and 0x3B-0x3F in are Important Registers - they contain: RAMPZ (on large-flash parts like the Dx-series) the configuration change protection register, the stack pointer (2 bytes) and the status register. None of which you ought to access with pointers.

When it comes to writing functions that will take a pointer as an argument, particularly if meant for production use, you can often weed out a lot of bogus values just by knowing what your function is built to do. If it takes a pointer to a location in RAM, well, the location should be between RAMSTART and RAMEND, right? Peripherals of a given type are located consecutively in memory, so if you're taking a pointer to a type B timer, it should be not lower than the address of TCB0, nor higher than the address of the highest number TCB on that part. Unless VPORTs are what you expect, anything below 0x3F is clearly wrong, and so on. Function pointers to locations beyond the end of flash obviously are not right. Nor are function pointers to locations  lower than `_VECTORS_SIZE` (or `_VECTORS_SIZE` plus the size of the bootloader section) are also wrong; you don't call vectors directly (indeed, it's not clear what `reti` does when called outside of an interrupt; though I wager it is indistinguishable from a normal `ret`). Remember that function pointers are word addressed, not byte addressed. If your function is passed such arguments, it's an error condition, which you don't have a way to gracefully inform the caller of necessarily, since there's no support for exceptions, but you have a choice about whether to detect these and what you do if you encounter them. Handling them with more care costs flash and execution speed, and depending on what sort of use the function and larger application is intended for, and how it will be getting the values it passes, may not be worthwhile - but that is a decision that you should make with intent, not by default when you don't consider that you or someone else working on the project may pass garbage to the function and blindly use pointers.

While it may not be of paramount importance for typical Arduino applications, if you're looking into production software, this is much more of a concern, and you don't have to turn a bad call, which already will by definition lead to the absence of correct behavior, into the presence of incorrect behavior by following the pointer. Maybe the only thing that makes sense is to fire a software reset and hope it doesn't happen again, or maybe it makes sense to do nothing and return. Both of those are usually safer than shitting on a random peripheral or memory address and continuing execution as if nothing happened or reading data you know is wrong and treating it as valid, and is most definitely safer than following a clearly bogus function pointer to god-knows-where, in the expectation of it eventually hitting 0x0000 and thus our dirty reset trap, and letting that reset it - it may or may not (I believe this is called the "halting problem"), and if it does, who knows what it will do on it's way there - it will be careening wildly around the program space like a ball in a pinball machine before it leaves play through the dirty reset trap, or finds it's way back to loop after corrupting assorted registers and memory locations.

As it is (usually) particularly hard to recognize a bad function pointer, the more important it is for the system to not go off the deep end (and the more trees you might have to climb and customer sites you might have to visit to revive the system if it does), the more suspicion function pointers and to a lesser extent, all pointers should be viewed with. You're thinking of taking a pointer to a peripheral? What about taking the number of that peripheral instead (ex, 1, instead of &TCB1) and creating the pointer yourself (ex, TCB_t * tcbptr = (&TCB0 + tcbnumber). We do provide #defines for every peripheral type that may exist in quantities greater than 1 listing the number of them present (See [Constants Reference](Ref_Defines.md)) to make it easy to test whether you should expect the peripheral number passed to exist.

#### Clock Abuse
Sufficient noise (like if you had a large 5-foot-something broad-band antenna connected to the crystal - even the case of the crystal, and even through a fairly poor electrical connection ("You mean like a person touching the crystal case with dry hands?" "Well, I suppose that would count yes....") - particularly under adverse conditions, such as with inappropriate loading caps), it can trigger a reset or hang due to clock failure. In the case when the crystal starts but then starts behaving badly due to change in the surroundings - someone poking it, for example, you might hope that this should trigger the clock failure detection on parts that have that - but that only detects the case when the clock *stops*. In this case, what has happened is that the clock is seeing noise as clock pulses, so the CFD thinks it's fine - but the signal has been so thoroughly deranged as to cause instructions to execute incorrectly - this is the same thing that happens when the clock speed is abruptly changed or a signal that does not meet the requirements specified in the electrical properties section is used as the clock). Crystals that do not provide a method to tie the case to the ground are more vulnerable to this, as are ones that do but which aren't correctly connected to ground. I have seen part footprints used that use a 4-terminal crystal, but connect only the two that aren't grounded to the part. Don't do that.

Overclocking too hard can also do it (If it is overclocked enough that arithmetic and logical instructions get incorrect results, this can corrupt the stack pointer when functions are called and variables pushed and popped). A good test is to set up the part under conditions mimicking the most adverse ones you are designing for (highest operating temperature, lowest Vdd), and have the sketch print millis() and micros() every second. Open the serial console, and walk away for an hour or two, and then scroll through the giant serial log. Has it reset at some point? Do you ever see micros() values not 1000 times the millis value? If yes, the part is not stable at that speed, and you should back off or try your luck with another chip, hoping to get one that runs.

## Example of how some common dirty resets break stuff, and how this solution makes them less harmful

We have in the megaTinyCore or DxCore "library" an example where you can comment out the reset section of init_reset_flags(), and one of three causes of a dirty reset (jump to 0, array overrun, and bad ISR), and see how that dirty reset has measurable impacts - registers don't get reset! But if you don't comment out the reset, like magic, they all trigger resets.

Try it out if you want to see for yourself how things can go wrong.
