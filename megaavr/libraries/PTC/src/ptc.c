/*
 * Refer to ptc_touch.h file for copyright, changelog, usage and license information
 */


#include "ptc.h"



// The following functions are used internally only.
// get the pointer of the last valid node struct can be "firstNode"
cap_sensor_t* ptc_get_last_node(void);


uint8_t ptc_append_node(cap_sensor_t* pNewNode);

// Prepares the ADC/PTC registers for the next conversion batch, starts conversion with firstNode
void ptc_init_conversion(uint8_t nodeType);

// Strts the conversion of the node that is passed as argument
void ptc_start_conversion (cap_sensor_t* node);

// State-Machine Handler, interprets the measured values and decides if key is "pressed"
void ptc_process_node_sm (cap_sensor_t* node);

// Helper function
void ptc_process_measurement (cap_sensor_t* node);

// Handles (initial) calibration
uint8_t ptc_process_calibrate (cap_sensor_t* node);

// Handles adjustment of the reference value when a button is not pressed
void ptc_process_adjust ();

void ptc_set_registers(cap_sensor_t* node);


cap_sensor_t *firstNode = NULL;
cap_sensor_t *lowPowerNode = NULL;

volatile cap_sensor_t *currConvNode = NULL;

ptc_node_type_t currConvType = 0;   // does not "remember" if type was low-power
ptc_node_type_t nextConvType = 0;

uint16_t acqPeriod = 20;    // Period in ms until a new acquision is started
uint16_t lastAcqTime = 0;   // millis value of last Acqusition (16-bit)

ptc_freq_t freq_select = 0;  /* FREQ_SEL_0 to FREQ_SEL_15, FREQ_SEL_SPREAD -> CTRLD */

volatile ptc_lib_t ptc_lib_state = PTC_LIB_IDLE;


/*
 * Global settings for the state-machine applied on every node.
 * Pointer to this struct can be retrieved by ptc_get_sm_settings()
 *
 */
ptc_lib_sm_set_t ptc_sm_settings = {
  .force_recal_delta = 150,
  .touched_detect_nom = 3,
  .untouched_detect_nom = 3,
  .touched_max_nom = 200,
  .drift_up_nom = 15,
  .drift_down_nom = 15
};

ptc_lib_sm_set_t* ptc_get_sm_settings() {
  return &ptc_sm_settings;
}

// Analog gain Values, extracted with a debugger

const uint8_t ptc_a_gain_lut[] = {
  0x3F, 0x1C, 0x0B,
  0x05, 0x03, 0x01,
};





//#if (NUM_TOTAL_PINS > 32)
//uint16_t ptc_touch_stack = 0;
//#else
//uint8_t ptc_touch_stack = 0;
//#endif

// Workarounds to make the code work with DAs... Argh.
#if defined (__PTC_DA__)
  #ifndef ADC_SAMPNUM_ACC1_gc
    #define ADC_SAMPNUM_ACC1_gc ADC_SAMPNUM_NONE_gc
  #endif
  #ifndef ADC_REFSEL_VDDREF_gc
    #define ADC_REFSEL_VDDREF_gc 0x00
  #endif
#endif

#if defined (__PTC_Tiny__)
  #define PTC_DEFAULT_SC_CC 0x0567
  #define PTC_DEFAULT_MC_CC 0x0234
#elif defined (__PTC_DA__)
  #define PTC_DEFAULT_SC_CC 0x00F0
  #define PTC_DEFAULT_MC_CC 0x00A0
#endif
/*
 * Different Functions that change the behaviour of the supplied node
 */
__attribute__ ((weak))
void ptc_event_callback(const ptc_cb_event_t eventType, cap_sensor_t* node)  {
  (void)eventType;
  (void)node;
}
__attribute__ ((weak, alias("ptc_event_callback"))) void ptc_event_cb_touch(const ptc_cb_event_t eventType, cap_sensor_t* node);
__attribute__ ((weak, alias("ptc_event_callback"))) void ptc_event_cb_wake(const ptc_cb_event_t eventType, cap_sensor_t* node);
__attribute__ ((weak, alias("ptc_event_callback"))) void ptc_event_cb_conversion(const ptc_cb_event_t eventType, cap_sensor_t* node);
__attribute__ ((weak, alias("ptc_event_callback"))) void ptc_event_cb_calibration(const ptc_cb_event_t eventType, cap_sensor_t* node);
__attribute__ ((weak, alias("ptc_event_callback"))) void ptc_event_cb_error(const ptc_cb_event_t eventType, cap_sensor_t* node);



// Set the threshold for touch detection and away from touch for a node
uint8_t ptc_node_set_thresholds (cap_sensor_t* node, int16_t th_in, int16_t th_out) {
  if (NULL == node)
    return PTC_LIB_BAD_POINTER;

  node->touch_in_th = th_in;
  node->touch_out_th = th_out;
  return PTC_LIB_SUCCESS;
}


