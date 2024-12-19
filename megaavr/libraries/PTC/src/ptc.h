/*
  library to use the PTC module in AVR devices
  Copyright (c) 2023, MX682X

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  This license applies to all files that are part of this library
  (ptc.c, ptc.h, ptc_io.h, ptc_types.h)
*/



#pragma once
#ifndef PTC_H
#define PTC_H

#if defined(MEGATINYCORE) || defined(DXCORE)
  #include <Arduino.h>
#else
  #include <avr/interrupt.h>
  #ifndef _fastPtr_d
    #define _fastPtr_d(_x_, _y_) _x_ = _y_;
  #endif
#endif

#include "ptc_types.h"
#include "ptc_io.h"


#ifndef NULL
  #define NULL  (void*)0
#endif


#ifdef __cplusplus
extern "C" {
#endif


#define NODE_GAIN(_a_, _d_)      (uint8_t)(((_a_) << 4) | ((_d_) & 0x0F))
#define NODE_RSEL_PRSC(_r_, _p_) (uint8_t)(((_r_) << 4) | ((_p_) & 0x0F))
#define NUM_TO_BM(__n__)    (0x01 << __n__)






#define PTC_CHECK_FOR_BAD_POINTER(__p__)  \
  if (NULL == __p__) {                    \
    if (__builtin_constant_p(__p__)) {    \
      badArg("Null pointer detected");    \
    }                                     \
    return PTC_LIB_BAD_POINTER;           \
  }


void badArg(const char *)  __attribute__((weak, error("")));
void badCall(const char *) __attribute__((weak, error("")));




//extern void ptc_conversion_complete(uint8_t type);
//extern void ptc_error_callback(uint8_t source, cap_sensor_t *node);
extern void ptc_event_callback(const ptc_cb_event_t eventType, cap_sensor_t *node);

extern void ptc_event_cb_touch(const ptc_cb_event_t eventType, cap_sensor_t *node);
extern void ptc_event_cb_wake(const ptc_cb_event_t eventType, cap_sensor_t *node);
extern void ptc_event_cb_conversion(const ptc_cb_event_t eventType, cap_sensor_t *node);
extern void ptc_event_cb_calibration(const ptc_cb_event_t eventType, cap_sensor_t *node);
extern void ptc_event_cb_error(const ptc_cb_event_t eventType, cap_sensor_t *node);


// Enables the node. Can be called while an acquisition is in progress.
uint8_t ptc_enable_node(cap_sensor_t *node);

// Disables a node. If the conversion is started, it will be finished
uint8_t ptc_disable_node(cap_sensor_t *node);

// Can be used outside an acquisition process to select ADC/SELFCAP/MUTUAL/SHIELD
void ptc_set_next_conversion_type(ptc_node_type_t type);

// Main task handle for PTC. Handles State-Machine, drift, and calibration
void ptc_process(uint16_t currTime);


// Set the threshold for touch detection and away from touch for a node.
// a "0" will be interpreted as don't change
uint8_t ptc_node_set_thresholds(cap_sensor_t *node, int16_t th_in, int16_t th_out);


// Change Resistor Setting. Note: Only has an effect on mutual sensors
uint8_t ptc_node_set_resistor(cap_sensor_t *node, ptc_rsel_t res);

// Change prescaler.
uint8_t ptc_node_set_prescaler(cap_sensor_t *node, ptc_presc_t presc);

// Sets the gain through adjusting the charge integrator (increases the sensitivity (and noise))
uint8_t ptc_node_set_gain(cap_sensor_t *node, ptc_gain_t gain);

// Sets the number of oversamples. (the value is right-shifted automatically (reduces noise))
uint8_t ptc_node_set_oversamples(cap_sensor_t *node, uint8_t ovs);

// Sets the number of additional PTC Clocks for sampling a node. See also: ADC.SAMPCTRL
uint8_t ptc_node_set_charge_share_delay(cap_sensor_t *node, uint8_t csd);

// this is an internal function, there is no sense in calling it directly
uint8_t ptc_add_node(cap_sensor_t *node, uint8_t *pCh, const uint8_t type);


inline uint8_t ptc_add_selfcap_node(cap_sensor_t *node, const ptc_ch_bm_t xCh, const ptc_ch_bm_t yCh) {
  if (__builtin_constant_p(yCh)) {
    if (yCh == 0) {
      badArg("yCh bitmap mustn't be 0 (Pin_Pxn is not a PTC pin)");
    }
    if (yCh & xCh) {
      badArg("pin bitmap overlap detected");
    }
  }

  // this places only the significant number of bits on stack. Significantly reduces register pressure on DAs
  const uint8_t tsize = sizeof(ptc_ch_arr_t);
  uint8_t pCh[tsize * 2];

  pCh[0]         = (uint8_t)(xCh >>  0);
  pCh[0 + tsize] = (uint8_t)(yCh >>  0);

  #if __PTC_Pincount__ >= 8
  pCh[1]         = (uint8_t)(xCh >>  8);
  pCh[1 + tsize] = (uint8_t)(yCh >>  8);

  #if __PTC_Pincount__ >= 16
  pCh[2]         = (uint8_t)(xCh >> 16);
  pCh[2 + tsize] = (uint8_t)(yCh >> 16);
  pCh[3]         = (uint8_t)(xCh >> 24);
  pCh[3 + tsize] = (uint8_t)(yCh >> 24);
  pCh[4]         = (uint8_t)(xCh >> 32);
  pCh[4 + tsize] = (uint8_t)(yCh >> 32);

  #if __PTC_Pincount__ >=   40
  pCh[5]         = (uint8_t)(xCh >> 40);
  pCh[5 + tsize] = (uint8_t)(yCh >> 40);

  #endif
  #endif
  #endif

  return ptc_add_node(node, pCh, NODE_SELFCAP_bm);
};


inline uint8_t ptc_add_mutualcap_node(cap_sensor_t *node, const ptc_ch_bm_t xCh, const ptc_ch_bm_t yCh) {
  if (__builtin_constant_p(yCh) && __builtin_constant_p(xCh)) {
    if (yCh == 0) {
      badArg("yCh bitmap mustn't be 0 (Pin_Pxn is not a PTC pin)");
    }
    if (xCh == 0) {
      badArg("xCh bitmap mustn't be 0 (Pin_Pxn is not a PTC pin)");
    }
    if (yCh & xCh) {
      badArg("pin overlap detected");
    }
  }

  // this places only the significant number of bits on stack. Significantly reduces register pressure on DAs
  const uint8_t tsize = sizeof(ptc_ch_arr_t);
  uint8_t pCh[tsize * 2];

  pCh[0]          = (uint8_t)(xCh >>  0);
  pCh[0 + tsize] = (uint8_t)(yCh >>  0);

  #if __PTC_Pincount__ >= 8
  pCh[1]          = (uint8_t)(xCh >>  8);
  pCh[1 + tsize] = (uint8_t)(yCh >>  8);

  #if __PTC_Pincount__ >= 16
  pCh[2]          = (uint8_t)(xCh >> 16);
  pCh[2 + tsize] = (uint8_t)(yCh >> 16);
  pCh[3]          = (uint8_t)(xCh >> 24);
  pCh[3 + tsize] = (uint8_t)(yCh >> 24);
  pCh[4]          = (uint8_t)(xCh >> 32);
  pCh[4 + tsize] = (uint8_t)(yCh >> 32);

  #if __PTC_Pincount__ >=   40
  pCh[5]          = (uint8_t)(xCh >> 40);
  pCh[5 + tsize] = (uint8_t)(yCh >> 40);

  #endif
  #endif
  #endif

  return ptc_add_node(node, pCh, NODE_MUTUAL_bm);
};



uint8_t ptc_suspend(void);
void ptc_resume(void);

// If you want to know the compensation capacitance in femto Farad
uint16_t ptc_get_node_cc_fempto(cap_sensor_t *node);

ptc_lib_sm_set_t *ptc_get_sm_settings();

// X and Y channel bitmasks
inline ptc_ch_bm_t ptc_get_node_xCh_bm(cap_sensor_t *node) {
  if (node == NULL) {
    return 0x00;
  }
  ptc_ch_bm_t retval = 0;
  memcpy(&retval, node->hw_xCh_bm, sizeof(node->hw_xCh_bm));
  return retval;
}

inline ptc_ch_bm_t ptc_get_node_yCh_bm(cap_sensor_t *node) {
  if (node == NULL) {
    return 0x00;
  }
  ptc_ch_bm_t retval = 0;
  memcpy(&retval, node->hw_yCh_bm, sizeof(node->hw_yCh_bm));
  return retval;
}

// the measured PTC value. 512 is 0. 0x00 means BAD_POINTER
inline uint16_t ptc_get_node_sensor_value(cap_sensor_t *node) {
  if (node == NULL) {
    return 0x00;
  }
  return node->sensorData;
}

// returns true, if node is a valid pointer and node is touched, otherwise false.
// No other return value so there can be easy checks - not null or null
inline uint8_t ptc_get_node_touched(cap_sensor_t *node) {
  if (node == NULL) {
    return 0x00;
  }

  if (node->stateMachine & (PTC_SM_TOUCH_DETECT | PTC_SM_TOUCH_OUT_FLT)) {
    return 0x01;
  }
  return 0x00;
}


inline uint8_t ptc_get_node_sm(cap_sensor_t *node) {
  PTC_CHECK_FOR_BAD_POINTER(node);
  return node->stateMachine;
}


inline int16_t ptc_get_node_delta(cap_sensor_t *node) {
  if (node == NULL) {
    return 0x8000;  //-32k - impossible value for normal operation
  }
  return (node->sensorData - node->reference);
}

inline uint8_t ptc_get_node_state(cap_sensor_t *node) {
  if (node == NULL) {
    return 0xFF;
  }
  return (node->stateMachine);
}

inline ptc_id_t ptc_get_node_id(cap_sensor_t *node) {
  if (node == NULL) {
    return 0xFF;
  }
  return (node->id);
}


inline uint8_t ptc_node_request_recal(cap_sensor_t *node) {
  PTC_CHECK_FOR_BAD_POINTER(node);

  node->stateMachine = PTC_SM_NOINIT_CAL;
  node->lastStateChange = 0;
  return PTC_LIB_SUCCESS;
}


void ptc_init_ADC0(void);
uint8_t ptc_lp_init(cap_sensor_t *node, uint8_t event_ch);
uint8_t ptc_lp_disable(void);
uint8_t ptc_lp_was_waken(void);

// Called by the interrupt routine. Saves result and selects next node
extern void ptc_eoc(void);


#ifdef __cplusplus
}
#endif

#endif
