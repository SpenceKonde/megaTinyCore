#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void yield(void);

#define LOW              0    /* used for digitalWrite(), digitalRead(), openDrain() and attachInterrupt() */
#define HIGH             1    /* used for digitalWrite(), digitalRead(). There is no option for HIGH level interrupt provided by the hardware */
#define FLOATING         HIGH /* Not in official API - used for openDrain, while making clear what you're doing */
#define CHANGE           4    /* used for attachInterrupt() */
#define FALLING          2    /* used for attachInterrupt() */
#define RISING           3    /* used for attachInterrupt() */
#define INPUT            0    /* used for pinMode() */
#define OUTPUT           1    /* used for pinMode() */
#define INPUT_PULLUP     2    /* used for pinMode() */
#define LSBFIRST         0    /* used for shiftIn/shiftOut */
#define MSBFIRST         1    /* used for shiftIn/shiftOut */


#define PI            3.1415926535897932384626433832795
#define HALF_PI       1.5707963267948966192313216916398
#define TWO_PI        6.283185307179586476925286766559
#define DEG_TO_RAD    0.017453292519943295769236907684886
#define RAD_TO_DEG    57.295779513082320876798154814105
#define EULER         2.718281828459045235360287471352

#define SERIAL        0x00
#define DISPLAY       0x01

#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#ifndef constrain
#define constrain(x,low,high)   ({  \
  __typeof__ (x)    _x = (x);           \
  __typeof__ (low)  _l = (low);         \
  __typeof__ (high) _h = (high);        \
  _x < _l ? _l : _x > _h ? _h :_x ;})
#endif

#ifndef radians
#define radians(deg) ((deg)*DEG_TO_RAD)
#endif

#ifndef degrees
#define degrees(rad) ((rad)*RAD_TO_DEG)
#endif

#ifndef sq
#define sq(x)        ({ __typeof__ (x) _x = (x); _x * _x; })
#endif


/* This is sorta silly. But normal round() builtin is bulkier and returns a float while arduino code written for other plaforms will expect
integer types... This also has the same concerns about sideffects as before - except they're less likely to show up when
starting from a float. Who does meFloat++? Certainly not as many as do constrain(myInt++,minval,maxval);
*/

#ifndef round
#define round(x)     ({ __typeof__ (x) _x = (x);  _x >= 0 ? (long)(_x + 0.5) : (long)(_x - 0.5 );})
#endif

/* In the end, it is probably better to do this manually rather than using round(). Why?

* If you know the value you're starting with is postiive, you can skip the first test and just add 0.5 and cast to integer type - saves space.
* You can use the minimum size datatypes (wthis casts things to 4-byte values. Maybe the thing your rounding is, say -50.0 to 50.0 you can cast to a single byte for more compact code
* You can choose which of the rounding strategies you wish to use. Because there are not less than TEN DIFFERENT STRATEGIES TO ROUND NUMBERS
* Any of these approaches will result in smaller code:
* For example, rounding a number you know ought to fit a 16 bit unsigned datatype easily, you might do:
* uint16_t roundedval = (uint16_t) (floatval + 0.5)
*
* Even better, avoid using floats in the first place. Why are you using them? sometimes you really don't have much of a choice, other times
* you very definitely do. This is particularly true with ADC readings, for example say you're using to 4.096 Volt reference, and you
* want to measure and print out a voltage.
uint32_t reading = analogReadEnh(adc_pin, 12);  //where adc_pin is just the pin number being read
float temp = reading;
Serial.print(temp/1000); // 4.096v reference, and 12 bit accuracy (ie, 0-4095). So each LSB is 1 mv so dividing by 1000 gets volts.
Serial.println('V'); //print out the units too
3.2k on a 1626/
Or you could do:
uint32_t reading = analogReadEnh(adc_pin, 12); // same analog reading
Serial.print(reading/1000) //volts
Serial.print('.'); //decimal point
Serial.print(reading%1000) //millivolts
Serial.println('V'); //print out the units too
// This would print the same thing, only without any floating point derived error... weighing in at 2k of flash.

// And just doing this saves another 100 bytes:
uint32_t reading = analogReadEnh(1, 12);  //where adc_pin is just the pin number being read
Serial.print((uint16_t)reading);
Serial.println("mV");


* Yeah the last one is kind of a copoout,  but not only is it flash-efficient, it also runs *much* faster, because division
* be it floating point or otherwise, is sloowwwwwwww on AVR. But that was no excuse for round being broken like it was.
*/



typedef void (*voidFuncPtr)(void);

// interrupts() / noInterrupts() must be defined by the core

// a bunch of really silly hand-holding macros!
#define lowByte(w)  ((uint8_t) ((w)    &  0xff))
#define highByte(w) ((uint8_t) ((w)    >>    8))
#define  bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define   bitSet(value, bit)  ((value) |= (1UL << (bit)))
#define bitClear(value, bit)  ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#ifndef bit
#define bit(b) (1UL << (b))
#endif

#if defined(DISCOURAGE_NONSTANDARD_TYPES) && DISCOURAGE_NONSTANDARD_TYPES
typedef bool      boolean __attribute__ ((deprecated("a 'boolean' is called a 'bool' in standard C - suggest using standard type name")));
typedef uint8_t   byte __attribute__ ((deprecated("a 'byte' is an 'unsigned char' (may be more than 8 bits on non-AVR) or 'uint8_t' (always 8 bits) in standard C - suggest using one of these standard types.")));
typedef uint16_t  word __attribute__ ((deprecated("a 'word' is an 'unsigned int' (may be more than 16 bits on non-AVR) or 'uint15_t' (always 16 bits) in standard C - suggest using one of these standard types.")));
#else
typedef bool      boolean;
typedef uint8_t   byte;
typedef uint16_t  word;
#endif
void init(void);
void initVariant(void);

int atexit(void (*func)()) __attribute__((weak));
int main() __attribute__((weak));

#ifdef EXTENDED_PIN_MODE
// Platforms who want to declare more than 256 pins need to define EXTENDED_PIN_MODE globally
typedef uint32_t pin_size_t;
#else
typedef uint8_t pin_size_t;
#endif

void    pinMode(         pin_size_t pinNumber, uint8_t mode);
void    digitalWrite(    pin_size_t pinNumber, uint8_t val);
int8_t  digitalRead(     pin_size_t pinNumber);
void    analogWrite(     pin_size_t pinNumber, int value);
int     analogRead(      pin_size_t pinNumber);
void    analogReference(         uint8_t mode);
bool    analogReadResolution(     uint8_t res);

unsigned long millis(void);
unsigned long micros(void);

void delay(unsigned long);
void delayMicroseconds(unsigned int us);
unsigned long     pulseIn(pin_size_t pin, uint8_t state, unsigned long timeout);
unsigned long pulseInLong(pin_size_t pin, uint8_t state, unsigned long timeout);

void   shiftOut(pin_size_t dataPin, pin_size_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(pin_size_t dataPin, pin_size_t clockPin, uint8_t bitOrder);

void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, uint8_t mode);
void detachInterrupt(pin_size_t interruptNumber);

void setup(void);
void loop(void);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
  /* C++ prototypes */
  uint16_t makeWord(uint16_t w);
  uint16_t makeWord(byte h, byte l);

  #define word(...) makeWord(__VA_ARGS__)

  unsigned long     pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);
  unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);

  void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0);
  void noTone(uint8_t _pin);

  // WMath prototypes
  long random(long);
  long random(long, long);
  void randomSeed(unsigned long);
  long map(long, long, long, long, long);

#endif // __cplusplus