// Change Resistor Setting. Note: Only has an effect on mutual sensors
uint8_t ptc_node_set_resistor(cap_sensor_t* node, uint8_t res) {
  PTC_CHECK_FOR_BAD_POINTER(node);

  if (res > RSEL_MAX)
    return PTC_LIB_BAD_ARGUMENT;


  if (node->type & NODE_MUTUAL_bm) {
    uint8_t presc = node->hw_rsel_presc & 0x0F;
    presc |= ((res & 0x0F) << 4);
    node->hw_rsel_presc = presc;
    return PTC_LIB_SUCCESS;
  }

  return PTC_LIB_BAD_ARGUMENT;
}


// Change preschaler. Recomended ADC frequency: < 1.5MHz, but max 3 factors below
uint8_t ptc_node_set_prescaler(cap_sensor_t* node, uint8_t presc) {
  PTC_CHECK_FOR_BAD_POINTER(node);

  if ((presc > (PTC_PRESC_DEFAULT + 2)) || (presc < PTC_PRESC_DEFAULT))
    return PTC_LIB_BAD_ARGUMENT;

  uint8_t res = node->hw_rsel_presc & 0xF0;
  res |= (presc & 0x0F);
  node->hw_rsel_presc = res;
  return PTC_LIB_SUCCESS;
}


uint8_t ptc_node_set_gain(cap_sensor_t* node, uint8_t aGain, uint8_t dGain) {
  PTC_CHECK_FOR_BAD_POINTER(node);
  if (aGain > 0x05) {
    if (__builtin_constant_p(aGain))
      badArg("Analog Gain too high. Max Analog Gain Value is 0x05 (equals 32x)");
    return PTC_LIB_BAD_ARGUMENT;
  }

  if (dGain > 0x06) {
    if (__builtin_constant_p(dGain))
      badArg("Digital Gain too high. Max Digital Gain Value is 0x06 (equals 64x)");
    return PTC_LIB_BAD_ARGUMENT;
  }
  node->hw_a_d_gain = NODE_GAIN(aGain, dGain);
  return PTC_LIB_SUCCESS;
}



/*
 *  Two functions to suspend and resume of the normal PTC operation, however,
 *  this functions will only work when no acqusition is in progress. Neither
 *  when the library is in low-power mode. This is due to the fact that the
 *  low-power mode relies on the registers being set once, at the low-power
 *  initialization. Thus, the user has to disable LP mode, then suspend, resume
 *  and then re-init low-power mode.
 */
uint8_t ptc_suspend(void) {
  if (PTC_LIB_IDLE == ptc_lib_state) { // allow disabling only outside conversions
    ptc_lib_state = PTC_LIB_SUSPENDED;
    #if defined (__PTC_Tiny__)
      PTC.CTRLP = 0x00;
    #elif defined (__PTC_DA__)
      PTC.CTRLA = 0x00;
    #endif
    return PTC_LIB_SUCCESS;
  }
  return PTC_LIB_WRONG_STATE;
}

void ptc_resume(void) {
  if (PTC_LIB_SUSPENDED == ptc_lib_state) {
    ptc_lib_state = PTC_LIB_IDLE;
  }
}


// own implementation of mTC's initADC0(), as the one that comes with the core does not overwrite
// sample number (CTRLB)
void ptc_init_ADC0(void) {
  PTC_t* pPTC;
  _fastPtr_d(pPTC, &PTC);
  #if defined (__PTC_Tiny__)
    #if   F_CPU   > 24000000
      pPTC->CTRLC  = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >= 12000000
      pPTC->CTRLC  = ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  6000000
      pPTC->CTRLC  =  ADC_PRESC_DIV8_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  3000000
      pPTC->CTRLC  =  ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #else
      pPTC->CTRLC  =  ADC_PRESC_DIV2_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #endif
    #if   (F_CPU == 6000000 || F_CPU == 12000000 || F_CPU == 24000000 || F_CPU ==25000000)
      pPTC->SAMPCTRL = (7);
    #elif (F_CPU == 5000000 || F_CPU == 10000000 || F_CPU == 20000000)
      pPTC->SAMPCTRL = (13);
    #else
      pPTC->SAMPCTRL = (10);
    #endif
    pPTC->CTRLD    = ADC_INITDLY_DLY16_gc;
    pPTC->CTRLB    = ADC_SAMPNUM_ACC1_gc;
    pPTC->CTRLA    = ADC_ENABLE_bm;
  #elif defined (__PTC_DA__)

  #endif
}


// a helper function to reduce copy-pasting between self and mutual initialisations
void ptc_add_node_common(cap_sensor_t* node, ptc_ch_bm_t yCh, ptc_ch_bm_t xCh);

