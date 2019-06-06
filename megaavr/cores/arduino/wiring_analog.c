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

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	/* Clear relevant settings */
	ADC0.CTRLC &= ~(ADC_REFSEL_gm);
	VREF.CTRLA &= ~(VREF_ADC0REFSEL_gm);

	/* If reference NOT using internal reference from VREF */
	if((mode == EXTERNAL) || (mode == VDD)) {

		/* Set reference in ADC peripheral */
		ADC0.CTRLC |= mode;

	/* If reference using internal reference from VREF */
	} else if (
	   (mode == INTERNAL0V55)
		|| (mode == INTERNAL1V1)
		|| (mode == INTERNAL2V5)
		|| (mode == INTERNAL4V3)
		|| (mode == INTERNAL1V5)) {

		/* Set ADC reference to INTERNAL */
		ADC0.CTRLC |= INTERNAL;

		/* Configure VREF ADC0 reference */
		VREF.CTRLA |= (mode << VREF_ADC0REFSEL_gp);

	/* Non-standard values / default */
	} else {

		/* Non valid value will set default */
		/* Set ADC reference to INTERNAL */
		ADC0.CTRLC |= INTERNAL;

		/* Configure VREF ADC0 reference */
		VREF.CTRLA |= (INTERNAL0V55 << VREF_ADC0REFSEL_gp);
	}
}

int analogRead(uint8_t pin)
{
	pin = digitalPinToAnalogInput(pin);
	if(pin > NUM_ANALOG_INPUTS) return NOT_A_PIN;
	
	/* Check if TWI is operating on double bonded pin (Master Enable is high 
		in both Master and Slave mode for bus error detection, so this can 
		indicate an active state for Wire) */
	if(isDoubleBondedActive(pin)) return 0;

	uint8_t low, high;

#if defined(analogPinToChannel)
	/* If analog pin number != adc0 channel */
#endif

#if defined(ADC0)
	/* Reference should be already set up */
	/* Select channel */
	ADC0.MUXPOS = (pin << ADC_MUXPOS_gp);

	/* Start conversion */
	ADC0.COMMAND = ADC_STCONV_bm;

	/* Wait for result ready */
	while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));

	/* Save state */
	uint8_t status = SREG;
	cli();

	/* Read result */
	low = ADC0.RESL;
	high = ADC0.RESH;

	/* Restore state */
	SREG = status;

#else	/* No ADC, return 0 */
	low = 0;
	high = 0;
#endif

	/* Combine two bytes */
	return (high << 8) | low;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{

	uint8_t bit_pos  = digitalPinToBitPosition(pin);
	if(bit_pos == NOT_A_PIN || isDoubleBondedActive(pin)) return;

	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistently with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);

	if(val < 1){	/* if zero or negative drive digital low */

		digitalWrite(pin, LOW);

	} else if(val > 255){	/* if max or greater drive digital high */

		digitalWrite(pin, HIGH);

	} else {	/* handle pwm to generate analog value */

		/* Get timer */
		uint8_t digital_pin_timer =  digitalPinToTimer(pin);

		uint16_t* timer_cmp_out;
		TCB_t *timer_B;

		/* Find out Port and Pin to correctly handle port mux, and timer. */
		switch (digital_pin_timer) {

			case TIMERA0:
				/* Calculate correct compare buffer register */
				timer_cmp_out = ((uint16_t*) (&TCA0.SINGLE.CMP0BUF)) + bit_pos;

				/* Configure duty cycle for correct compare channel */
				(*timer_cmp_out) = (val);

				/* Enable output on pin */
				TCA0.SINGLE.CTRLB |= (1 << (TCA_SINGLE_CMP0EN_bp + bit_pos));

				break;

			case TIMERB0:
			case TIMERB1:
			case TIMERB2:
			case TIMERB3:

				/* Get pointer to timer, TIMERB0 order definition in Arduino.h*/
				//assert (((TIMERB0 - TIMERB3) == 2));
				timer_B = ((TCB_t *)&TCB0 + (digital_pin_timer - TIMERB0));

				/* set duty cycle */
				timer_B->CCMPH = val;

				/* Enable Timer Output	*/
				timer_B->CTRLB |= (TCB_CCMPEN_bm);

				break;

			/* If non timer pin, or unknown timer definition.	*/
			/* do a digital write								*/

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
}