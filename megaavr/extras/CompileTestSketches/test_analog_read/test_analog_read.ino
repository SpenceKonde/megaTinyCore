void setup() {
  // put your setup code here, to run once:

}



volatile long trashbag = 0;
// See it's a trashbag because nothing we put in there has any value
// Being used to make sure optimization can't eliminate stuff that we want to compiletest.
void loop() {
  analogReadResolution(ADC_NATIVE_RESOLUTION);
  analogReadResolution(10);
  analogReadResolution(8);
  #if defined(INTERNAL0V55)
    analogReference(INTERNAL0V55);
  #endif
  #if defined(INTERNAL1V1)
    analogReference(INTERNAL1V1);
  #endif
  #if defined(INTERNAL1V5)
    analogReference(INTERNAL1V5);
  #endif
  #if defined(INTERNAL4V34)
    analogReference(INTERNAL4V34);
  #endif
  #if defined(INTERNAL2V5)
    analogReference(INTERNAL2V5);
  #endif
  #if defined(INTERNAL1V024)
    analogReference(INTERNAL1V024);
  #endif
  #if defined(INTERNAL2V048)
    analogReference(INTERNAL2V048);
  #endif
  #if defined(INTERNAL4V096)
    analogReference(INTERNAL4V096);
  #endif
  #if defined(EXTERNAL)
    analogReference(EXTERNAL);
  #endif

  trashbag = analogRead(ADC_TEMPERATURE);
  trashbag = analogReadEnh(ADC_TEMPERATURE,ADC_NATIVE_RESOLUTION);
  trashbag = analogRead(ADC_GROUND);
  trashbag = analogReadEnh(ADC_GROUND,ADC_NATIVE_RESOLUTION);
  #ifdef DAC0
    trashbag = analogRead(ADC_DAC0);
    trashbag = analogReadEnh(ADC_DAC0,ADC_NATIVE_RESOLUTION);
  #endif
  #ifdef VDDDIV10
    trashbag = analogRead(ADC_VDDDIV10);
    trashbag = analogReadEnh(ADC_VDDDIV10,ADC_NATIVE_RESOLUTION);
  #endif
  #ifdef DACREF0
    trashbag = analogRead(ADC_DACREF0);
    trashbag = analogReadEnh(ADC_DACREF0,ADC_NATIVE_RESOLUTION);
  #endif
  #ifdef DACREF1
    trashbag = analogRead(ADC_DACREF0);
    trashbag = analogReadEnh(ADC_DACREF0,ADC_NATIVE_RESOLUTION);
  #endif
  #ifdef DACREF2
    trashbag = analogRead(ADC_DACREF0);
    trashbag = analogReadEnh(ADC_DACREF0,ADC_NATIVE_RESOLUTION);
  #endif


  //getAnalogSampleDuration(); Has to be removed because it prevented all sketches from compiling for reasons I couldn't fathom
  analogSampleDuration(10);
  analogClockSpeed(1000);

  #ifdef PIN_A0
    trashbag = analogRead(PIN_A0);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A0,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A0,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A0,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A0,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A0,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A0,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A0,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A0,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A0,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A0,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A0,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A0,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A1
    trashbag = analogRead(PIN_A1);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A1,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A1,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A1,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A1,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A1,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A1,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A1,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A1,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A1,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A1,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A1,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A1,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A2
    trashbag = analogRead(PIN_A2);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A2,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A2,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A2,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A2,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A2,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A2,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A2,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A2,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A2,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A2,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A2,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A2,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A3
    trashbag = analogRead(PIN_A3);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A3,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A3,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A3,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A3,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A3,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A3,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A3,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A3,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A3,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A3,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A3,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A3,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A4
    trashbag = analogRead(PIN_A4);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A4,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A4,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A4,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A4,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A4,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A4,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A4,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A4,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A4,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A4,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A4,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A4,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A5
    trashbag = analogRead(PIN_A5);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A5,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A5,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A5,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A5,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A5,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A5,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A5,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A5,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A5,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A5,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A5,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A5,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif
  #ifdef PIN_A6
    trashbag = analogRead(PIN_A6);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A6,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A6,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A6,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A6,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A6,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A6,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A6,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A6,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A6,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A6,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A6,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A6,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A7
    trashbag = analogRead(PIN_A7);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A7,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A7,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A7,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A7,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A7,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A7,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A7,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A7,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A7,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A7,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A7,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A7,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A8
    trashbag = analogRead(PIN_A8);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A8,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A8,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A8,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A8,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A8,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A8,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A8,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A8,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A8,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A8,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A8,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A8,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A9
    trashbag = analogRead(PIN_A9);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A9,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A9,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A9,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A9,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A9,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A9,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A9,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A9,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A9,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A9,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A9,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A9,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A10
    trashbag = analogRead(PIN_A10);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A10,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A10,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A10,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A10,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A10,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A10,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A10,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A10,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A10,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A10,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A10,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A10,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A11
    trashbag = analogRead(PIN_A11);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A11,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A11,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A11,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A11,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A11,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A11,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A11,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A11,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A11,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A11,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A11,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A11,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A12
    trashbag = analogRead(PIN_A12);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A12,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A12,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A12,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A12,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A12,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A12,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A12,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A12,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A12,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A12,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A12,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A12,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A13
    trashbag = analogRead(PIN_A13);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A13,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A13,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A13,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A13,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A13,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A13,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A13,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A13,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A13,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A13,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A13,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A13,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A14
    trashbag = analogRead(PIN_A14);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A14,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A14,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A14,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A14,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A14,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A14,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A14,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A14,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A14,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A14,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A14,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A14,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif
  #ifdef PIN_A15
    trashbag = analogRead(PIN_A15);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A15,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A15,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A15,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A15,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A15,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A15,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A15,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A15,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A15,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A15,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A15,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A15,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A16
    trashbag = analogRead(PIN_A16);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A16,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A16,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A16,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A16,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A16,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A16,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A16,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A16,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A16,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A16,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A16,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A16,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A17
    trashbag = analogRead(PIN_A17);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A17,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A17,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A17,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A17,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A17,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A17,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A17,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A17,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A17,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A17,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A17,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A17,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A18
    trashbag = analogRead(PIN_A18);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A18,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A18,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A18,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A18,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A18,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A18,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A18,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A18,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A18,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A18,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A18,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A18,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A19
    trashbag = analogRead(PIN_A19);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A19,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A19,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A19,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A19,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A19,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A19,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A19,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A19,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A19,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A19,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A19,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A19,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A20
    trashbag = analogRead(PIN_A20);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A20,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A20,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A20,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A20,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A20,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A20,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A20,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A20,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A20,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A20,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A20,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A20,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A21
    trashbag = analogRead(PIN_A21);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A21,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A21,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A21,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A21,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A21,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A21,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A21,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A21,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A21,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A21,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A21,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A21,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A22
    trashbag = analogRead(PIN_A22);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A22,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A22,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A22,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A22,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A22,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A22,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A22,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A22,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A22,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A22,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A22,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A22,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A23
    trashbag = analogRead(PIN_A23);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A23,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A23,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A23,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A23,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A23,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A23,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A23,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A23,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A23,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A23,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A23,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A23,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A24
    trashbag = analogRead(PIN_A24);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A24,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A24,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A24,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A24,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A24,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A24,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A24,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A24,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A24,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A24,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A24,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A24,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A25
    trashbag = analogRead(PIN_A25);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A25,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A25,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A25,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A25,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A25,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A25,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A25,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A25,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A25,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A25,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A25,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A25,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A26
    trashbag = analogRead(PIN_A26);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A26,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A26,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A26,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A26,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A26,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A26,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A26,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A26,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A26,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A26,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A26,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A26,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A27
    trashbag = analogRead(PIN_A27);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A27,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A27,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A27,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A27,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A27,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A27,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A27,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A27,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A27,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A27,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A27,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A27,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A28
    trashbag = analogRead(PIN_A28);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A28,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A28,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A28,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A28,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A28,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A28,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A28,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A28,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A28,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A28,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A28,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A28,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A29
    trashbag = analogRead(PIN_A29);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A29,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A29,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A29,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A29,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A29,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A29,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A29,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A29,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A29,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A29,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A29,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A29,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A30
    trashbag = analogRead(PIN_A30);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A30,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A30,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A30,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A30,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A30,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A30,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A30,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A30,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A30,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A30,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A30,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A30,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif

  #ifdef PIN_A31
    trashbag = analogRead(PIN_A31);
    for (int8_t res = 8; res < ADC_MAX_OVERSAMPLED_RESOLUTION; res++) {
      trashbag = analogReadEnh(PIN_A31,ADC_NATIVE_RESOLUTION);
    }
    #ifdef ADC_ACC2
      trashbag = analogReadEnh(PIN_A31,ADC_ACC2);
    #endif
    #ifdef ADC_ACC4
      trashbag = analogReadEnh(PIN_A31,ADC_ACC4);
    #endif
    #ifdef ADC_ACC8
      trashbag = analogReadEnh(PIN_A31,ADC_ACC8);
    #endif
    #ifdef ADC_ACC16
      trashbag = analogReadEnh(PIN_A31,ADC_ACC16);
    #endif
    #ifdef ADC_ACC32
      trashbag = analogReadEnh(PIN_A31,ADC_ACC32);
    #endif
    #ifdef ADC_ACC64
      trashbag = analogReadEnh(PIN_A31,ADC_ACC64);
    #endif
    #ifdef ADC_ACC128
      trashbag = analogReadEnh(PIN_A31,ADC_ACC128);
    #endif
    #ifdef ADC_ACC256
      trashbag = analogReadEnh(PIN_A31,ADC_ACC256);
    #endif
    #ifdef ADC_ACC512
      trashbag = analogReadEnh(PIN_A31,ADC_ACC512);
    #endif
    #ifdef ADC_ACC1024
      trashbag = analogReadEnh(PIN_A31,ADC_ACC1024);
    #endif
    #if defined(ADC_DIFFERENTIAL) && ADC_DIFFERENTIAL==1
      trashbag = analogReadDiff(PIN_A31,ADC_GROUND,ADC_NATIVE_RESOLUTION);
    #endif
  #endif
}