// if xCh > 0, with shield, otherwise usual selfcap
uint8_t ptc_add_selfcap_node_asserted(cap_sensor_t* node, const ptc_ch_bm_t yCh, const ptc_ch_bm_t xCh) {
  PTC_CHECK_POINTER(node, PTC_LIB_BAD_POINTER); // check not in h file as gcc doesn't know about the memory address

  if (ptc_append_node(node) != PTC_LIB_SUCCESS)
    return PTC_LIB_BAD_POINTER;

  ptc_add_node_common(node, yCh, xCh);

  if (xCh > 0)  node->type = NODE_SELFCAP_SHIELD_bm;
  else          node->type = NODE_SELFCAP_bm;


  node->touch_in_th = 20;
  node->touch_out_th = 10;
  node->hw_compCaps = PTC_DEFAULT_SC_CC;  /* value from official library */
  node->hw_rsel_presc = NODE_RSEL_PRSC(RSEL_VAL_0, PTC_PRESC_DEFAULT);

  return PTC_LIB_SUCCESS;
}


uint8_t ptc_add_mutualcap_node_asserted(cap_sensor_t* node, ptc_ch_bm_t yCh, ptc_ch_bm_t xCh) {
  PTC_CHECK_POINTER(node, PTC_LIB_BAD_POINTER); // check not in h file as gcc doesn't know about the memory address

  if (ptc_append_node(node) != PTC_LIB_SUCCESS)
    return PTC_LIB_BAD_POINTER;

  ptc_add_node_common(node, yCh, xCh);

  node->type = NODE_MUTUAL_bm;

  node->touch_in_th = 10;
  node->touch_out_th = 5;
  node->hw_compCaps = PTC_DEFAULT_MC_CC;  /* value from official library */
  node->hw_rsel_presc = NODE_RSEL_PRSC(RSEL_VAL_100, PTC_PRESC_DEFAULT);

  return PTC_LIB_SUCCESS;
}

void ptc_add_node_common(cap_sensor_t* node, ptc_ch_bm_t yCh, ptc_ch_bm_t xCh) {

  node->stateMachine = PTC_SM_NOINIT_CAL;
  node->hw_xCh_bm = xCh;
  node->hw_yCh_bm = yCh;
  node->hw_a_d_gain = NODE_GAIN(ADC_SAMPNUM_ACC1_gc, ADC_SAMPNUM_ACC16_gc);

  #if defined (__PTC_Tiny__)
    uint8_t* src = (uint8_t*)&PTC.PIN_OVR;  // Get "addresses" of relevant elements
    uint8_t* orY = (uint8_t*)&yCh;          // This allows to split the uint64_t to an access of one byte at a time.
    uint8_t* orX = (uint8_t*)&xCh;          // on AtTinies, the compiler unrolls the loop to two iterations
    uint8_t bit_pos  = 0;   // counts from 0 to "highest PTC pin number"
    for (uint8_t i = 0; i < sizeof(ptc_ch_bm_t); i++) {
      // this equals PTC.PIN_OVR |= (xCh | yCh);, but more friendly for CPU-registers on DAs with uint64_t
      uint8_t temp_bm = *(orY++) | *(orX++);
      *(src) |= temp_bm;
      src++;

      for (uint8_t j = 0; j < 8; j++) {
        if (temp_bm & 0x01) {
          if (bit_pos < sizeof(ptc_ch_to_pin)) {   // avoid accesses outside of the array, which can be smaller then the bit-map
            uint8_t offset = ptc_ch_to_pin[bit_pos];
            if (offset != 0) {
              uint16_t basePointer = (uint16_t)&PORTA;
              uint8_t* portSettings = (uint8_t*)(basePointer + offset);
              (*portSettings) = PORT_ISC_INPUT_DISABLE_gc;
            }
          }
        }
        bit_pos++;
        temp_bm >>= 1;
      }
    }
  #elif defined (__PTC_DA__)
// Not started yet
  #endif
}



uint8_t ptc_enable_node(cap_sensor_t* node) {
  PTC_CHECK_FOR_BAD_POINTER(node);

  node->state.disabled = 0;
  return PTC_LIB_SUCCESS;
}

// Will finish conversion, but not start a new one
uint8_t ptc_disable_node(cap_sensor_t* node) {
  PTC_CHECK_FOR_BAD_POINTER(node);

  node->state.disabled = 1;
  return PTC_LIB_SUCCESS;
}



// Make the ADC wake up the CPU after the next conversion to allow a drift
uint8_t ptc_lp_force_drift(void) {
  if (NULL == lowPowerNode)
    return PTC_LIB_WRONG_STATE;

  if (PTC.INTCTRL == ADC_WCMP_bm) { // only do something when we are waiting for a value over the threshold
    PTC.INTCTRL = (ADC_RESRDY_bm | ADC_WCMP_bm);
  }

  return PTC_LIB_SUCCESS;
}




// select node type to convert next (Selfcap, Shield, Mutual)
// when not specified, the type of the "firstNode" is always used
void ptc_set_next_conversion_type(ptc_node_type_t type) {
  nextConvType = type;
}

