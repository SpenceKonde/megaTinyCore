# Startup order and callback-like-functions
These are not callbacks. Callbacks are not efficient in this context. However, weakly defined functions like these achieve the same effect, and do so efficiently. To do so you override them - typically you would override one that isn't an empty function with an empty function to disable that initialization step, or you would override ones that default to an empty function with code that needs to run at the appointed time.

A callback would be where you supply a function pointer which we then call (which would end up as an icall instruction). Doing such a thing is much more costly, because the compiler can't assume it is constant. Hence call-used registers (even if the function doesn't use them) get saved and restored, function call overhead cannot be avoided, and the compiler can't optimize them. We try to avoid these. They're only used when the function to be called can actually change at runtime, mainly when "attaching" interrupts.

**This document reflects the 2.5.12 release of megaTinyCore**
Some of these were added in 2.4.x (actually, quite a few of them were, which was the impetus for writing this)

## Startup Sequence
When execution reaches the application after reset or bootloader, it hits the reset vector first, and then jumps where ver that is pointing and begins execution with the .init sections
```text
Things most reasonable to override are marked with a + and the start of the line.
Things that might make sense to override only with empty functions are marked with - at the start of the line.
Things that might make sense call if main or init is overwritten are marked with # at the start of line
Sections defined by AVR-GCC are marked with S at start of the line.
Things that you will likely break stuff without benefit by overriding are marked with X at start of line

S .init2: This initializes the stack and clears r1 (the known zero) that avr-gcc needs. No C code before here works correctly!
S .init3: This calls the megaTinyCore function _initThreeStuff()
  _initThreeStuff() is a creatively named internal early initialization routine; it cannot be overridden. but the functions it calls can be.
+ init_reset_flags() - checks reset flags, and if none are set, fires software reset. You may wish to override this in order to assiist debugging
+  onPreMain() - can be used to provide code that needs to run before class constructors.
S .init4: For devices with > 64 KB of ROM, .init4 defines the code which takes care of copying the contents of .data from the flash to SRAM.
S .init6: Used for constructors in C++ programs.
S .init9: Jumps to main()
  main()
+   onBeforeInit() - This is called first, before any initialization.
-   init() - to initialize the peripherals so the core-provided functions work.
#   init_clock() - This is called first, to initialize the system clock. You oughtn't override this, but if you override main you almost certainly want to call this
#   init_ADC0() - This is called to initialize the ADC
#   //init_ADC1() - on the 1-series w/16 or 32k flash, aka "Golden 1-series" series parts with a second ADC, this function exists but it NOT called. See Ref_Analog.md.
    init_timers() - This function calls the timer initialization functions
-    init_TCA0() - Initialized TCA0 in split mode for PWM output. If you don't want PWM or need to use PWM in 16-bit mode or need buffering, AND are truly desperate for more flash, you can override with an empty function.
                    Don't do that if you are using TCA0 for millis, though.
-     init_TCD0() - If present and not used for millis (1-series)
X   init_millis() - This is called to kick off millis() timekeeping. This will configure TCD0 or any TCB if it is used for millis as well.
                    TCD0 require a nontrivial amount of additional code here and elsewhere when used for millis().
X initVariant() - A rare few libraries define this for stuff they need to run before setup.
+ onAfterInit() - returns a uint8_t, normally 0. If it returns anything else, we will not enable interrupts, with all the hazards that entails (see below for some specifics)
  setup() - Finally the normal setup() is called
  loop() - and the normal loop(), called continually.
```

If you for some reason end up directly putting functions into init 5, 7, or 8 (best steer clear of the ones that are taken) be warned:
The function must be declared with both `((naked))` and `((used))` attributes! The latter is required or the optimizer will just eliminate it. The former is needed to prevent it from generating a return instruction at the end. This causes a crash when it tries to return and there's nothing on the call stack to return to.

## Recommended override candidates
These are the functions which are there with the intent that users may have good reason to override them.

