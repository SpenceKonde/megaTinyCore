#include <ptc.h>
#include <avr/sleep.h>
/*
 * This example demonstrates how to use the PTC in sleep mode.
 * The library changes the triggering to an Event-System based approach.
 * However, only one node can be monitored this way. To allow a wake-up,
 * all nodes a connected together in the low-power node.
 * PA4 is the X-Line, while PA5, PA6 and PA7 act as Y-Line.
 */
#define MySerial Serial
#if !defined(MILLIS_USE_TIMERNONE)
cap_sensor_t nodes[3];
cap_sensor_t lp_node;   // as an example, could also be part of the array above

void setup() {
  MySerial.begin(115200);

  // this puts the node on the list and initializes to default values
  ptc_add_mutualcap_node(&nodes[0], PIN_TO_PTC(PIN_PA4), PIN_TO_PTC(PIN_PA5));
  ptc_add_mutualcap_node(&nodes[1], PIN_TO_PTC(PIN_PA4), PIN_TO_PTC(PIN_PA6));
  ptc_add_mutualcap_node(&nodes[2], PIN_TO_PTC(PIN_PA4), PIN_TO_PTC(PIN_PA7));

  // make the "low power" node a lumped sensor - this allows to monitor all electrodes at the same time.
  // In other words, this creates a single, big button.
  ptc_add_mutualcap_node(&lp_node, PIN_TO_PTC(PIN_PA4), (PIN_TO_PTC(PIN_PA5) | PIN_TO_PTC(PIN_PA6) | PIN_TO_PTC(PIN_PA7)));

  RTC.PITCTRLA = RTC_PITEN_bm;                          // In this example, the PIT is our Event source/timer
  EVSYS.CHANNEL0 = EVSYS_CHANNEL0_RTC_PIT_DIV2048_gc;   // The user must select the Event Channel

  SLPCTRL.CTRLA = SLEEP_MODE_STANDBY | SLPCTRL_SEN_bm;  // The deepest we can go is Stand-by

  // Make sure Serial works
  MySerial.println("Hello World!");
}

uint32_t nextSleep = 10000;   // entering sleep after 10 seconds of inactivity

void loop() {
  uint32_t ms = millis();
  ptc_process(ms);            // main ptc task, requires regular calls
  if (ms > nextSleep) {
    nextSleep = ms + 10000;
    MySerial.println("Sleepy...");
    MySerial.flush();         // wait for transmission to finish


    ptc_lp_init(&lp_node, EVSYS_USER_CHANNEL0_gc);  // Will set STARTEI bit to start conversions, needs the desired Event Channel
    while (ptc_lp_was_waken() != PTC_LIB_WAS_WAKEN) {
      sleep_cpu();      // Make sure the ADC has woken the CPU, otherwise, go back to sleep.
    }                   // The library does not filter wake-ups. After all, something else might have woken the chip.
    MySerial.println("I'm Awake!");
    ptc_lp_disable();   // disable the low-power node to restore the usual acquisition process
    MySerial.flush();   // wait for finished transmission from Serial
  }
}

// callbacks that are called by ptc_process at different points to ease user interaction
void ptc_event_cb_touch(const ptc_cb_event_t eventType, cap_sensor_t *node) {
  uint32_t ms = millis();
  if (PTC_CB_EVENT_TOUCH_DETECT == eventType) {         // a low-power node, that has waken the chip won't issue a TOUCH_DETECT
    MySerial.print("node touched:");
    MySerial.println(ptc_get_node_id(node));
    nextSleep = ms + 10000;
  } else if (PTC_CB_EVENT_TOUCH_RELEASE == eventType) { // however, a TOUCH_RELEASE will be, when the finger is removed again
    MySerial.print("node released:");
    MySerial.println(ptc_get_node_id(node));
    nextSleep = ms + 10000;
  }
}

void ptc_event_cb_conversion(const ptc_cb_event_t eventType, cap_sensor_t *node) {
  if (PTC_CB_EVENT_CONV_TYPE_CMPL_MSK & eventType) {
    // Do more complex things here
  }
  (void)node;   // remove unused warning
}

void ptc_event_cb_calibration(const ptc_cb_event_t eventType, cap_sensor_t *node)  {
  if (PTC_CB_EVENT_ERR_CALIB_LOW == eventType) {
    MySerial.print("Calib error, Cc too low.");
  } else if (PTC_CB_EVENT_ERR_CALIB_HIGH == eventType) {
    MySerial.print("Calib error, Cc too high.");
  } else if (PTC_CB_EVENT_ERR_CALIB_TO == eventType) {
    MySerial.print("Calib error, calculation timeout.");
  } else {
    MySerial.print("Calib Successful. Time to fix: ");  // Time to fix indicates how long the calibration took. Should be < 6 preferably.
    MySerial.print(node->lastStateChange);              // Values close to 10 might indicate some problems with noise.
  }
  MySerial.print(" Node: ");
  MySerial.println(ptc_get_node_id(node));
}

void ptc_event_cb_wake(const ptc_cb_event_t eventType, cap_sensor_t *node) {
  if (PTC_CB_EVENT_WAKE_TOUCH == eventType) {
    // True if the node was touched when a wakeup occurred
  } else if (PTC_CB_EVENT_WAKE_NO_TOUCH == eventType) {
    // True if the node was no touch when a wakeup occurred
  }
  (void)node;   // remove unused warning
}
#else
void setup() {
}
void loop() {
}
#endif