// ev_ch: bitmask of required channels to be used for ADC0/PTC, e.g. channel 2 and 5 -> 0x24
// this would allow to periodically start ADC0/PIT by a timer without sleep
uint8_t ptc_lp_init(cap_sensor_t* node) {
  PTC_CHECK_FOR_BAD_POINTER(node);

  if (NULL != lowPowerNode)
    return PTC_LIB_WRONG_STATE;

  node->state.low_power = 1;
  node->stateMachine = PTC_SM_LOW_POWER;

  lowPowerNode = node;

  ptc_init_conversion(node->type);

  return PTC_LIB_SUCCESS;
}

uint8_t ptc_lp_disable(void) {
  if (NULL == lowPowerNode)
    return PTC_LIB_WRONG_STATE;

  PTC.INTCTRL = 0;
  PTC.EVCTRL = 0;

  lowPowerNode->state.low_power = 0;
  lowPowerNode->state.win_comp = 0;

  return PTC_LIB_SUCCESS;
}

// use this in a conversion complete callback to see if the window comparator was triggered
uint8_t ptc_lp_was_waken(void) {
  if (NULL == lowPowerNode)
    return PTC_LIB_WRONG_STATE;

  if (1 == lowPowerNode->state.win_comp)
    return PTC_LIB_WAS_WAKEN;
  else
    return PTC_LIB_ASLEEP;

  return PTC_LIB_ERROR;
}



/* not recommended to use, as the calculation is bloated */
uint16_t ptc_get_node_cc_femto (cap_sensor_t* node) {
  if (NULL == node)
    return 0;

  uint16_t retVal = 0;
  uint16_t comp = node->hw_compCaps;
  for (uint8_t i = 0; i < 3; i++) {
    retVal /= 10; /* "skips" last addition */
    uint8_t temp = comp & 0x0F;
    retVal += (temp * 675);
    comp >>= 4;   /* select next field */
  }
  retVal += (comp & 0x03) * 6750;
  comp >>= 2;
  retVal += (comp & 0x03) * 6200;
  return retVal;  //max 51000
}


// pass the time, e.g. return value of millis to the function instead of hving a
// millis in here. improves portability
void ptc_process (uint16_t currTime) {
  if ((PTC_LIB_CONV_WCMP | PTC_LIB_CONV_LP) & ptc_lib_state) {
    if (NULL != lowPowerNode) {
      if (PTC_LIB_CONV_WCMP == ptc_lib_state) {
        ptc_event_cb_wake(PTC_CB_EVENT_WAKE_TOUCH, lowPowerNode);
      } else {
        ptc_event_cb_wake(PTC_CB_EVENT_WAKE_NO_TOUCH, lowPowerNode);
      }
      ptc_process_measurement(lowPowerNode);
      if (NULL == lowPowerNode) { // Due to callbacks, low Power node may have been set to NULL by a disable
        currConvType = NODE_TYPE_NOCONV_bm;
        ptc_lib_state = PTC_LIB_IDLE;
      } else {
        PTC.INTCTRL = ADC_WCMP_bm;
        ptc_lib_state = PTC_LIB_EVENT;
      }
    }
  } else if (PTC_LIB_CONV_COMPL == ptc_lib_state) {
    ptc_lib_state = PTC_LIB_IDLE;
    for (cap_sensor_t* node = firstNode; node != NULL; node = node->nextNode) {
      ptc_process_measurement(node);
    }
    ptc_event_cb_conversion((PTC_CB_EVENT_CONV_CMPL | currConvType), (cap_sensor_t *)currConvNode);
    currConvType = NODE_TYPE_NOCONV_bm;
  }

  if ((currTime - lastAcqTime) >= acqPeriod) {
    lastAcqTime += acqPeriod;
    if (PTC_LIB_IDLE == ptc_lib_state) {
      uint8_t convType;
      if (nextConvType == 0) {
        convType = firstNode->type;
      } else {
        convType = nextConvType;
      }
      ptc_init_conversion(convType);
    }
  }
}



/*
 * Internal functions only below. Not part of the API
 */
void ptc_process_measurement (cap_sensor_t* node) {
  if (node->state.error)
    return;

  if (node->state.data_ready != 0) {
    node->state.data_ready = 0;
    ptc_process_node_sm(node);
    ptc_event_cb_conversion(PTC_CB_EVENT_CONV_CMPL, node);
  }
}


