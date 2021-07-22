
#include <USERSIG.h>
#define watchdogReset()  __asm__ __volatile__ ("wdr\n");
#include "util/delay.h"

#if !defined(MILLIS_USE_TIMERNONE)
  #warning "millis needs to be disabled while tuning - preventing millis initialization, Sketch does not depend on it if it was not modified."
  void init_millis() {
    ;
  }
#endif
#define SERIAL_DEBUG
#ifdef PIN_PA5  // Unless it lacks that pin, use PA5 (Arduino pin 1).
  #define TUNING_PIN PIN_PA5   // Tune with PA5 on these parts
  #define FORCE_PIN PIN_PA6    // If this pin is pulled low within 30 seconds of power on,
                               // it will recalibrate even if the chip is alreadyt tuned

#else // on 8-pin parts, we will use PA7 the default serial RX pin, also pin 1. PA6 would be the TX line and could conflict if the board had a bootloader.
  #define TUNING_PIN PIN_PA7
  #define FORCE_PIN PIN_PA6
#endif

// #define ONEKHZMODE             // uncomment if using a 1kHz source instead of 500 Hz

#define DONE_PIN LED_BUILTIN
#define PROG_PIN LED_BUILTIN


uint8_t stockCal;
#if MEGATINYCORE_SERIES == 2
  const uint8_t homeindex        = 1;
  #if F_CPU == 16000000
    const uint16_t speeds[]      = {12000, 16000, 20000, 24000, 25000, 30000, 65535};
    const uint16_t PulseLens[]   = { 6000,  8000, 10000, 12000, 12500, 15000, 65535};
    uint8_t CalBytes[]           = { 0x80,  0x80,  0x80,  0x80,  0x80,  0x80,  0x80};
    const uint8_t names[]        = {   12,    16,    20,    24,    25,    30,  0xFF};
    #define CAL_START (USER_SIGNATURES_SIZE-12)
  #elif F_CPU==20000000
    const uint16_t speeds[]      = {16000, 20000, 24000, 25000, 30000, 32000, 65535};
    const uint16_t PulseLens[]   = { 6400,  8000,  9600, 10000, 12000, 12800, 65535};
    uint8_t CalBytes[]           = { 0x80,  0x80,  0x80,  0x80,  0x80,  0x80,  0x80};
    const uint8_t names[]        = {   16,    20,    24,    25,    30,    32,  0xFF};
    #define CAL_START (USER_SIGNATURES_SIZE-6)
  #endif
#else  //0/1 series
  const uint8_t homeindex        = 2;
  #if F_CPU == 16000000
    const uint16_t speeds[]      = {10000, 12000, 16000, 20000, 24000, 25000, 65535};
    const uint16_t PulseLens[]   = { 5000,  6000,  8000, 10000, 12000, 12500, 65535};
    uint8_t CalBytes[]           = { 0x80,  0x80,  0x80,  0x80,  0x80,  0x80,  0x80};
    const uint8_t names[]        = {   10,    12,    16,    20,    24,    25,  0xFF};
    #define CAL_START (USER_SIGNATURES_SIZE-12)
  #elif F_CPU==20000000
    const uint16_t speeds[]      = {12000, 16000, 20000, 24000, 25000, 30000, 65535};
    const uint16_t PulseLens[] = { 4800,  6400,  8000,  9600, 10000, 12000, 65535};

    uint8_t CalBytes[]           = { 0x80,  0x80,  0x80,  0x80,  0x80,  0x80,  0x80};
    const uint8_t names[]        = {   12,    16,    20,    24,    25,    30,  0xFF};
    #define CAL_START (USER_SIGNATURES_SIZE-6)
  #endif
#endif
uint8_t TuningDone = 0;

