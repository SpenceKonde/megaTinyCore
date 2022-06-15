/* This sketch assumes that, at the default calibration and the operating conditions in question,
 * the internal oscillator is perfectly tuned. It then uses input capture to time 127 cycless of the PIT/64 event
 * generator. It records that as the baseline, and then steps through all possible calibration values
 * repeating that measurement. The value will be proportional to the system clock, and allows us to calculate
 * approximate speed, and hence find the calibration values for key speeds.
 *
 */
/* INDENT-OFF */
#include <USERSIG.h>
#define watchdogReset()  __asm__ __volatile__ ("wdr\n");
#include "util/delay.h"
#define HAS_EXT_XTAL         0 /* If you have watch crystal connected set to 1 */


#if MEGATINYCORE_SERIES      == 2
  const uint8_t homeindex     = 1;
  #if F_CPU                  == 16000000
    const float targets[6]    = {12.0, 16.0, 20.0, 24.0, 25.0, 30.0};
    #define CAL_START (USER_SIGNATURES_SIZE - 12)
  #elif F_CPU                == 20000000
    const float targets[6]    = {16.0, 20.0, 24.0, 25.0, 30.0, 32.0};
    #define CAL_START (USER_SIGNATURES_SIZE - 6)
  #endif
#else  // 0/1 series
  const uint8_t homeindex     = 2;
  #if F_CPU                  == 16000000
    const float targets[6]    = {10.0, 12.0, 16.0, 20.0, 24.0, 25.0};
    #define CAL_START (USER_SIGNATURES_SIZE - 12)
  #elif F_CPU                == 20000000
    const float targets[6]    = {12.0, 16.0, 20.0, 24.0, 25.0, 30.0};
    #define CAL_START (USER_SIGNATURES_SIZE - 6)
  #endif
#endif

#define DEBUG
//#define OUTPUT_1K_CLOCK

volatile uint16_t TCBtime1[128];
volatile uint16_t TCBtime2[128];
uint8_t datapoint             = 0;
uint8_t baselineOSCCAL        = 0;
uint32_t baselinesum          = 0;
int16_t baseasymmetry         = 0;
float calibratedFreq          = 0.0;
float lastspeed               = 0;
float newspeed                = 0;
volatile uint8_t samplecount  = 0;
void setup() {
  VPORTA.DIR |= 0x80;
  GPIOR0 = 0;
  #if defined(DEBUG)
    Serial.begin(115200);
    volatile uint8_t * sigptr = (volatile uint8_t *) &SIGROW;
    sigptr = Serial.printHex(sigptr,16, ':');
    sigptr = Serial.printHex(sigptr,16, ':');
    sigptr = Serial.printHex(sigptr,16, ':');
    Serial.printHex(sigptr,16, ':');
    if (        (FUSE.OSCCFG & 0x03) == 0x01) {
      #if F_CPU == 20000000L
        Serial.println("ERROR: Fuses set for 16 MHz but sketch compiled for 20. STOP.");
        while(1);
      #else
        Serial.println("Oscillator configured for 16 MHz operation: OK");
      #endif
    } else if ( (FUSE.OSCCFG & 0x03) == 0x02) {
      #if F_CPU == 16000000L
        Serial.println("ERROR: Fuses set for 20 MHz but sketch compiled for 16. STOP.");
        while(1);
      #else
        Serial.println("Oscillator configured for 20 MHz operation: OK");
      #endif
    } else {
      Serial.println("ERROR: Oscillator configuration fuse set to invalid value. STOP.");
    }
    if (USERSIG.read(CAL_START + homeindex) != 0xFF) {
      Serial.println("Tuning process appears to have already been done");
    }
  #else
    #if F_CPU == 20000000L
      if ((FUSE.OSCCFG & 0x03) == 0x01)  ImTotallyJustBlink();
    #elif F_CPU == 16000000L
      if ((FUSE.OSCCFG & 0x03) == 0x02)  ImTotallyJustBlink();
    #else
      ImTotallyJustBlink();
    #endif
    if (USERSIG.read(CAL_START + homeindex) != 0xFF) {
      tidyTuningValues();
    // This goes through and writes 0xC0, the code that indicates that the device cannot reach that speed
    // as the tuning value for all targets which were not written previously, indicating that the tuning process
    // resulted in a crash or reset, which it often does on the 20 MHz setting, before it can get to 0x7F..
      ImTotallyJustBlink();
    }
  #endif

  initEVSYS();
  initRTC();
  takeOverTCA0();
  #ifdef OUTPUT_1K_CLOCK
    uint16_t newper     = (F_CPU / 2000);
    TCA0.SINGLE.PER     = (newper  - 1);
    TCA0.SINGLE.CMP1    = (newper >> 1) - 1 ;
    TCA0.SINGLE.CTRLB   = 0x23; //single slope cmp1en=.
    TCA0.SINGLE.CTRLA   = 0x03;
    openDrainFast(PIN_PB1, LOW);
  #endif
  baselineOSCCAL = CLKCTRL_OSC20MCALIBA;
  #if F_CPU == 20000000
    #if MEGATINYCORE_SERIES != 2
      calibratedFreq      = 20.0 * (1024.0 + getOscError()) / 1024;
    #else
      calibratedFreq      = 20.0;
    #endif
  #else
    #if MEGATINYCORE_SERIES != 2
      calibratedFreq      = 16.0 * (1024.0 + getOscError()) / 1024;
    #else
      calibratedFreq      = 16.0;
    #endif
  #endif
  init_selftimer();
  while (!(GPIOR0 & 0x01));
  for (uint8_t i = 0; i < 128; i++) {
    uint16_t endtime    = TCBtime2[i];
    uint16_t starttime  = TCBtime1[i];
    baselinesum        += endtime;
    endtime            -= starttime;
    baseasymmetry      += starttime;
    baseasymmetry      -= endtime;
  }
  #ifdef DEBUG
    Serial.println(baselinesum);
    Serial.printHexln(baselineOSCCAL);
    Serial.println(baseasymmetry);
    Serial.print("Freq: ");
    Serial.println(calibratedFreq);
    Serial.flush();
  #endif
  GPIOR0 &= ~0x01;
}

