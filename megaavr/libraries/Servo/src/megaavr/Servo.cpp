#if defined(ARDUINO_ARCH_MEGAAVR)

#include <Arduino.h>
#include <Servo.h>

#if (F_CPU > 10000000)
  #define usToTicks(_us)    ((( (_us) / 2) * clockCyclesPerMicrosecond()))                 // converts microseconds to tick
  #define ticksToUs(_ticks) (((unsigned) (_ticks) * 2) / clockCyclesPerMicrosecond())   // converts from ticks back to microseconds
  #define TRIM_DURATION  51                                   // compensation ticks to trim adjust for digitalWrite delays
#else
  #define usToTicks(_us)    ((( _us ) * clockCyclesPerMicrosecond()))                 // converts microseconds to tick
  #define ticksToUs(_ticks) (((unsigned) _ticks ) / clockCyclesPerMicrosecond())      // converts from ticks back to microseconds
  #define TRIM_DURATION  102                                                          // Makes a hell of a lot less difference than you'd think!
#endif

#define REFRESH_INTERVAL_TICKS (REFRESH_INTERVAL)



static servo_t servos[MAX_SERVOS];                         // static array of servo structures

uint8_t ServoCount = 0;                                    // the total number of attached servos

static volatile int8_t currentServoIndex[_Nbr_16timers];   // index for the servo being pulsed for each timer (or -1 if refresh interval)

// convenience macros
#define SERVO_INDEX_TO_TIMER(_servo_nbr) ((timer16_Sequence_t)(_servo_nbr / SERVOS_PER_TIMER))   // returns the timer controlling this servo
#define SERVO_INDEX_TO_CHANNEL(_servo_nbr) (_servo_nbr % SERVOS_PER_TIMER)                       // returns the index of the servo on this timer
#define SERVO_INDEX(_timer,_channel)  ((_timer*SERVOS_PER_TIMER) + _channel)                     // macro to access servo index by timer and channel
#define SERVO(_timer,_channel)  (servos[SERVO_INDEX(_timer,_channel)])                           // macro to access servo class by timer and channel

#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min * 4)   // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max * 4)   // maximum value in uS for this servo

void ServoHandler(int timer) {
  static uint32_t currentCycleTicks=0;
  if (currentServoIndex[timer] < 0) {
    // we're at servo index -1... but before we go further - we need to see if we've waited through a refresh cycle...
    if (currentCycleTicks < usToTicks(REFRESH_INTERVAL)) {
      uint32_t tval=usToTicks(REFRESH_INTERVAL)-currentCycleTicks;
      uint16_t tval16=(tval>65535)?65535:tval;
      _timer->CCMP = tval16;
      currentCycleTicks+=tval16;
      // wait longer, and terminate the ISR without further action./* Clear flag */
      _timer->INTFLAGS = TCB_CAPT_bm;
      return;
    } else {
      // current cycle is done, so reset the currentCycleTicks, and run the rest of things...
      currentCycleTicks=0;
    }
  } else {
    if (SERVO_INDEX(timer, currentServoIndex[timer]) < ServoCount && SERVO(timer, currentServoIndex[timer]).Pin.isActive == true) {
      //digitalWrite(SERVO(timer, currentServoIndex[timer]).Pin.nbr, LOW);   // pulse this channel low if activated
      ((PORT_t *)&PORTA + SERVO(timer, currentServoIndex[timer]).Pin.port)->OUTCLR=SERVO(timer, currentServoIndex[timer]).Pin.bitmask;
    }
  }

  // Select the next servo controlled by this timer
  currentServoIndex[timer]++;

  if (SERVO_INDEX(timer, currentServoIndex[timer]) < ServoCount && currentServoIndex[timer] < SERVOS_PER_TIMER) {
    if (SERVO(timer, currentServoIndex[timer]).Pin.isActive == true) {   // check if activated
      //digitalWrite(SERVO(timer, currentServoIndex[timer]).Pin.nbr, HIGH);   // it's an active channel so pulse it high
      ((PORT_t *)&PORTA + SERVO(timer, currentServoIndex[timer]).Pin.port)->OUTSET=SERVO(timer, currentServoIndex[timer]).Pin.bitmask;
    }

    // Get the counter value
    _timer->CCMP = SERVO(timer, currentServoIndex[timer]).ticks;
    currentCycleTicks+=SERVO(timer, currentServoIndex[timer]).ticks;
  } else {
    // finished all channels so wait for the refresh period to expire before starting over
    if (currentCycleTicks + 100 < usToTicks(REFRESH_INTERVAL)) {   // let's not bother if it's gonna take less than 100 ticks for the next
      uint32_t tval=usToTicks(REFRESH_INTERVAL)-currentCycleTicks;
      uint16_t tval16=tval>65535?65535:tval;
      _timer->CCMP = tval16;
      currentCycleTicks+=tval16;
    } else { // in this case it's less than 100 ticks for next interval, so we just set it to that...
      currentCycleTicks=usToTicks(REFRESH_INTERVAL);
    }
    currentServoIndex[timer] = -1;   // this will get incremented at the end of the refresh period to start again at the first channel
  }
  /* Clear flag */
  _timer->INTFLAGS = TCB_CAPT_bm;
}

