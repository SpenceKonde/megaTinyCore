# Performance of common API functions

Speed  | digitalWrite(OUTPUT)  | digitalWrite(INPUT)   | digitalRead()   | analogRead()  | pinMode()
------------   | ------------  | ------------  | ------------  | ------------  | ------------
Clock Cycles   | 130   | 153   | 108.5   | N/A   | 52.5
20 MHz   | 6.50  | 7.65  | 5.43  | 56.01   | 2.63
16 MHz   | 8.13  | 9.56  | 6.78  | 70.01   | 3.28
10 MHz   | 13.00   | 15.30   | 10.85   | 89.35   | 5.25
8 MHz  | 16.25   | 19.13   | 13.56   | 111.68  | 6.56
5 MHz  | 26.00   | 30.60   | 21.70   | 91.09   | 10.50
4 MHz  | 32.50   | 38.25   | 27.13   | 113.87  | 13.13
1 MHz  | 130.0   | 153.00  | 108.50  | 125.46  | 52.50

## Notes

### digitalWrite() on INPUT vs OUTPUT pins
On classic AVRs, the PORT register controlled whether the pullups were enabled; That is not the case with the megaAVR parts, but much code is in circulation that assumes this, so the behavior is emulated. However, this imposes a performance penaly on digitalWrite() when called on an input pin.

### pinMode() is average of INPUT and OUTPUT
Setting a pin OUTPUT is simpler than setting it as INPUT. Result shown is an average.

### analogRead depends on the ADC clock
analogRead depends in large part on the ADC clock, which is set to 156.25 kHz for 20/10/5 MHz boards, and 125 kHz for other clock speeds. However, it does still require some normal processor time before and after the conversion, as shown by the longer times at slower clock speeds.
I am not sure why analogRead() is so much faster at 20 MHz either - this may be addressed in a future version.
