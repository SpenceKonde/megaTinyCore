#include <ptc.h>
#include <avr/sleep.h>
/*
 * This example creates two normal nodes and a combined node
 * on the pins PA4 and PA6 with PA5 acting as shield. Had to use
 * the shield, as the compensation went too high for the combined
 * node.
 * As it is not possible to change the node when the CPU is asleep
 * a combined node is used to wait for touch events.
 * The first nodes have the IDs 0 and 1, while the lp_node has
 * the ID 2.
 * As millis() is not incremented while the CPU is asleep, it will
 * take the Chip 10 seconds after a wake event before it goes back
 * to sleep.
 * ptc_process will also fire either PTC_CB_EVENT_WAKE_TOUCH or
 * PTC_CB_EVENT_WAKE_NO_TOUCH after a conversion on the lp-node
 * has been finished.
 */
#define MySerial Serial

cap_sensor_t nodes[2];
cap_sensor_t lp_node;   // as an example, could also be part of the array

void setup() {
  MySerial.begin(115200);

  // this puts the node on the list and initializes to default values
  ptc_add_selfcap_node(&nodes[0], PIN_TO_PTC(PIN_PA4), PIN_TO_PTC(PIN_PA5));
  ptc_add_selfcap_node(&nodes[1], PIN_TO_PTC(PIN_PA6), PIN_TO_PTC(PIN_PA5));

  // as each node requires some calibration and drift, it is recommended
  // to keep it "in the loop" during normal operation.
  // However, it is possible to disable it until it is needed.
  // This is also a so called "lumped" node. This allows to check all nodes at the same time.
  // Useful for low-power monitoring for user-input
  ptc_add_selfcap_node(&lp_node, PIN_TO_PTC(PIN_PA4) | PIN_TO_PTC(PIN_PA6), PIN_TO_PTC(PIN_PA5));



  // In low-power mode, the ADC is triggered based on positive Event fanks
  // Due to the big variety of possible Event sources, it is up to the
  // User to set up the Routing of the Event System.
  // This example will use the RTC PIT as Event generator, as it allows us
  // to use the low-power oscillator.

  // Enable PIT for the EVSYS with 32kHz / 4096 giving us about 8Hz
  RTC.PITCTRLA = RTC_PITEN_bm;
  EVSYS.ASYNCCH3 = EVSYS_ASYNCCH3_PIT_DIV4096_gc;
  EVSYS.ASYNCUSER1 = EVSYS_ASYNCUSER1_ASYNCCH3_gc;

  // For Debugging of EVSYS (to make sure there is a clock)
  //EVSYS.ASYNCUSER8 = EVSYS_ASYNCUSER8_ASYNCCH3_gc;
  //PORTMUX.CTRLA = PORTMUX_EVOUT0_bm;
  //pinMode(PIN_PA2, OUTPUT);


  // Stand-by sleep
  SLPCTRL.CTRLA = SLEEP_MODE_STANDBY | SLPCTRL_SEN_bm;

  // Make sure Serial works
  MySerial.println("Hello World!");
}

uint32_t nextSleep = 10000;

void loop() {
  uint32_t ms = millis();
  ptc_process(ms);    // main ptc task, requires regular calls
  if (ms > nextSleep) {
    nextSleep = ms + 10000;
    MySerial.println("Sleepy...");
    MySerial.flush(); // empty the Serial TX buffer

    ptc_lp_init(&lp_node);  // Will set STARTEI bit to start conversions
    while (ptc_lp_was_waken() != PTC_LIB_WAS_WAKEN) {
      sleep_cpu();  // Make sure the ADC has woken the CPU, otherwise, go back to sleep
    }               // The library does not filter the wake-up events. The user must make sure
                    // there are no detected touches before going back to sleep.
    MySerial.println("I'm Awake!");
    ptc_lp_disable();
  }
}

void ptc_event_cb_touch(const ptc_cb_event_t eventType, cap_sensor_t* node) {
  if (PTC_CB_EVENT_TOUCH_DETECT == eventType) {
    MySerial.print("node touched:");
    MySerial.println(ptc_get_node_id(node));
  } else if (PTC_CB_EVENT_TOUCH_RELEASE == eventType) {
    MySerial.print("node released:");
    MySerial.println(ptc_get_node_id(node));
  }
}

void ptc_event_cb_conversion(const ptc_cb_event_t eventType, cap_sensor_t* node) {
  if (PTC_CB_EVENT_CONV_TYPE_CMPL_MSK == eventType) {
    // Do more complex things here
  }
  (void)node;   // remove unused warning
}

void ptc_event_cb_calibration(const ptc_cb_event_t eventType, cap_sensor_t* node)  {
  if (PTC_CB_EVENT_ERR_CALIB_LOW == eventType) {
    MySerial.print("Calib error, Cc too low.");
  } else if (PTC_CB_EVENT_ERR_CALIB_HIGH == eventType) {
    MySerial.print("Calib error, Cc too high.");
  } else if (PTC_CB_EVENT_ERR_CALIB_TO == eventType) {
    MySerial.print("Calib error, calculation timeout.");
  } else {
    MySerial.print("Calib Successful.");
  }
  MySerial.print(" Node: ");
  MySerial.println(ptc_get_node_id(node));
}

void ptc_event_cb_wake(const ptc_cb_event_t eventType, cap_sensor_t* node) {
   if (PTC_CB_EVENT_WAKE_TOUCH == eventType) {
      // True if the node was touched when a wakeup occurred
  } else if (PTC_CB_EVENT_WAKE_NO_TOUCH == eventType) {
    // True if the node was no touch when a wakeup occurred
  }
  (void)node;   // remove unused warning
}