void ptc_process_node_sm (cap_sensor_t* node) {
  uint16_t nodeData   = node->sensorData;
  uint8_t  nodeSM     = node->stateMachine;
  uint8_t  lastChange = node->lastStateChange;
  uint16_t reference  = node->reference;
   int16_t nodeDelta  = nodeData - reference;

  if (nodeSM == PTC_SM_NOINIT_CAL) {         /* Beginning here */
    uint8_t retVal = ptc_process_calibrate(node);
    if (PTC_LIB_CALIB_DONE == retVal) {
      node->reference = nodeData;
      if (node->state.low_power) {
        nodeSM = PTC_SM_LOW_POWER;
      } else {
        nodeSM = PTC_SM_NO_TOUCH;
      }
      ptc_event_cb_calibration(PTC_CB_EVENT_CONV_CALIB, node);
    } else if (PTC_LIB_SUCCESS != retVal || lastChange > 10) {  // if we stay here for over 10 conversions, something didn't went right
      node->state.error = 1;
      if (PTC_LIB_CALIB_TOO_LOW == retVal) {
        ptc_event_cb_calibration(PTC_CB_EVENT_ERR_CALIB_LOW, node);
      } else if (PTC_LIB_CALIB_TOO_HIGH == retVal) {
        ptc_event_cb_calibration(PTC_CB_EVENT_ERR_CALIB_HIGH, node);
      } else {
        ptc_event_cb_calibration(PTC_CB_EVENT_ERR_CALIB_TO, node);
      }
      return;
    } else if (PTC_LIB_SUCCESS == retVal) {
      // calibration didn't finish (yet), but didn't threw an error
    }

  } else if (nodeSM & PTC_SM_RECAL_FLT) {   /* if the delta was really low, we might have to recalibrate the caps */
    if (nodeDelta > -150) {
      nodeSM = PTC_SM_NO_TOUCH;
    } else if (lastChange > 3) {
      nodeSM = PTC_SM_NOINIT_CAL;
    }


  } else if (nodeSM & PTC_SM_NT_LOW_FLT) {   /* no touch, delta below -1, threshold drift */
    if (nodeDelta > 0) {
      nodeSM = PTC_SM_NO_TOUCH;
    } else if (nodeDelta < -150) {
      nodeSM = PTC_SM_RECAL_FLT;
    } else if (lastChange > ptc_sm_settings.drift_down_nom) {  /* By using '>' the drift can be disabled with integer overflow */
      node->reference--;
      nodeSM = PTC_SM_NO_TOUCH;
    }


  } else if (nodeSM & PTC_SM_NO_TOUCH) {   /* default State, no touch */
    if (nodeDelta < -150) {                 /* if the touch value is way too small, something chnaged with the lines, recal */
      nodeSM = PTC_SM_RECAL_FLT;
    } else if (nodeDelta >= node->touch_in_th) {
      nodeSM = PTC_SM_TOUCH_IN_FLT;
    } else if (reference <= (512 - ptc_sm_settings.force_recal_delta)) {
      nodeSM = PTC_SM_NOINIT_CAL;
    } else if (reference >= (512 + ptc_sm_settings.force_recal_delta)) {
      nodeSM = PTC_SM_NOINIT_CAL;
    } else if (nodeDelta < -1) {
      nodeSM = PTC_SM_NT_LOW_FLT;
    } else if (nodeDelta > 1) {
      nodeSM = PTC_SM_NT_HIGH_FLT;
    }


  } else if (nodeSM & PTC_SM_NT_HIGH_FLT) {
    if (nodeDelta < 0) {
      nodeSM = PTC_SM_NO_TOUCH;
    } else if (nodeDelta >= node->touch_in_th) {
      nodeSM = PTC_SM_TOUCH_IN_FLT;
    } else if (lastChange > ptc_sm_settings.drift_up_nom) {
      node->reference++;
      nodeSM = PTC_SM_NO_TOUCH;
    }


  } else if (nodeSM & PTC_SM_TOUCH_IN_FLT) {
    if (lastChange >= ptc_sm_settings.touched_detect_nom) {
      nodeSM = PTC_SM_TOUCH_DETECT;
      ptc_event_cb_touch(PTC_CB_EVENT_TOUCH_DETECT, node);
    } else if (nodeDelta < node->touch_in_th) {
      nodeSM = PTC_SM_NO_TOUCH;
    }


  } else if (nodeSM & PTC_SM_TOUCH_OUT_FLT) {
    if (lastChange >= ptc_sm_settings.untouched_detect_nom) {
      nodeSM = PTC_SM_NO_TOUCH;
      ptc_event_cb_touch(PTC_CB_EVENT_TOUCH_RELEASE, node);
    } else if (nodeDelta > node->touch_in_th) {
      nodeSM = PTC_SM_TOUCH_DETECT;
    }


  } else if (nodeSM & PTC_SM_TOUCH_DETECT) {
    if (lastChange > ptc_sm_settings.touched_max_nom) {
      nodeSM = PTC_SM_NOINIT_CAL;
    } else if (nodeDelta < node->touch_out_th) {
      nodeSM = PTC_SM_TOUCH_OUT_FLT;
    }


  } else if (nodeSM & PTC_SM_LOW_POWER) {
    if (0 == node->state.low_power) {
      if (nodeDelta > node->touch_in_th) {
        nodeSM = PTC_SM_TOUCH_DETECT;
      } else {
        nodeSM = PTC_SM_NO_TOUCH;
      }
      lowPowerNode = NULL;
    }
    if (nodeDelta < -1) {
      node->reference--;
      PTC.WINHT--;
    } else if (nodeDelta > 1) {
      node->reference++;
      PTC.WINHT++;
    }


  }


  if (node->stateMachine != nodeSM) {
    node->lastStateChange = 0;
    node->stateMachine = nodeSM;
  } else {
    node->lastStateChange = lastChange + 1;
  }
}