int8_t getOscError() {
  #if (MEGATINYCORE_SERIES != 2)
    #if F_CPU == 16000000
      return SIGROW.OSC16ERR5V;
    #else
      return SIGROW.OSC20ERR5V;
    #endif
  #else
    return 0;
  #endif
}

void init_selftimer() {
  TCB0.CTRLB    = 0x05; // freq and PW
  TCB0.EVCTRL   = 1;
  TCB0.INTFLAGS = 0xFF;
  TCB0.INTCTRL  = 1;
  TCB0.CTRLA    = 3;
}

void restartData() {
  samplecount   = 0;
  TCB0.INTFLAGS = 0xFF;
  TCB0.INTCTRL  = 1;
  TCB0.CTRLA    = 3;
}
void processData() {
  lastspeed = newspeed;
  uint32_t newsum=0;
  for        (uint8_t i = 0; i < 128; i++) {
    uint16_t endtime    = TCBtime2[i];
    uint16_t starttime  = TCBtime1[i];
    newsum             += endtime;
    (void) starttime;
  }
  newspeed = (calibratedFreq * newsum)/baselinesum;
  Serial.print("Speed: ");
  Serial.print(newspeed);
  Serial.print("OSCCAL: ");
  Serial.println(datapoint);
  for (uint8_t x = 0; x < 6; x++) {
    if ((newspeed > targets[x]) && (lastspeed < targets[x])){
      /* One of these is the target!! */
      Serial.println("Found a target.");
      if ((targets[x] - lastspeed) > (newspeed - targets[x])) {
        USERSIG.write(CAL_START + x, datapoint);
      } else {
        USERSIG.write(CAL_START + x, datapoint - 1);
      }
    }
  }
}

ISR(TCB0_INT_vect) {
  TCBtime2[samplecount]   = TCB0.CNT;
  TCBtime1[samplecount++] = TCB0.CCMP;
  if (samplecount > 127) {
    TCB0.INTCTRL   = 0;
    VPORTA.IN     |= 0x20;
    TCB0.CTRLA     = 2;
    GPIOR0        |= 1;
  }
}


void ImTotallyJustBlink() {
  pinMode(LED_BUILTIN, OUTPUT);
  while(1) { /* pretend to be blink */
    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWriteFast(LED_BUILTIN, LOW);
    delay(1000);
  }
}

void initEVSYS() {
#if !defined(MEGATINYCORE) || MEGATINYCORE_SERIES == 2
  EVSYS.USERTCB0CAPT    = 0x02;
  EVSYS.CHANNEL1        = 0x0B; // PIT/64
  EVSYS.USEREVSYSEVOUTA = 0x02; //connect evout pin to this
  //PORTMUX.EVSYSROUTEA  = 1; //Route to PA7
#else
  EVSYS.ASYNCCH3        = 0x11; //PIT/64
  EVSYS.ASYNCUSER8      = 0x06;
  EVSYS.ASYNCUSER9      = 0x06;
  //PORTMUX.CTRLA = 2;
#endif
  //Now we have visualization of the PIT!
}

void initRTC() {

  while (RTC.STATUS || RTC.PITSTATUS);
  #if (HAS_EXT_XTAL==1)
      _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0x32);
      _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0x33);
    RTC.CLKSEL            = 0x02;
    RTC.CTRLA             = 0x01;
    while(!(CLKCTRL.MCLKSTATUS & 0x40)); //spin until osc started
  #else
    RTC.CLKSEL            = 0x00;
    RTC.CTRLA             = 0x01;
  #endif
  RTC.PITCTRLA          = 0x01;
}


void configureCal(uint8_t cal) {
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,     CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm); //prescale enabled, div by 4
  delay(10);
  _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA,  cal); /* Switch to new clock - prescale of 4 already on */
  _NOP(); //voodoo to prevent malfunctions from changing the clock live.
  delay(1);
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,     CLKCTRL_PDIV_4X_gc);   //prescale disabled, div by 4
  delay(1);
}
void displaypoint() {
  //Serial.printf("%02hhX: %8lu        %5d", datapoint, sums[datapoint], asyms[datapoint]);
  Serial.printHex(datapoint);
  Serial.print(":  ");
  Serial.println(newspeed);
  Serial.flush();
}

void loop() {

  VPORTA.IN         |= 0x80;
  delay(10);
  //Serial.printHex(datapoint);
  if (GPIOR0 & 1) {
    configureCal(baselineOSCCAL);
    processData();
    displaypoint();
    #if MEGATINYCORE_SERIES == 2
      if (datapoint == 0x7F)
    #else
      if (datapoint == 0x3F)
    #endif
    {
      while (1);
    } else
    datapoint++;
    GPIOR0          &= ~0x01;
    GPIOR0          &= ~0x02;
  } else if(!(GPIOR0 &  0x02)) {
    configureCal(datapoint);
    restartData();
    GPIOR0          |=  0x02;
  }
}

void tidyTuningValues() {
  for (uint8_t j = 0; j < 6; j++) {
    if (USERSIG.read(CAL_START + j) == 0xFF) {
      if (j < homeindex) {
        // must be too low Mark as such
        USERSIG.write(CAL_START + j, 0x80);
      } else {
        USERSIG.write(CAL_START + j, 0xC0); // indicating we didn't even make it there.
      }
    }
  }
}
