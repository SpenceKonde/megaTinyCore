#include <ptc.h>
/*
 * This example creates two different sensing nodes.
 * PA4 and PA5 are on the Y-Lines, PA6 is the X-Line.
 * PTC_CB_EVENT_TOUCH_DETECT and PTC_CB_EVENT_TOUCH_RELEASE can
 * be used for quick actions, like switching a pin or variable,
 * but it is recommended to use PTC_CB_EVENT_CONV_MUTUAL_CMPL, as
 * otherwise the handling of the successive nodes would be delayed.
 */
#define MySerial Serial
#if !defined(MILLIS_USE_TIMERNONE)
cap_sensor_t nodes[2];

void setup() {
  MySerial.begin(115200);

  // this puts the node on the list and initializes to default values
  ptc_add_mutualcap_node(&nodes[0], PIN_TO_PTC(PIN_PA4), PIN_TO_PTC(PIN_PA7));
  ptc_add_mutualcap_node(&nodes[1], PIN_TO_PTC(PIN_PA5), PIN_TO_PTC(PIN_PA7));


  // Make sure Serial works
  MySerial.println("Hello World!");
}

void loop() {
  ptc_process(millis());    // main ptc task, requires regular calls
}

// callbacks that are called by ptc_process at different points to ease user interaction
void ptc_event_cb_touch(const ptc_cb_event_t eventType, cap_sensor_t *node) {
  if (PTC_CB_EVENT_TOUCH_DETECT == eventType) {
    MySerial.print("node touched:");
    MySerial.println(ptc_get_node_id(node));
  } else if (PTC_CB_EVENT_TOUCH_RELEASE == eventType) {
    MySerial.print("node released:");
    MySerial.println(ptc_get_node_id(node));
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
    MySerial.print("Calib Successful.");
  }
  MySerial.print(" Node: ");
  MySerial.println(ptc_get_node_id(node));
}
#else
void setup() {
}
void loop() {
}
#endif