uint8_t ptc_process_calibrate (cap_sensor_t* node) {
  uint16_t rawData = node->sensorData;

  #if defined(__PTC_Tiny__)
    uint16_t compensation = node->hw_compCaps;
    uint8_t cc_accurate =             compensation        & 0x0F;
    uint8_t cc_fine =      ((uint8_t) compensation >> 4)  & 0x0F;
    uint8_t cc_coarse =     (uint8_t)(compensation >> 8)  & 0x0F;
    uint8_t cc_add_rough =  (uint8_t)(compensation >> 8)  & 0xC0;
    uint8_t cc_rough  =     (uint8_t)(compensation >> 12) & 0x03;

    //uint8_t err = 0;

    int8_t dirOvf;
    int8_t dir;
    if (rawData > 0x0200) {
      rawData = rawData - 0x1FF;
      if (rawData < 10) {
        return PTC_LIB_CALIB_DONE;
      }

      if (node->type & NODE_MUTUAL_bm){
        dir = -1;
        dirOvf = -6;
        rawData /= 2;
      } else {
        dir = 1;
        dirOvf = 6;
        if (rawData > 0x0100) {
          if (cc_add_rough <= 0xC0) {
            cc_add_rough = (cc_add_rough + 0x40);
            rawData -= 0xF0;
          }
        }
      }
    } else {
      rawData = 0x1FF - rawData;
      if (rawData < 10) {
        return PTC_LIB_CALIB_DONE;
      }

      if (node->type & NODE_MUTUAL_bm){
        dir = 1;
        dirOvf = 6;
        rawData /= 2;
      } else {
        dir = -1;
        dirOvf = -6;
        if (rawData > 0x0100) {
          if (cc_add_rough >= 0x40) {
            cc_add_rough = (cc_add_rough - 0x40);
            rawData -= 0xF0;
          }
        }
      }
    }

    while (rawData > 0x0001) {
      while (rawData > 0x00CF) {
        cc_rough += dir;            // this algorithm takes advantage of integer underflow
        if (cc_rough > 0x03) {      // by checking against >0x03, we can also check for 0xFF aka -1
          cc_rough -= dir;          // thus saving some flash. +/-1 can be made in one insn
          break;                    // by using sub reg with 0xFF or 0x01 in a reg respectively
        }
        rawData -= 0xCF;
      }

      while (rawData > 0x0015) {
        cc_coarse += dir;
        if (cc_coarse > 0x0F)
          break;
        rawData -= 0x15;
      }
      if (cc_coarse > 0x0F) {
        cc_rough += dir;
        if (cc_rough > 0x03) {
          cc_rough -= dir;
          cc_coarse -= dir;
        } else {
          cc_coarse -= dirOvf;
        }
      }


      while (rawData > 0x0001) {
        cc_fine += dir;
        if (cc_fine > 0x0F)
          break;
        rawData -= 0x02;
      }

      if (cc_fine > 0x0F) {
        cc_coarse += dir;
        if (cc_coarse > 0x0F) {
          cc_rough += dir;
          if (cc_rough > 0x03) {
            cc_rough -= dir;
            cc_coarse -= dir;
            cc_fine -= dir;
            if (dir < 0)
              return PTC_LIB_CALIB_TOO_LOW;
            else
              return PTC_LIB_CALIB_TOO_HIGH;
          } else {
            cc_coarse -= dirOvf;
          }
        } else {
          cc_fine -= dirOvf;
        }
      } /* if (cc_fine > 0x0F) */
    } /* (rawData > 0x0001) */


    cc_fine <<= 4;
    cc_rough <<= 4;
    cc_rough |= cc_add_rough;
    cc_rough |= cc_coarse;
    cc_fine  |= cc_accurate;
    node->hw_compCaps = (uint16_t)((cc_rough << 8) | cc_fine);
    return PTC_LIB_SUCCESS;

  #elif defined (__PTC_DA__)
    int8_t dir;
    if (rawData > 0x0200) {
      rawData = rawData - 0x1FF;
      if (rawData < 5) {
        return PTC_LIB_CALIB_DONE;
      }

      if (node->type & NODE_MUTUAL_bm) {
        dir = -1;
        rawData /= 2;
      } else {
        dir = 1;
      }
    } else {
      rawData = 0x1FF - rawData;
      if (rawData < 5) {
        return PTC_LIB_CALIB_DONE;
      }

      if (node->type & NODE_MUTUAL_bm) {
        dir = 1;
        rawData /= 2;
      } else {
        dir = -1;
      }
    }
    node->hw_compCaps += (int16_t)(dir * rawData);
  #endif
}


