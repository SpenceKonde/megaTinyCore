# Notes on peripherals and other things
This document lays out some general notes on the peripherals - some general, some advanced, and some particularly aimed at people who are used to classic AVRs...

## Interrupts

### Don't let your code try to call an ISR that doesn't exist

If you don't have an ISR defined, it will go to the "bad interrupt" handler, which jumps to the reset vector... This puts your sketch into a guaranteed broken state: The INTFLAG is still set, and the interrupt is still enabled - but interrupts won't be firing because CPUINT thinks you're already in an interrupt because the `reti` instruction (RETurn from Interrupt) was never executed... The result is generally completely broken sketch behavior. So don't 

### Flags MUST be cleared in interrupts
* Unlike classic AVRs, you must clear the interrupt flag in the ISR unless specified otherwise in the datasheet (in some cases, there is a specific condition that will result in the flag being cleared, and in a few, you can't control the  Do this by writing 1 to the bit. Failing to do so can produce surprising results, because the processor doesn't *halt* - it just runs agonizingly slowly (plus whatever the interrupt does keeps happening - but this may not be as obvious) - because at least one instruction will always happen between interrupts.

Here's a sketch that demonstrates this, and another interesting thing; the loop waiting for the ISR is exited as soon as it fires the first time... but it keeps running continually, letting the while loop after it run one instruction for each time the ISR runs. Both of those unsigned ints cause the ISR and while() loop to toggle the pins more slowly, so you can see how fast the two are running relative to each other with even really crude means. 

```c++
volatile unsigned int test=0;
unsigned int test2=0;
void setup() {
  PORTA.DIR=0xC0;
  TCA0.SINGLE.CTRLA=0x0F; //TCA0 1024 prescaler, Running
  TCB0.CTRLA=0; TCB0.CTRLB=0; TCB0.INTCTRL=1; //Stop TCB0, Periodic Interrupt timing mode, interrupt on
  TCB0.CNT=0;
  TCB0.CCMP=F_CPU/1024; // Set TOP to number of clocks until overflow
  TCB0.CTRLA=0x05;
  while(!test);
  while(1){
    __asm__ __volatile__("nop");
    if (!test2++) PORTA.OUTTGL=0x40;
  }
}
void loop () {}
ISR(TCB0_INT_vect){
  if (!test++) PORTA.OUTTGL=0x80;
  //TCB0.INTFLAGS=1; //Should be here!
}

```

As it happens the "test2" PA7 transitions 10 times for each time PA6 does - and sure enough in the assembly listing (sketch -> export compiled binary with this core exports it to sketch folder!), there are 10 instructions. **And the ISR takes a whopping 50 instructions!** (Lots of POPs and the RETI at the end; loading a byte from RAM into registers is also 3 cycles, and storing it back is 2 more. So it is surprisingly slow. Just like you'd expect (20,000,000/(2^16*51)), it transitionas once per 167mS or so.

### Watch out for the optimizer...
You may be wondering about the nop instruction there.
That's because there's another wacky thing demonstrated there: The compiler can get awfully clever when optimizing - as in, too clever. Without that, it will perform loop unrolling - replacing the loop with with all the values test2 will hold:

This:
```c++
while(1){
  if (!test2++) PORTA.OUTTGL=0x40;
}
```
Becomes:
```c++
if (!0) PORTA.OUTTGL=0x40;
if (!1) PORTA.OUTTGL=0x40;
if (!2) PORTA.OUTTGL=0x40;
...
if (!65534) PORTA.OUTTGL=0x40;
if (!65535) PORTA.OUTTGL=0x40;
```
And the constants where the test for the if() evaluates to false (ie, 65535 of the 65536 cases) can be eliminated - they do nothing right?
Leaving:
```c++
while(1){
  PORTA.OUTTGL=0x40;
}
```

Which is why one normally writes busy-wait and cycle-counting stuff using inline assembly.