### init_reset_flags
This is used only when not building for an optiboot board (if you are building for optiboot, this same functionality is performed by the bootloader, as it's needed to ensure that the bootloader won't try to run on a chip in an unknown state. The bootloader prime directive is that no code, no matter how awful, should be able to break the bootloader and prevent the upload of corrected code. )
```c
void init_reset_flags() __attribute__((weak)) {
  uint8_t flags = RSTCTRL.RSTFR;
  RSTCTRL.RSTFR = flags;
  if (flags == 0) {
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  }
  GPIOR0 = flags;
}
```
This can be used to try to pick apart when you are getting unexpected resets (likely "dirty resets"), by doing something else when RSTCTRL.RSTFR == 0 - note that because it is run so early in the initialization process, and we're here because we called an ISR that doesn't exist or smashed the stack, you can count on very little working correctly; your best bet is probably to test theories and give output by writing I/O pins to, for example, turn on an LED. (Since you have no timekeeping yet, and are likely debugging a showstopping bug, you probably just want to write the LED pin with pinModeFast and digitalWriteFast, and then hang with while(1). Having multiple LEDs connected here really helps: turn on one LED for any dirty reset, and a second LED for the theory you're testing). You might, for example, want to determine if the dirty reset is coming about through a bad ISR: if `CPUINT.STATUS & (CPUINT_LVL0EX_bm | CPUINT_LVL1EX_bm)` that means it it thinks it's in an interrupt, but it's running non-interrupt initialization code, so one of two things happened: either a non-existent ISR was called, or a function called by an ISR`*`,  wasn't inlined and smashed the stack (the dirty reset can occur after smashing the stack only when a `ret` or `reti` is executed, so if you smash the stack within the ISR proper, the reti will still execute, RSTCTRL.STATUS will be cleared, and *then* you'll end up in a dirty reset). See the [Reset Reference](Ref_Reset.md) for more information.


### onPreMain
```c
void onPreMain() __attribute__((weak)) {
}
```
This is the recommended way to run code before the class constructors. Runs after init_reset_flags, so it won't run on a dirty reset if reset flags are being cleared as they should be.


### onAfterInit
```c++

uint8_t onAfterInit() __attribute__((weak)) {
  return 0;
}

```
onAfterInit runs just before setup is called, but after all other initialization. returning a non-zero value will prevent interrupts from being enabled. Good for debugging if you suspect that an interrupt is firing and wrecking everything as soon as they're enabled (for example, an interrupt which you cannot identify is being enabled by a class constructor in a poorly written library, such that the flag is set before setup() is called - and there's something horribly wrong about that interrupt (say, maybe it doesn't exist causing a dirty reset, or loops forever because it does `while(SOME_REGISTER = 1);` (instead of `== 1`)). In cases like this, it can be hard to get information out ("Who here knows what a hardware debugger is?" *most hands go up* "Who here has seen a hardware debugger for an AVR in person?" *most hands go down* "Who here has used one...." *all the hands remain up* "... for hardware debugging, not just programming" *all the hands go down except for that one smart guy in every class* )


### onBeforeInit
```c++
void onBeforeInit() __attribute__((weak));{;}
```
onBeforeInit is called as the first code in main, useful if you need to squeeze in some code between class constructors and the rest of the internal initialization. Rarely useful - where it is, it's probably a shim for a poorly written library whose class constructor makes a mess of something that then causes an even bigger problem when init() is called, while you wait for a fix from the author - in this situation, if you don't want to modify your copy of the library, but you have to do something to extinguish whatever small dumpster fire was started by the class constructor before the init() routine dumps a bucket of gasoline on it, this is how you would do it. Please report any library that requires this to me so that I can A) apply additional pressure to the library maintainer, and B) warn people in the library compatibility list.

## Clock Failure Handling
The AVR DB, DD, and likely future parts that support an external crystal, have Clock Failure Detection (CFD) and there are callbacks to go with it. The tinyAVRs do not, and hence have no such callbacks. You're not missing much though - the CFD doesn't work very well (at least on the DB-series), because the misfiring clock often causes malfunctioning of the CPU such that the ISR does not operate correctly either.

If using an external clock source, and it stops, so does the chip. There are two possible results if the external clock disappears:
1. If the WDT is enabled, it will reset the chip. init_clock() will then try to use the non-functional source again, presumably failing, and leave the default /6 prescaler enabled while running from the internal oscillator. This will also happen if set to use an external clock that is not functioning at startup. If you're trying to use an external clock, but you instead appear to be running at 2.66 or 3.33 MHz, this is what is happening.
2. If the WDT is not enabled, it will just hang when deprived of a clock source.

## Things you probably shouldn't override
The functions listed below can be overridden. In most cases they should not be. They are listed for completeness; many of them were added for use during development of the core to make core development easier. They may be useful for debugging and on rare occasions in

### main
```c
int main() {
  onBeforeInit(); //Emnpty callback called before init but after the .init stuff. First normal code executed - no peripherals have been set up yet, but class constructors have been run. Useful if you need to fiddle with a class instance at this point in time,
  init(); //Interrupts are turned on just prior to init() returning.
  initVariant();
  if (!onAfterInit()) sei();  //enable interrupts.
  setup();
  for (;;) {
    loop();
    #ifdef ENABLE_SERIAL_EVENT /* this is never true unless core is modified
      This option is strongly deprecated and Serial Event's days i nthe core are numbered. Users of serialEvent should begin migration with a sense of urgency*/
      if (serialEventRun) serialEventRun();
    #endif
  }
}
```
This is the main program, and it calls all the initialization functions described herein except for initThreeStuff(). The only things that run first are the things in the .initN sections - this means init_reset_flags(), onPreMain(), and class constructors. It can be overridden but in this case nothing will be initialized, and the clock will be 4 MHz internal when it is called. If you have a different speed selected, everything that depends on F_CPU (including the avrlibc delay.h) will have all timing wrong. Even if you override main, you probably want to call init_clock() at the start or be sure to compile for 4 MHz. If you just don't want to use the Arduino setup/loop structure, but you do want everything else (millis, pwm, adc, and anything that a library needs to do in initVariant), simply put your code in setup and leave loop empty - don't override main.

### Initializers of peripherals
Overriding these is not recommended. They're most likely to become relevant if you have overridden init() or main(), yet still want the peripheral in question initialized (which of course raises the question of why you overrode init() or main() in the first place). Occasionally useful for debugging, and that's why they exist.
```c
void init()             __attribute__((weak)); // This calls all of the others.
void init_clock()       __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()        __attribute__((weak)); // this is called to initialize ADC0.
//void init_ADC1()      __attribute__((weak)); // this is not called automatically, but can be called manually on parts with an ADC1 to initialize that just as we do ADC0.
void init_timers();                            // this function calls the timer initialization functions. Overriding is not permitted.
void init_TCA0()        __attribute__((weak)); // called by init_timers() - Don't override this if using TCA0 for millis.
void init_TCD0()        __attribute__((weak)); // called by init_timers() - Does nothing if TCD0 is used as millis timer.
void init_millis()      __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.
```
They are called in the order shown above.

Any of them can be overridden, but overriding them with anything other than an empty function is not recommended and is rarely a good idea except for debugging. If the peripheral isn't being used and you're desperate for flash (maybe on a small flash DD or EA series parts) these will save a tiny amount of flash. Don't override with anything other than an empty function. If you're initializing them differently, do it in setup().

The timer initialization functions do different things if the timer is used for millis.

#### init_clock
Initializes the system clock so that it will run at the F_CPU passed to it. Don't override this unless you can ensure that the F_CPU that is passed via the compiler command line will be correct or as s desperate measure in debugging. Overriding it with an empty function may be useful when debugging exotic problems where you want to make sure that a problem isn't being caused by the clock configuration code. (and accept that the timekeeping will be wrong while debugging)

*if overriding main() or init() you probably want to call this directly at initialization, otherwise no timing or baud rate calculation will work correctly - this is what sets the chip to run at the clock speed you're compiling for*

Changing this is the wrong way to debug a problem that you think might be caused by a malfunctioning external clock. In that case, just build for internal clock.

#### init_ADC0
Initializes the ADC clock prescaler to get a legal frequency, sets up defaults and enables the ADC. It can be overridden with an empty function to prevent it from initializing the ADC to save flash if the ADC is not used. if main is overridden and tou want the right clock speed, you MUST init_clock MUST be called first in that case.

#### init_ADC1
As above for ADC1 on parts that have it (1614, 1616, 1617. 3216, 3217 - smaller size and 0-series parts don't have it, and 2=series parts have an ADC0 that is far the ones on the 0/1-series). Must be called manually. See the [Analog Reference](Ref_Analog.md)

#### init_timers
Calls initTCA0(), and then initTCD0() (if the part has one). There is no general initialization of TCBs as they are not used for e generic purpose. This cannot be overridden - override the function for the timer you're doing something different to, if required for your application. If all the timer initialization functions are overridden with empty functions then this will be optimized away.

#### init_TCA0
Initialize the type A timer for PWM. The one for a timer used as millis must not be overridden. It is not recommended to override these at all except with an empty function in order to leave the peripheral in reset state (but takeOverTCA0() will also put it back in it's reset state - and if you don't `takeOverTCA0` then it will try to turn off PWM when you call digitalWrite() - and not necessarily on the pin you called it on! That is *why* there is a `takeOverTCA0()` function; If you don't want to use analogWrite() through the timer, instead call takeOverTCA0() - which you need to do even if these are overridden if you don't want analogWrite() and digitalWrite() to manipulate the timer. As the tinyAVRs only have one TCA, there's no `init_TCA1()` and the PORTMUX configuration is handled in this function. If not using PWM, overriding this saves 22 bytes of flash. The only case when you can safely override this with an empty function without calling takeOkverTCA0() is if neither your code nor any library makes any call to digitalWrite() or analogWrite(). The case of disabling all PWM functionality on one or both types of timer to save flash will be addressed in 2.6.x.

#### init_TCBs does not exist in megaTinyCore
On DxCore, this function provides support for using Type B timers as really lame PWM timers (see [Timer Reference](Ref_Timers.md)), although we do not recommend using those timers for that purpose.

As PWM from TCBs is not provided through the Arduino API functions on megaTinyCore there would be no need for this function. That means that the only initialization of of TCBs is performed by tone, Servo/Servo_megaTinyCore, your own sketch code, or by third party libraries. If used for millis, it is handled through `init_millis()` (due to the limits as described in the Timer Reference and more obtusely in the datasheet, on neither core do we attempt to support PWM on the millis timer - while it can be done (indeed, the official Nano Every support it), it uses considerable flash (which we can't spare on a tiny) while degrading micros performance as well as timing accuracy.

#### init_TCD0
Initializes the type D timer. It is not recommended to override this except with an empty function in order to leave the peripheral in it's reset state. This is particularly useful with the type D timer, which has no "hard reset" command like the TCA does, and it's got the enable-locked fields and the ENRDY bit - If you're going to take it over anyway, you'll have an easier time if you override this - you don't have to put your initialization code there (though you could), simply overriding it with an empty function will give you a clean start . As with the others, it is recommended to override with just an empty function in that case. Overriding with empty function saves 34b of flash.

#### init_millis
Initializes and kicks off millis timekeeping. If millis is handled by a type B or D timer, it also performs all initialization of that timer. **Overriding this (with an empty function) is for debugging ONLY** and should never be done except as a way of leaving in place millis, micros (they will always return 0) and delay (it will hang forever, so you need to use `_delay_ms() instead` for delays) if you need to isolate the impact of the millis interrupt running.

*If you just want to turn off millis, set the millis timer to "disabled"*. That provides a working delay() implementation, allows libraries to detect that millis is disabled, and realizes far more flash savings than just overriding init_millis() with an empty function.

### initVariant
```c++
void initVariant() __attribute__((weak)){;}
```
This is the ONLY one of these functions that is standard (other than init, setup, and loop, of course).

initVariant is meant for variant files to call, but none of them ever do that on any core i've seen, and library authors use this sometimes for code that needs to run at startup. **NO SKETCH SHOULD EVER OVERRIDE THIS** - This hook is reserved for library and variant authors. It is part of the Arduino API and is present on all cores as far as I know.