void ptc_init_conversion(uint8_t nodeType) {
  PTC_t *pPTC;
  _fastPtr_d(pPTC,&PTC);

  if (ptc_lib_state != PTC_LIB_IDLE)
    return;

#if defined (__PTC_Tiny__)
  pPTC->CTRLP = 0x00;
  if (nodeType == NODE_MUTUAL_bm) {  /* NODE_MUTUAL */
    pPTC->CTRLP    = 0xC0 | 0x20;
    pPTC->SHIELD  = 0x00;
  } else if (nodeType == NODE_SELFCAP_bm){  /* NODE_SELFCAP */
    pPTC->CTRLP  = 0x28;
    pPTC->SHIELD = 0x00;
  } else if (nodeType == NODE_SELFCAP_SHIELD_bm) {
    pPTC->CTRLP  = 0x28;
    pPTC->SHIELD = 0x86;
  } else {
    return;
  }

  pPTC->CTRLA = 0x00;

  uint8_t freq = freq_select;
  if (freq > 0x0F) /* FREQ_SELECT_15 */
    freq = ADC_ASDV_bm;
  pPTC->CTRLD = freq;
  pPTC->INTFLAGS = ADC_RESRDY_bm | ADC_WCMP_bm; // clear ISR flags, if there were unhandled

  currConvType = nodeType;
  if (NULL == lowPowerNode) {
    pPTC->INTCTRL = ADC_RESRDY_bm;
    pPTC->CTRLE = ADC_WINCM_NONE_gc;
    ptc_lib_state = PTC_LIB_CONV_PROG;
    ptc_start_conversion(firstNode);
  } else {
    pPTC->INTCTRL = ADC_WCMP_bm;  // Wakeup only above of window
    pPTC->CTRLE = ADC_WINCM_ABOVE_gc;
    pPTC->WINHT = (lowPowerNode->reference + lowPowerNode->touch_in_th) << (lowPowerNode->hw_a_d_gain & 0x0F);
    ptc_lib_state = PTC_LIB_EVENT;
    ptc_start_conversion(lowPowerNode);
  }

#elif defined (__PTC_DA__)
// The following code is a placeholder, DA not really started yet
  if (nodeType == NODE_MUTUAL_bm) {
    pPTC->CTRL_SC = 0x00;
    //PTC.CTRL_BOOST = 0x00;
    pPTC->CTRLA = 0x00;
  } else if (nodeType == NODE_SELFCAP_bm || nodeType == NODE_SELFCAP_SHIELD_bm) {
    pPTC->CTRL_SC = 0x01;
    //PTC.CTRL_BOOST = 0x00;
    pPTC->CTRLA = 0x00;
  //} else if (nodeType == 0x82) {  // unused
  //  PTC.CTRL_SC = 0x01;
  //  PTC.CTRL_BOOST = 0x09;
  //  PTC.CTRLA = 0x00;
  } else {
    return;
  }


  uint8_t freq = freq_select;
  if (nodeType == NODE_MUTUAL_bm) {
    pPTC->SAMPDLY = 0x00;
    if (freq < 0x10) /* freq is 0 - 0x0F, 0x10 is auto variation */
      freq |= ADC_INITDLY_DLY16_gc;
    else
      freq = ADC_INITDLY_DLY16_gc | ADC_SAMPDLY_DLY15_gc;
    pPTC->CTRLD = freq;
  } else {
    pPTC->CTRLD = ADC_INITDLY_DLY16_gc;
    if (freq > 0x0F)
      freq = 0x0F;
    pPTC->SAMPDLY = freq;
  }

  pPTC->INTFLAGS = ADC_RESRDY_bm | ADC_WCMP_bm; // clear ISR flags, if there were unhandled

  currConvType = nodeType;
  if (NULL != lowPowerNode) {
    pPTC->INTCTRL = ADC_WCMP_bm;  // Wakeup only above of window
    pPTC->CTRLE = ADC_WINCM_ABOVE_gc;
    pPTC->WINHT = (lowPowerNode->reference + lowPowerNode->touch_in_th) << (lowPowerNode->hw_a_d_gain & 0x0F);
    ptc_lib_state = PTC_LIB_EVENT;
    ptc_start_conversion(lowPowerNode);
  } else {
    pPTC->INTCTRL = ADC_RESRDY_bm;
    pPTC->CTRLE = ADC_WINCM_NONE_gc;
    ptc_lib_state = PTC_LIB_CONV_PROG;
    ptc_start_conversion(firstNode);
  }
#endif
}