#if defined USE_TIMERB0
  ISR(TCB0_INT_vect)
#elif defined USE_TIMERB1
  ISR(TCB1_INT_vect)
#elif defined USE_TIMERB2
  ISR(TCB2_INT_vect)
#endif
{
  ServoHandler(0);
}

static void initISR() {
  //divide CLK_PER by 2 instead of using TCA0-prescaled at 16/20MHz
  #if (F_CPU > 10000000)
  _timer->CTRLA = TCB_CLKSEL_CLKDIV2_gc;
  #else // and don't divide it at all at lower clock speeds
  _timer->CTRLA = TCB_CLKSEL_CLKDIV1_gc;
  #endif
  // Timer to Periodic interrupt mode
  // This write will also disable any active PWM outputs
  _timer->CTRLB = TCB_CNTMODE_INT_gc;

  _timer->CCMP = 0x8000; //Experience has shown that without this, it goes off the rails
  // Enable interrupt
  _timer->INTCTRL = TCB_CAPTEI_bm;
  // Enable timer
  _timer->CTRLA |= TCB_ENABLE_bm;
}

static void finISR() {
  // Disable interrupt
  _timer->INTCTRL = 0;
}

static boolean isTimerActive(timer16_Sequence_t timer) {
  // returns true if any servo is active on this timer
  for (uint8_t channel = 0; channel < SERVOS_PER_TIMER; channel++) {
    if (SERVO(timer, channel).Pin.isActive == true) {
      return true;
    }
  }
  return false;
}

/****************** end of static functions ******************************/

Servo::Servo() {
  if (ServoCount < MAX_SERVOS) {
    this->servoIndex = ServoCount++;                    // assign a servo index to this instance
    servos[this->servoIndex].ticks = usToTicks(DEFAULT_PULSE_WIDTH);   // store default values
  } else {
    this->servoIndex = INVALID_SERVO;  // too many servos
  }
}

uint8_t Servo::attach(byte pin) {
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t Servo::attach(byte pin, int min, int max) {
  timer16_Sequence_t timer;

  if (this->servoIndex < MAX_SERVOS) {
    //pinMode(pin, OUTPUT);                                   // set servo pin to output
    //servos[this->servoIndex].Pin.nbr = pin;
    uint8_t bitmask=digitalPinToBitMask(pin);
    if (bitmask==NOT_A_PIN) return NOT_A_PIN;
    servos[this->servoIndex].Pin.bitmask=bitmask;
    uint8_t prt=digitalPinToPort(pin);
    servos[this->servoIndex].Pin.port=prt;
    PORT_t *port=((PORT_t *)&PORTA + prt);
    port->DIRSET=bitmask;
    // todo min/max check: abs(min - MIN_PULSE_WIDTH) /4 < 128
    this->min  = (MIN_PULSE_WIDTH - min) / 4; //resolution of min/max is 4 uS
    this->max  = (MAX_PULSE_WIDTH - max) / 4;
    // initialize the timer if it has not already been initialized
    timer = SERVO_INDEX_TO_TIMER(servoIndex);
    if (isTimerActive(timer) == false) {
      initISR();
    }
    servos[this->servoIndex].Pin.isActive = true;  // this must be set after the check for isTimerActive
  }
  return this->servoIndex;
}

void Servo::detach() {
  timer16_Sequence_t timer;

  servos[this->servoIndex].Pin.isActive = false;
  timer = SERVO_INDEX_TO_TIMER(servoIndex);
  if (isTimerActive(timer) == false) {
    finISR();
  }
}

void Servo::write(unsigned int value) {
  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
  if (value < MIN_PULSE_WIDTH) {
    // ditch this, the argument is an unsigned int, so this is pointless...
    //if (value < 0) {
    //  value = 0;
    //} else
    if (value > 180) {
      value = 180;
    }

    value = map(value, 0, 180, SERVO_MIN(), SERVO_MAX());
  }
  writeMicroseconds(value);
}


void Servo::writeMicroseconds(unsigned int value) {
  // calculate and store the values for the given channel
  byte channel = this->servoIndex;
  if ((channel < MAX_SERVOS)) {  // ensure channel is valid
    if (value < (uint16_t) SERVO_MIN()) {        // ensure pulse width is valid
      value = SERVO_MIN();
    } else if (value > (uint16_t) SERVO_MAX()) {
      value = SERVO_MAX();
    }


    value = usToTicks(value);  // convert to ticks BEFORE compensating for interrupt overhead
    value = value - TRIM_DURATION;
    servos[channel].ticks = value;
  }
}

int Servo::read() { // return the value as degrees
  return map(readMicroseconds() + 1, SERVO_MIN(), SERVO_MAX(), 0, 180);
}

unsigned int Servo::readMicroseconds() {
  unsigned int pulsewidth;
  if (this->servoIndex != INVALID_SERVO) {
    pulsewidth = ticksToUs(servos[this->servoIndex].ticks + TRIM_DURATION);
  } else {
    pulsewidth  = 0;
  }
  return pulsewidth;
}

bool Servo::attached() {
  return servos[this->servoIndex].Pin.isActive;
}

#endif
