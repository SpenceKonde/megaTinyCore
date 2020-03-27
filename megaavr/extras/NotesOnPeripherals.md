### Notes on peripherals, advanced
This document lays out some general notes on the peripherals for advanced users - particularly those who are used to how things work on classic AVR parts.

### Interrupts and Interupt Flags

##### Disabled interrupts don't set flags
Unlike classic AVRs, if an interrupt is not enabled, the interrupt flag will not be set. IE, on a classic AVR, you could do something like this as a delay (it's how optiboot times the triple-blink):
```
TCCR1B=0; TCCR1A=0; TIMSK1=0; TIFR1=255; // stop timer, WGM=0, no PWM, mask all ints, clear all flags.
TCNT1=-(F_CPU/1024); // timer clocks at 1024 prescaler in 1 second, as negative sets count that far away from overflow
TCCR1B=0x05; //turn on Timwer 1 with prescaler=1024
while(!(TIFR1&(1<<TOV1))); //spin for 1 second
```
On megaavr, trying the same thing will spin forever, because the INTFLAGS register are never set if it's not enabled... 
```
  TCA0.SINGLE.CTRLA=0x0F; //TCA0 1024 prescaler
  TCB0.CTRLA=0; TCB0.CTRLB=0; TCB0.INTCTRL=0; TCB0.CNT=0; //Stop TCB0, Periodic Interrupt timing mode, interrupt off, Count 0
  TCB0.CCMP=(F_CPU/1024); //Number of clocks until overflow
  TCB0.CTRLA=0x05; //TCB0 uses prescaled CLK_TCA
  while(!TCB0.INTFLAGS); //Spins forever!
```
Of course, if you enable the interrupt, but have interrupts globally disabled, that trick works fine:

```
  TCA0.SINGLE.CTRLA=0x0F; //TCA0 1024 prescaler
  TCB0.CTRLA=0; TCB0.CTRLB=0; TCB0.INTCTRL=1; TCB0.CNT=0; //Stop TCB0, Periodic Interrupt timing mode, interrupt on, Count 0
  TCB0.CCMP=(F_CPU/1024); //Number of clocks until overflow
  TCB0.CTRLA=0x05; //TCB0 uses prescaled CLK_TCA
  cli();
  while(!TCB0.INTFLAGS); //Spins forever!
  TCB0.INTFLAGS=1; //You really need to do this here too!
  sei();
```

If you don't clear the INTFLAGS when doing that (even if you clear INTCTRL instead), that interrupt will still fire... so:

##### Don't let your code try to call an ISR that doesn't exist

If you don't have an ISR defined, it will go to the "bad interrupt" handler, which jumps to the reset vector... but the INTFLAG is still set, so the non-existent interrupt will still fire... again and again and again.



##### Flags MUST be cleared in interrupts
* Unlike classic AVRs, you MUST clear the interrupt flag in the ISR. They are not cleared automatically. Do this by writing 1 to the bit. Failing to do so can produce surprising results, because the processor doesn't *halt* - it just runs agonizingly slowly (plus whatever the interrupt does keeps happening - but this may not be as obvious) - because at least one instruction will always happen between interrupts. 

Here's a sketch that demonstrates this, and another interesting thing; the loop waiting for the ISR is exited as soon as it fires the first time... but it keeps running continually, letting the while loop after it run one instruction for each time the ISR runs. Both of those unsigned ints cause the ISR and while() loop to toggle the pins more slowly, so you can see how fast the two are running relative to eachother with even really crude means.

```
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

##### Don't rely on variables overflowing...
You may be wondering about the nop instruction there. 
That's because there's another wacky thing demonstrated there: The C standard - surprisingly - doesn't guarantee that it won't optimize away things that can only happen if an unsigned integer overflows. It usually doesn't - but it *can*. And here, apparently taking out that noop causes it to do so - in this case, PA6 flips every time the now very tight (3 instructions) loop runs, instead of every 65436'th time! Obviously making test2 volatile prevents it from optimizing that out, and then you can remove the nop... Interestingly enough, removing the first while loop, and making not-volatile doesn't result in the same surprising optimization in the ISR - it still runs at the same speed... This is a reminder that you shouldn't rely on the behavior of unsigned, non-volatile variables overflowing. 

As it happens, I'd read that about the C language just days ago - quite a coincidence.....