void ensureCleanReset() {
  uint8_t rstfr = RSTCTRL.RSTFR;
  if (rstfr == 0) {
    rstfr = GPIOR0 & (~RSTCTRL_WDRF_bm);
  }
  if (!(rstfr & 0x30)) { //if neither a software or or UPDI reset, it may be unclean from excessive overclocking
    _PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
  } else {
    if (rstfr & 0x20) { //UPDI reset - that implies we just uploaded this and that we should retune the oscillator.
       // You don't upload a tuning sketch if you don't intend to tune it.
       TuningDone = 255;
    } else if (rstfr != 0x10) { // if it's not just a reset flag, let's clean it for good measure
      RSTCTRL.RSTFR = RSTCTRL.RSTFR;
      GPIOR0 = 0;
      _PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
    }
  }
  // If we made it through there, we started cleanly and can stop spamming reset.
  RSTCTRL.RSTFR = RSTCTRL.RSTFR;
  GPIOR0 = 0;
}
void setup() {
  ensureCleanReset();
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
  #endif
  #if (F_CPU==20000000L)
    stockCal = (*(uint8_t *)(0x1100 + 26));
  #elif  (F_CPU==16000000L)
    stockCal = (*(uint8_t *)(0x1100 + 24));
  #else
    #error "This sketch must be compiled for 16 MHz or 20 MHz only (and should be run once at each speed)"
  #endif
  #ifdef FORCE_PIN
    pinMode(FORCE_PIN, INPUT_PULLUP);
  #endif
  if ((USERSIG.read(CAL_START + homeindex) != 0xFF) && TuningDone != 255) {
    TuningDone = 1;
    tidyTuningValues();
  }
  #ifdef PROG_PIN
    pinMode(PROG_PIN,OUTPUT);
  #endif

  _delay_ms(10);
  //See if we already wrote some tuning constants - the one at our home speed is enough to cancel tuning
  // except when tuning_done=255 meaning it was just uploaded
  #ifdef FORCE_PIN
    uint16_t countdown = 3000;
    uint16_t lowcount  = 0;
    while(countdown--) {
      if (digitalRead(FORCE_PIN) == LOW) lowcount++;
      _delay_ms(1);
    }
    if (lowcount > 500) {
      Serial.println("tuning forced");
      TuningDone = 0;
    }
  #endif
  if (TuningDone == 1) {
    #ifdef SERIAL_DEBUG
        Serial.println("already tuned");
        for (byte i = 0; i < 6; i++) {
          if (i) Serial.print(':');
          Serial.printHex(USERSIG.read(CAL_START+i));
        }
      Serial.println();
      Serial.flush();
    #endif
    // If we're tuned we will pretend to be blink :-)
    TotallyJustBlink();
  }
  if (TuningDone == 255) {
    #ifdef SERIAL_DEBUG
        Serial.println("tuning with new sketch");
        for (byte i = 0; i < 6; i++) {
          if (i) Serial.print(':');
          Serial.printHex(USERSIG.read(CAL_START+i));
        }
      Serial.println();
      Serial.flush();
    #endif
  }
}

void TotallyJustBlink() {
  #ifdef VPORTA
    VPORTA.DIR=0;
  #endif
  #ifdef VPORTB
    VPORTB.DIR=0;
  #endif
  #ifdef VPORTC
    VPORTC.DIR=0;
  #endif
  pinMode(LED_BUILTIN,OUTPUT);
  while (1) {
    digitalWrite(LED_BUILTIN,HIGH);
    _delay_ms(1000);
    digitalWrite(LED_BUILTIN,LOW);
    _delay_ms(1000);
  }
}


void tidyTuningValues() {
  for (uint8_t j = 0; j < 6; j++) {
    if (USERSIG.read(CAL_START + j) == 0xFF) {
      if (j < homeindex) {
        // must be too low Mark as such
        USERSIG.write(CAL_START + j, 0x80);
      } else {
        USERSIG.write(CAL_START + j, 0xC0); //indicating we didn't even make it there.
      }
    }
  }
}

void loop() {
  #ifdef SERIAL_DEBUG
    Serial.println("tuning");
    Serial.flush();
  #endif
  _PROTECTED_WRITE(WDT_CTRLA, 0x0B);
  uint16_t lastpulselen = 1;
  uint8_t currentTarget = 0;
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); //prescale enabled, div by 4
  #if MEGATINYCORE_SERIES==2
    for (byte x = 0; x < 128; x++)  // 128 possible settings for 2-series
  #else
    for (byte x = 0; x < 64; x++)   //  64 for 0/1-series
  #endif
  {
    watchdogReset();
    _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA, x); /* Switch to new clock - prescale of 4 already on */
    _NOP();
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 2);   //prescale disabled, div by 4
    _delay_ms(1);
    digitalWriteFast(PROG_PIN,HIGH);
    byte done = 0;
    byte i;
    uint32_t pulselen;
      while (done == 0) {
        pulselen = 0;
        i = 0;
        #if defined(ONEKHZMODE)
          while (i < 16)
        #else
          while (i < 8)
        #endif
        {
          uint16_t temp=pulseIn(PIN_PA5, LOW, (uint32_t) 100000UL);
          #if defined(ONEKHZMODE)
            if (temp < 200 || temp > 1000)
          #else
            if (temp < 400 || temp > 2000)
          #endif
          {
            i = 100; //set to impossible value to reach normally.
          }
          pulselen += temp;
           i++;
        }
        if (i == 8 || i == 16){
          done = 1;
        }

      }
