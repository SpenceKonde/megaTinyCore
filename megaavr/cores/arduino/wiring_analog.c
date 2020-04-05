/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul
*/

#include "wiring_private.h"
#include "pins_arduino.h"
#include "Arduino.h"

void analogReference(uint8_t mode)
{
	switch (mode)
	{
	#ifdef EXTERNAL
		case EXTERNAL:
	#endif
		case VDD:
			ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | mode | ADC_SAMPCAP_bm; //per datasheet, recommended SAMPCAP=1 at ref > 1v - we don't *KNOW* the external reference will be >1v, but it's probably more likely...
			// VREF.CTRLA does not need to be reconfigured, as the voltage references only supply their specified voltage when requested to do so by the ADC.
			break;
		case INTERNAL0V55:
			VREF.CTRLA =  VREF.CTRLA & ~(VREF_ADC0REFSEL_gm); //These bits are all 0 for 0.55v reference, so no need to do the mode << VREF_ADC0REFSEL_gp here;
			ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm | ADC_SAMPCAP_bm)) | INTERNAL; //per datasheet, recommended SAMPCAP=0 at ref < 1v
			break;
		case INTERNAL1V1:
		case INTERNAL2V5:
		case INTERNAL4V34:
		case INTERNAL1V5:
			VREF.CTRLA = (VREF.CTRLA & ~(VREF_ADC0REFSEL_gm)) | (mode << VREF_ADC0REFSEL_gp);
			ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | INTERNAL | ADC_SAMPCAP_bm; //per datasheet, recommended SAMPCAP=1 at ref > 1v
			break;
		default:
			ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | VDD | ADC_SAMPCAP_bm; //per datasheet, recommended SAMPCAP=1 at ref > 1v - we don't *KNOW* the external reference will be >1v, but it's probably more likely...
	}
}


int analogRead(uint8_t pin)
{
	#ifdef ADC_DAC0
		if (pin!=ADC_DAC0 && pin != ADC_INTREF && pin != ADC_TEMPERATURE)
	#else
		if (pin != ADC_INTREF && pin != ADC_TEMPERATURE)
	#endif
	{
		pin = digitalPinToAnalogInput(pin);
		if(pin == NOT_A_PIN) return -1;
	}


#if defined(ADC0)
	/* Reference should be already set up */
	/* Select channel */
	ADC0.MUXPOS = (pin << ADC_MUXPOS_gp);

	/* Start conversion */
	ADC0.COMMAND = ADC_STCONV_bm;

	/* Wait for result ready */
	while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));

	/* Combine two bytes */
	return ADC0.RES;

#else	/* No ADC, return 0 */
	return 0;
#endif

}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
	uint8_t bit_pos  = digitalPinToBitPosition(pin);
	if(bit_pos == NOT_A_PIN) return;
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistently with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);

	//if(val < 1){	/* if zero or negative drive digital low */

	//	digitalWrite(pin, LOW);

	//} else if(val > 255){	/* if max or greater drive digital high */

	//	digitalWrite(pin, HIGH);

	//} else {	/* handle pwm to generate analog value */
	/* Get timer */
	uint8_t digital_pin_timer =  digitalPinToTimer(pin);
	uint8_t* timer_cmp_out;

	//TCB_t *timer_B;
	/* Find out Port and Pin to correctly handle port mux, and timer. */
	switch (digital_pin_timer) { //use only low nybble which defines which timer it is

		case TIMERA0:
			if(val <= 0){	/* if zero or negative drive digital low */
				digitalWrite(pin, LOW);
			} else if(val >= 255){	/* if max or greater drive digital high */
				digitalWrite(pin, HIGH);
			} else {
				/* Calculate correct compare buffer register */
				#ifdef __AVR_ATtinyxy2__
				if (bit_pos==7) bit_pos=0; //on the xy2, WO0 is on PA7
				#endif
				if (bit_pos>2) {
					bit_pos-=3;
					timer_cmp_out = ((uint8_t*) (&TCA0.SPLIT.HCMP0)) + (bit_pos<<1);
					(*timer_cmp_out) = (val);
					TCA0.SPLIT.CTRLB |= (1 << (TCA_SPLIT_HCMP0EN_bp + bit_pos));
				} else {
					timer_cmp_out = ((uint8_t*) (&TCA0.SPLIT.LCMP0)) + (bit_pos<<1);
					(*timer_cmp_out) = (val);
					TCA0.SPLIT.CTRLB |= (1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));
				}
			}
			break;
		/* None of these parts have a Timer B that gives us PWM on a pin we don't already have it on.
		case TIMERB0:
		case TIMERB1:
		case TIMERB2:
		case TIMERB3:


			// Get pointer to timer, TIMERB0 order definition in Arduino.h
			//assert (((TIMERB0 - TIMERB3) == 2));
			timer_B = ((TCB_t *)&TCB0 + (digital_pin_timer - (TIMERB0&0x07));

			// set duty cycle
			timer_B->CCMPH = val;

			///Enable Timer Output
			timer_B->CTRLB |= (TCB_CCMPEN_bm);

			break;
						*/
		#if defined(DAC0) && defined(DACVREF)
		case DACOUT:
			DAC0.DATA=val;
			DAC0.CTRLA=0x41; //OUTEN=1, ENABLE=1
			break;
		#endif
	    #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
		case TIMERD0:
			if(val < 1){	/* if zero or negative drive digital low */
				digitalWrite(pin, LOW);
			} else if(val > 254){	/* if max or greater drive digital high */
				digitalWrite(pin, HIGH);
			} else {
			    if (bit_pos) {
			    	TCD0.CMPBSET=(255-val)<<1;
			    } else {
			    	TCD0.CMPASET=(255-val)<<1;
			    }
				if (!(TCD0.FAULTCTRL & (1<<(6+bit_pos)))) { //bitpos will be 0 or 1 for TIMERD pins
					//if not active, we need to activate it, which produces a glitch in the PWM
					TCD0.CTRLA=TIMERD0_PRESCALER;//stop the timer
					while(!(TCD0.STATUS&0x01)) {;} // wait until it's actually stopped
					uint8_t sreg=SREG;
					cli();
					_PROTECTED_WRITE(TCD0.FAULTCTRL,TCD0.FAULTCTRL|(1<<(6+bit_pos)));
					SREG=sreg;
					TCD0.CTRLA=TIMERD0_PRESCALER|1; //reenable it
				} else {
					while(!(TCD0.STATUS&0x02)) {;} //if previous sync in progress, wait for it to finish.
					TCD0.CTRLE=0x02; //Synchronize
				}
			}
			break;

		#endif

		/* If non timer pin, or unknown timer definition.	*/
		/* do a digital write	*/
		case NOT_ON_TIMER:
		default:
			if (val < 128) {
				digitalWrite(pin, LOW);
			} else {
				digitalWrite(pin, HIGH);
			}
			break;
	}
}