void ptc_start_conversion (cap_sensor_t* node) {
  while (1) {
    if (NULL == node) {
      PTC.INTCTRL = 0;    // disable ISR for ADC reuse
      ptc_lib_state = PTC_LIB_CONV_COMPL;
      return;
    }
    if ((0 == node->state.disabled) && (node->type == currConvType)) {
      break;
    } else {
      node = node->nextNode;
    }
  }

  currConvNode = node;

  ptc_set_registers(node);
}

void ptc_set_registers(cap_sensor_t* node) {
  PTC_t *pPTC;
  _fastPtr_d(node,node);  // Sometimes it takes the compiler a bit more of convincing...
  _fastPtr_d(pPTC,&PTC);

  if (NULL == node)
    return;

  uint8_t analogGain = 0x3F;
  if ((node->state.disabled == 0) && (node->stateMachine != PTC_SM_NOINIT_CAL)) {
    uint8_t lut_index = node->hw_a_d_gain / 16;  // A little workaround as >> 4 is kinda broken sometimes.
    analogGain = ptc_a_gain_lut[lut_index];
  }

  uint8_t chargeDelay = node->hw_csd;
  if (chargeDelay < 0x1B) {
    chargeDelay += 4;
  } else {
    chargeDelay = 0x1F;
  }

  #if defined (__PTC_Tiny__)
    pPTC->YBM  = node->hw_yCh_bm;
    pPTC->XBM  = node->hw_xCh_bm;
    pPTC->COMP = node->hw_compCaps;
    pPTC->AGAIN = analogGain;
    pPTC->SAMPCTRL = chargeDelay;
    pPTC->CTRLB = node->hw_a_d_gain & 0x0F;
    pPTC->RES = node->hw_rsel_presc / 16;
    pPTC->CTRLC = (node->hw_rsel_presc & 0x0F) | ADC_REFSEL_VDDREF_gc;
    pPTC->CTRLP |= 0x03;
    pPTC->CTRLA = ADC_RUNSTBY_bm | ADC_ENABLE_bm; /* 0x81 */

    if (0 == node->state.low_power)
      pPTC->COMMAND = 0x01; // Normal opertion: Manual Start
    else
      pPTC->EVCTRL = 0x01;  // Low Power: Start by positive Flank on Event
  #elif defined (__PTC_DA__)

  #endif
}

void ptc_eoc(void) {
  PTC_t *pPTC;
  volatile cap_sensor_t *pCurrentNode;  // volatile nedded to pass type check
  _fastPtr_d(pPTC,&PTC);
  _fastPtr_d(pCurrentNode,currConvNode);

  if (NULL == pCurrentNode)
    return;

  pPTC->CTRLA = 0x00;
  uint8_t flags = pPTC->INTFLAGS; // save the flags before they get cleared by RES read
  uint16_t rawVal =  pPTC->RES;   // clears ISR flags
  uint8_t oversampling = pCurrentNode->hw_a_d_gain  & 0x0F;
  pCurrentNode->sensorData = rawVal >> oversampling;

  //currConvNode->sensorData = pPTC->RES_TRUE;
  pCurrentNode->state.data_ready = 1;

  if (pCurrentNode->state.low_power) {
    if (flags & ADC_WCMP_bm) {
      pCurrentNode->state.win_comp = 1;
      ptc_lib_state = PTC_LIB_CONV_WCMP;
    } else {
      pCurrentNode->state.win_comp = 0;
      ptc_lib_state = PTC_LIB_CONV_LP;
    }

  } else {
    ptc_start_conversion(pCurrentNode->nextNode);
  }
}



// returns the last node in the list, or NULL if list empty.
cap_sensor_t* ptc_get_last_node (void) {
  if (firstNode == NULL)
    return NULL;

  cap_sensor_t *node = firstNode;
  while (1) {
    cap_sensor_t *nextNode = node->nextNode;
    if (nextNode == NULL)
      return node;
    else
      node = nextNode;
  }
}


// puts the node to the back of the single-linked list.
// returns NULL if list was empty, otherwise the previous last node
uint8_t ptc_append_node(cap_sensor_t* pNewNode) {
  cap_sensor_t* lastNode = ptc_get_last_node();
  if (lastNode == NULL) {
    firstNode = pNewNode;
    pNewNode->id = 0;
  } else if (lastNode != pNewNode) {  // Make sure we don't create an endless loop on accident
    lastNode->nextNode = pNewNode;
    pNewNode->id = lastNode->id + 1;
  } else {
    return PTC_LIB_ERROR;
  }
  return PTC_LIB_SUCCESS;
}


#if defined(__PTC_Tiny__)
ISR(ADC0_RESRDY_vect) {
  ptc_eoc();
}

ISR(ADC0_WCOMP_vect) {
  ptc_eoc();
}
#elif defined(__PTC_DA__)
ISR(PTC_PTC_vect) {
  ptc_eoc();
}
#endif