/*      Hey hey, wait a minute, pulseIn()?
 *      Do we actually trust that function's accuracy? On all parts?
 *      It's a block of inscrutable compiler generated assembly supplied as a .S file!
 *
 *  Actually, yes we do, because it's not inscrutable. I scruted that little bastard in 2020, which is
 *  why it no longer counts ticks 5/16ths faster than it should before the pulse starts. That was broken
 *  everywhere, on all parts. When a different problem with it was reported on ATTinyCore, it took me
 *  days to realize that it didn't work any better on an Uno (see, my test case was no pulse, just checking
 *  the timeout... you know, keep it simple). Best part was def. finding the optimization the compiler
 *  missed and using it to  get back exactly the space the fix cost....
 *
 *  Compiler generated assembly is the pits, and if this example looks like it's in a deeper pit than
 *  usual, I agree. I think it's the ancient version of avr-gcc they had to use to get it to implement
 *  the tight loops as... tight loops instead of something weird and worse else. avr-gcc has been doing
 *  one-step-forward-one-step-back for years now. It'd be nice if Microchip would spend some of the money
 *  we're giving them to pay someone to clean up all the cruft that has built up in the compiler....
 *  But I think they'd much prefer people use their own closed source compiler. Only a madman would be
 *  comfortable using a compiler where the source was secret.
 *  We could use input capture, sure, and it would look all sexy, and people would be like "oooh shiny"
 *  but this would have little impact on tuning accuracy. Maybe it would be more practical to do it
 *  faster, but... not my priority!
 *  if you disagree with this design decision, well, I'd be hasppy to merge something better!
 *  I think it is totally unnecessarey and have too many other bugs to deal with. -Spence 6/2021
 */
    digitalWriteFast(PROG_PIN,LOW);
    // First pass, we need to figure out what the first constant we write is.
    if (x == 0) {
      while (pulselen > PulseLens[currentTarget] && (pulselen - PulseLens[currentTarget]) > 150) {
        currentTarget++; //oscillator is too fast, can't hit that speed.

      }
    }
    uint8_t writeme = 0x80;
    if (pulselen >= PulseLens[currentTarget]) {
      if (lastpulselen < PulseLens[currentTarget]) {
        // this is the setting for one of the targeted frequencies!
        int16_t diffLastTarg = PulseLens[currentTarget] - lastpulselen;
        int16_t diffCurrTarg = pulselen - PulseLens[currentTarget];
        if (diffLastTarg > diffCurrTarg) {
          CalBytes[currentTarget] = x;
          writeme = x;
        } else {
          CalBytes[currentTarget] = x - 1;
          writeme = x - 1;
        }

      } else {
        //We got this one already!
        currentTarget++;
      }
#if MEGATINYCORE_SERIES == 2
    } else if (x == 0x7F && currentTarget < 6) {
#else
    } else if (x == 0x3F && currentTarget < 6) {
#endif
      // this is the last chance and we don't have values for all targets!
      // Are we so close tat we can get away with using it?
      uint16_t underoneandhalfpercent=PulseLens[currentTarget];
      underoneandhalfpercent >>= 6; //divide by 64, for around 1.6% of targe
      if (PulseLens[currentTarget] - pulselen < underoneandhalfpercent) {
        writeme=x;
      }
    }

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); //prescale enabled, div by 4: Slow back down so we can safely increment the speed next pass through loop.
    if (writeme != 0x80 && TuningDone != 1) {

      USERSIG.write(CAL_START + currentTarget, writeme);
    }
    lastpulselen = pulselen; //Today is just tomorrow's yesterday.
    _delay_ms(5);
  }
  //End of the loop, we've done them all!
  watchdogReset();
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); //prescale enabled, div by 4
  _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA, stockCal);
  _NOP(); _NOP();
  //now for ones we still haven't hit, mark as 0x80 ("tuning found unreachable")
  if (currentTarget <= 5 && USERSIG.read(CAL_START + currentTarget) == 0xFF) {
    while (currentTarget <= 5) {
      if (USERSIG.read(CAL_START + currentTarget) == 0xFF) {
        USERSIG.write(CAL_START + currentTarget, 0x80);
      }
      currentTarget++;
    }
  }
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 2); //prescale disabled, div by 4
  _PROTECTED_WRITE(WDT_CTRLA, 0x00);
  _delay_ms(20);
  #ifdef SERIAL_DEBUG
    for (byte i = 0; i < 6; i++) {
      if (i) Serial.print(':');
      Serial.printHex(USERSIG.read(CAL_START+i));
    }
    Serial.flush();
  #endif
  _PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
}
