#include "Wire.h"
#include <avr/sleep.h>


//This example was intended to demonstrate that it is possible to run
//a master and slave (MANDS) configuration on just 2kB Flash.
//This example uses the Periodic Interrupt Timer (PIT) to send
//an averaged ADC value to another device.
//But, if there is a write on address 0x1A with the data 0x04,
//an ADC measurement on another pin is made that can be read out
//by issuing a read on address 0x1A.


volatile uint8_t slaveRXflag;
volatile uint8_t rtc_flag;
uint8_t   adc_cnt;
uint16_t  adc_val;
uint32_t  adc_avg;


void setup() {
   RTC_init();
   Wire.begin(0x1A);
   Wire.begin();
   Wire.usePullups();
   Wire.onRequest(request);
   Wire.onReceive(receive);

}

void loop() {
   if (slaveRXflag !=  0) {
      adc_val      =   analogRead(PIN_PA7);
      slaveRXflag  =   0;
   }
   if (rtc_flag    !=  0) {
      adc_avg      +=  analogRead(PIN_PA6);
      adc_cnt++;
      if (adc_cnt  >   9) {
         adc_avg   /= 10;
         transmitVal(adc_avg);
         adc_avg   =   0;
         adc_cnt   =   0;
      }
      rtc_flag = 0;
   }

}


void RTC_init(void) {                        /* Initialize RTC:                                             */
  while (RTC.STATUS > 0) {}                  /* Wait for all register to be synchronized                    */
    RTC.CTRLA       = RTC_ENABLE_bm;         /* We must enable the RTC if using the PIT! See errata         */
    RTC.CLKSEL      = RTC_CLKSEL_INT32K_gc;  /* 32.768kHz Internal Ultra-Low-Power Oscillator (OSCULP32K)   */
    RTC.PITINTCTRL  = RTC_PI_bm;             /* PIT Interrupt: enabled                                      */
    RTC.PITCTRLA    = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 16384, resulting in 32.768kHz/32768 = 1Hz  */
    | RTC_PITEN_bm;                       /* /* Enable PIT counter: enabled                                 */
}

ISR(RTC_PIT_vect)
{
   rtc_flag = 1;
   RTC.PITINTFLAGS = RTC_PI_bm;              /* Clear interrupt flag by writing '1' (required)              */
}


void transmitVal(uint32_t value)
{
   Wire.beginTransmission(0x10);
   Wire.write((uint8_t) (value >> 24));
   Wire.write((uint8_t) (value >> 16));
   Wire.write((uint8_t) (value >>  8));
   Wire.write((uint8_t)  value);
   Wire.endTransmission(true);
}

void receive(int num) {
   (void) num;
   if (Wire.read() == 0x04) {
      slaveRXflag  =  0x01;
   }
}

void request(void) {
   Wire.write((uint8_t) (adc_val >> 8));
   Wire.write((uint8_t)  adc_val);
}
