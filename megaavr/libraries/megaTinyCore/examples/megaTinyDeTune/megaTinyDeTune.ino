/* This tool will clear the tuning values stored with megaTinyTuner */

#include <USERSIG.h>
#define watchdogReset()  __asm__ __volatile__ ("wdr\n");
#include "util/delay.h"
// *INDENT-OFF* astyle check wants this to be formatted in an unreadable and useless manner.
#warning "While this sketch is running, the clock will be at 2.66 or 3.33 MHz, which is always within operating conditions. "
// NO MILLIS!
void init_millis() {
  ;
}
// NO CLOCK CONFIG!
void init_clock() {
  ;
}

void ensureCleanReset() {

  uint8_t rstfr = RSTCTRL.RSTFR;
  if (rstfr == 0) {
    _PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
  } else if ((rstfr & 0x21) || )
    _PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
  } else {
    if (rstfr & 0x20) { // UPDI reset - that implies we just uploaded this and that we should retune the oscillator.
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

#if defined(SERIAL_DEBUG)
  #define BAUDHACK (115200*6)
  #define DBGSERIAL Serial
#endif

#define CAL_START (USER_SIGNATURES_SIZE-12)

void setup() {
  uint8_t tuned = 0
  watchdogReset();
  ensureCleanReset();
  #ifdef SERIAL_DEBUG
    Serial.begin(BAUDHACK);
    delay_ms(1); //6 ms
    Serial.println("Chip Sig:");
  #endif
    for (byte i = 0; i < 64; i++) {
      #ifdef SERIAL_DEBUG
        if (i) Serial.print(':');
        Serial.printHex(SIGROW[i]);
      #endif
    }

    #ifdef SERIAL_DEBUG
      Serial.println("Old cal:");
    #endif
    for (byte i = 0; i < 12; i++) {
      if (USERSIG.read(CAL_START + i)) tuned = 1;
      if (i) Serial.print(':');
      Serial.printHex(USERSIG.read(CAL_START + i));
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
          Serial.printHex(USERSIG.read(CAL_START + i));
        }
      Serial.println();
      Serial.flush();
    #endif
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
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); // prescale enabled, div by 4
  #if MEGATINYCORE_SERIES==2
    for (byte x = 0; x < 128; x++)  // 128 possible settings for 2-series
  #else
    for (byte x = 0; x < 64; x++)   //  64 for 0/1-series
  #endif
  {
    #if defined(SerialDebug)
       Serial.print((GPIOR1<<8)+GPIOR0)
    #endif
    watchdogReset();
    _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA, x); /* Switch to new clock - prescale of 4 already on */
    _NOP();
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 2);   // prescale disabled, div by 4
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
          uint16_t temp = pulseIn(TUNING_PIN, LOW, (uint32_t) 100000UL);
          #if defined(ONEKHZMODE)
            if (temp < 200 || temp > 1000)
          #else
            if (temp < 400 || temp > 2000)
          #endif
          {
            i = 100; // set to impossible value to reach normally.
            #if defined(SERIAL_DEBUG)
              volatile uint16_t* t = (volatile uint16_t*)(uint16_t)&GPIOR0;
              *t = temp;
            #endif
          }
          pulselen += temp;
           i++;
        }
        if (i == 8 || i == 16) {
          done = 1;
        }
      }
      #if defined(SERIAL_DEBUG)
        uint8_t curcal = x;
        _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA, stockCal);
        _NOP();
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 2);// prescale disabled.
        _NOP();
        // All that to log serial
        Serial.printHex(x);
        Serial.print(",");
        Serial.printHex(pulselen);
        Serial.flush();
        _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA, curcal);
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); // prescale enabled, div by 4: Slow back down so we can safely increment the speed next pass through loop.
      #endif
/*      Hey hey, wait a minute, pulseIn()?
 *      Do we actually trust that function's accuracy? On all parts?
 *      It's a block of inscrutable compiler generated assembly supplied as a .S file!
 *
 *  Actually, yes we do, because it's not inscrutable. I scruted that little bastard in 2020, which is
 *  why it no longer counts ticks 5/16ths faster than it should before the pulse starts. That was broken
 *  everywhere, on all parts. When a different problem with it was reported on ATTinyCore, it took me
 *  days to realize that it didn't work any better on an Uno (see, my test case was no pulse, just checking
 *  the timeout... you know, keep it simple). Best part was, after fixing it, finding the optimization the compiler
 *  missed and using it to get the exact same amount of flash back that was used for slowing the loop down before the pulse starts.
 *
 *  Compiler generated assembly is the pits, and if this example looks like it's in a deeper pit than
 *  usual, I agree. I think it's the ancient version of avr-gcc they had to use to get it to implement
 *  the tight loops as... tight loops instead of something weird and worse. avr-gcc has been doing
 *  one-step-forward-one-step-back for years now. It'd be nice if Microchip would spend some of the money
 *  we're giving them to pay someone to clean up all the cruft that has built up in the compiler....
 *  But I think they'd much prefer people use their own closed source compiler. IMO, Only a madman would be
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
        currentTarget++; // oscillator is too fast, can't hit that speed.

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
        // We got this one already!
        currentTarget++;
      }
#if MEGATINYCORE_SERIES == 2
    } else if (x == 0x7F && currentTarget < 6) {
#else
    } else if (x == 0x3F && currentTarget < 6) {
#endif
      // this is the last chance and we don't have values for all targets!
      // Are we so close tat we can get away with using it?
      uint16_t underoneandhalfpercent = PulseLens[currentTarget];
      underoneandhalfpercent >>= 6; // divide by 64, for around 1.6% of targe
      if (PulseLens[currentTarget] - pulselen < underoneandhalfpercent) {
        writeme = x;
      }
    }

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); // prescale enabled, div by 4: Slow back down so we can safely increment the speed next pass through loop.

    if (writeme != 0x80 && TuningDone != 1) {
      USERSIG.write(CAL_START + currentTarget, writeme);
    }
    lastpulselen = pulselen; // Today is just tomorrow's yesterday.
    _delay_ms(5);
  }
  // End of the loop, we've done them all!
  watchdogReset();
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 3); // prescale enabled, div by 4
  _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA, stockCal);
  _NOP(); _NOP();
  // now for ones we still haven't hit, mark as 0x80 ("tuning found unreachable")
  if (currentTarget <= 5 && USERSIG.read(CAL_START + currentTarget) == 0xFF) {
    while (currentTarget <= 5) {
      if (USERSIG.read(CAL_START + currentTarget) == 0xFF) {
        USERSIG.write(CAL_START + currentTarget, 0x80);
      }
      currentTarget++;
    }
  }
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 2); // prescale disabled, div by 4
  _PROTECTED_WRITE(WDT_CTRLA, 0x00);
  _delay_ms(20);
  #ifdef SERIAL_DEBUG
    for (byte i = 0; i < 6; i++) {
      if (i) Serial.print(':');
      Serial.printHex(USERSIG.read(CAL_START + i));
    }
    Serial.flush();
  #endif
  while(1);
  //_PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
}
