# ptc_touch
An Arduino compatible C library to support the PTC in the ATtiny 1-family as well as the DA family.

## Features
* Ready to use with minimal knowledge of the peripheral, but can still be well configured (options are similar to the "official" library)
* Supports Mutual-cap and Self-cap (+Shield) sensing technology
* Supports event-based triggering of conversions, as well as window-compared wake-up from stand-by sleep
* On ATtiny: Can be suspended to allow the polled use of ADC0's internal channels (ADC0 ISRs are used by the library)
* Reduced memory footprint due to CPU specific optimizations compared to the "official" implementation


## Installation
For the non-Arduino people: all required files are inside the src folder (No Arduino exclusive functions are used in this library).


### How to use this library


#### Step 1: Memory Initialization
Allocate memory for every single sensing node. The easiest way is to have an array with `cap_sensor_t` elements, e.g. `cap_sensor_t nodes[3];`. Even though it allows you to access the data stored in this structures directly, it is not allowed to change the contents directly. Always use the provided setter functions for this.

#### Step 2: Node registration
Initialize the node to be either a mutual-cap node with `uint8_t ptc_add_mutualcap_node(cap_sensor_t* node, ptc_ch_bm_t xCh, ptc_ch_bm_t yCh);` or to a self-cap node with `uint8_t ptc_add_selfcap_node(cap_sensor_t* node, ptc_ch_bm_t xCh, ptc_ch_bm_t yCh);`. When everything went without problems, the functions will return the enum `PTC_LIB_SUCCESS`. While mutual-cap requires yCh and xCh not to be 0 (otherwise `PTC_LIB_BAD_ARGUMENT` will be returned), the self-cap requires only the yCh not to be 0. If the xCh is zero, the node will be a normal self-cap node, otherwise it will be of the self-cap with shield type. This becomes relevant when you change between the types, but that will be explained later. This function will also disable the pullup and digital input function of the specified pins. The order, in which this function is called determines the conversion order. The nodes will be also numbered for easier identification. Up to 255 nodes are supported for devices with 16 or less PTC pins. 65k for bigger devices. The nodes are enabled by default, so, in case an added node should be disabled, you can call `uint8_t ptc_disable_node(cap_sensor_t* node);`, and for re-enabling `uint8_t ptc_enable_node(cap_sensor_t* node);`.
Here are some examples:
- `ptc_add_mutualcap_node(&nodes[0], PIN_TO_PTC(PIN_PA7), PIN_TO_PTC(PIN_PA4));`
  - PA4 will be sensing, PA7 will be driving
- `ptc_add_selfcap_node(&nodes[1], 0, PIN_TO_PTC(PIN_PA4));`
  - No Driving pin, only sensing on PA4
- `ptc_add_selfcap_node(&nodes[2], PIN_TO_PTC(PIN_PA7), (PIN_TO_PTC(PIN_PA4) | PIN_TO_PTC(PIN_PA5)));`
  - useful for wakeups, all "buttons" work as one, no matter on which you press, plus driven shield pin.


#### Step 3: Acquisition and Processing
This library requires a regular call to the function `void ptc_process(uint16_t currTime)`. This function handles all background functionality of the library and calls the callback `extern void ptc_event_callback(const uint8_t eventType, cap_sensor_t* node)`, or one of the few sub-callbacks below, when appropriate. First, the function checks if an acquisition was completed (all nodes of the selected type converted). If that's the case, it proceeds to handle the gathered data to handle the respective state machine of each node whose conversion was completed. The more nodes you have, the more time it might take.
The exact workings of this function will exceed the scope of this document.

This function takes an argument, `uint16_t currTime`, to decide when to start the next acquisition. This, compared to having a, as an example, `millis()` inside the library, offers the user significant flexibility on choosing the time source (e.g. TCB.CNT). The Period can be set by `void ptc_set_acqusition_period(uint16_t period)`. Whenever the currTime minus the currTime when the last acquisition trigger happened is greater or equal the period, a new acquisition is started.
However, if there was a successful call to `uint8_t ptc_suspend(void)`, only the timestamp is updated, but no acquisition is started. This also applies when the library was put into low-power mode, except that conversions can still be triggered by events.
Even though this library can handle three different node types, only one type can be acquired at a time. By default, the type of the first node in the list is used. But the user can use the function `ptc_set_next_conversion_type(uint8_t type)` to change this behavior.

#### Step 4: Callback
In order to understand what happens, there is an extern callback function defined in the h-file. This means that you are required to have a function in your sketch-file that look like this: `void ptc_event_callback(const uint8_t eventType, cap_sensor_t* node) {`. The reason why I used an extern function and not a pointer to a callback is that this allows the compiler to inline optimizations, like optimizing away calls with eventTypes that don't need handling.
There are following events:
- `PTC_CB_EVENT_WAKE_TOUCH` (node: lowPower):
  - This event happens in LP mode only when the Window-Comparator was triggered.
- `PTC_CB_EVENT_WAKE_NO_TOUCH` (node: lowPower):
  - This event happens in LP mode only when the Window-Comparator was not triggered.
- `PTC_CB_EVENT_CONV_CMPL` (node: current node):
  - This event happens whenever ptc_process finished handling said node
- `PTC_CB_EVENT_CONV_MUTUAL_CMPL`,
- `PTC_CB_EVENT_CONV_SELF_CMPL`,
- `PTC_CB_EVENT_CONV_SHIELD_CMPL` (node: last converted node):
  - This events happen whenever ptc_process has handled all nodes of said type.
- `PTC_CB_EVENT_CONV_CALIB` (node: current node):
  - This event happens, whenever the calibration was successful.
- `PTC_CB_EVENT_ERR_CALIB` (node: current node):
  - This event happens, whenever the calibration failed on said node.
- `PTC_CB_EVENT_TOUCH_DETECT`(node: current node):
  - This event triggers shortly before the state-machine is updated to the "touched" state.
- `PTC_CB_EVENT_TOUCH_RELEASE`(node: current node):
  - This event triggers shortly before the state-machine is updated to the "not-touched" state.

You can use a simple switch-case to check for the events in the callback.
Another option is to use following "sub-callbacks". As they are weak and aliased with `ptc_event_callback` it is possible to choose between one function to collect all events, or have dedicated function for each event type.
- `void ptc_event_cb_wake(const ptc_cb_event_t eventType, cap_sensor_t* node);`
  - called on `PTC_CB_EVENT_WAKE_*` events
- `void ptc_event_cb_conversion(const ptc_cb_event_t eventType, cap_sensor_t* node);`
  - called on `PTC_CB_EVENT_CONV_*_CMPL` events
- `void ptc_event_cb_error(const ptc_cb_event_t eventType, cap_sensor_t* node);`
  - called on `PTC_CB_EVENT_CONV_CALIB` events
- `void ptc_event_cb_calibration(const ptc_cb_event_t eventType, cap_sensor_t* node);`
  - called on `PTC_CB_EVENT_ERR_CALIB` events
- `void ptc_event_cb_touch(const ptc_cb_event_t eventType, cap_sensor_t* node);`
  - called on `PTC_CB_EVENT_TOUCH_*` events



### Low Power operation
This library also provides a so-called low power mode. This mode changes the operation of the library and the ADC. It will be set up to wait for Events, however it is up to the user to set up the Event-Routing network. The low power mode also sets up the window-comparator to detect touch events. It should be also noted, that it is not possible to use `ptc_suspend` while the library is in low-power mode. the low-power mode has to be disabled first before being able to suspend the library.

In order to initialize the low-power mode, just call `uint8_t ptc_lp_init(cap_sensor_t* node)` (to disable: `uint8_t ptc_lp_disable(void)`) with the node you want to have automatically converted. The node must be enabled and must have been calibrated beforehand to work correctly, meaning it had to have a couple finished conversions.

To check, if the window-comparator was triggered outside of the WAKE events, you can use the function `uint8_t ptc_lp_was_waken(void)` that returns either `PTC_LIB_WAS_WAKEN` or `PTC_LIB_ASLEEP`, at least while the low-power mode is active.

### Pins and how to use the ptc_add_* functions

The ptc_add_* functions require a bitmap (ptc_ch_bm_t yCh; ptc_ch_bm_t xCh) of pins, that will be connected to the PTC internals upon the beginning of a conversion. The bit position equals to the PTC pin numbering, that you can find in the PTC column in the datasheet. On Tinies, it is different from the usual numbering, for example PA4 on the 1614 is X0/Y0, resulting in a bitmap of 0x01. On DAs it is more straightforward, PA0 is bitmap 1<<0, PB0 is 1<<8. PORTC is skipped, so PD0 is 1<<16.

However, to make it easier, a compile-time look-up-table with a macro to access it is provided, PIN_TO_PTC(PIN_Pnx). If the pin is valid, the corresponding bit-map is returned, otherwise null. But this requires the megatiny/Dx core from Spence Konde, as the PIN_Pnx naming is defined there.

The reason, why a bit-map is used, is to provide a way to connect the electrodes of multiple pins to act as one big electrode. In order to create a node with multiple pins connected, a simple OR operation is enough.

The ptc_ch_bm_t is a typedef that depends on the pincount of the device and ranges from uint8_t to uint16_t (and up to uint64_t on the DAs). Up to 8 PTC-Pins, uint8_t, up to 16 PTC pins, uint16_t, and so on.

### PTC Operation
![PTC_InnerWorkings](https://github.com/MX682X/ptc_touch/assets/58419867/823c487c-0633-4031-b381-45e7a32867fb)
(Source: [Microchip's PTC Subsystem Firmware User's Guide](https://web.archive.org/web/20221225142000/https://www.mouser.com/pdfdocs/SAMA5D2_PTC_Firmware_UG.pdf))
This schematic was made for a different chip, but it is likely to look similar to this on the AVRs.

Most of the following is a hypothesis based on the publicly available documentation and observation.
The PTC is using a charge transfer between a variable and a fixed capacitance to measure a difference between those two. The variable capacitance in this case is the sensor electrode and the fixed one is the internal Cc. The neat thing about having a dedicated hardware for this, is that Cc is not really fixed, compared to the Sample-and-Hold capacitor of the normal ADC, it can be calibrated to be about the same as the electrode we want to measure (See below).

In order to measure the capacitance of the node, the PTC performs following Steps:
  1. Charge the electrode to Vdd
  2. Wait for charge to complete
  3. Disconnect node from Vdd and instead connect it to Cc
  4. Measure voltage of Cc
  5. Connect node and Cc to GND to discharge them

The measured voltage depends on what happens around the electrode. In an idle state, 50% of the charge of the node (and thus voltage) is transferred to Cc, however when something conductive is moved towards the electrode, the capacitance will increase slightly. With a higher capacitance, the node will have a higher charge, meaning less then 50% of the charge can be transferred to Cc until they reach an equal Voltage, which means the overall voltage will be higher, which can be measured by the ADC.

If you have trouble understanding: Imagine two equally sized volumes connected through a valve. You fill the first up with water, open the valve and look on the second, how high the water has risen. After marking the "idle" level, when you put a finger in the water and see the water rise. This new level is the increase in voltage due to an interference.

### Calibration / Compensation

Based on the documentation found online, the PTC has an internal, tunable capacitor connected after the series resistance to ground that is used to compensate the parasitic capacitance of the electrodes. Every node starts with a default compensation value. As soon as the node is enabled, the library attempts to find a compensation setting that will result in an ADC value of about 512 counts (1/2 of ADC resolution). Based on oscilloscope readings, it can also be said that the PTC tries to have a charge of 50% VCC on the electrode when being acquired. This is the also the reason, why the digital input function of the pins is disabled.

The maximum compensation is about 30pF for Mutual-cap and about 50pF for Self-cap. It is possible to get the compensation capacitance with uint16_t ptc_get_node_cc_femto(cap_sensor_t* node); - however this function has to do a lot of calculations and is thus a bit bloat-y. It will also return the value in femto farrads, to avoid floats. Read more here: (dead link pruned; I don't know what document it was, I don't think I've seen it).

Different pins have a different parasitic capacitance. I suspect this is depends on the internal circuitry and alternative functions, so it's normal to see some difference with pins next to each other.

### Tuning of nodes

In order to ease the use of the PTC module, the ptc_add_* functions will initialize the cap_sensor_t struct with some default values, like the CC value mentioned above. That values can be easily changed and will be applied the next time a conversion of said node starts. Here is a list:
- `uint8_t ptc_node_set_gain(cap_sensor_t *node, ptc_gain_t gain)`. Increases the sensitivity of the electrode by adjusting a capacitor on the integrator (I think). Defaults to 1x Gain.
  - Valid values are: `PTC_GAIN_1`, `PTC_GAIN_2`, `PTC_GAIN_4`, `PTC_GAIN_8`, `PTC_GAIN_16`, `PTC_GAIN_32` (Tiny only).
- `uint8_t ptc_node_set_oversamples(cap_sensor_t *node, uint8_t ovs)`. Defines the amount of ADC Oversampling. Will not affect the count value, as it is internally right-shifted. Valid values are in the range from 0 to 6 resulting in 1x, 2x, 4x, 8x, 16x, 32x, 64x oversampling. Defaults to 16x.
- `uint8_t ptc_node_set_charge_share_delay(cap_sensor_t *node, uint8_t csd)`. Affects the Sample length of the ADC. This does pretty much the same thing as ADC.SAMPCTRL register. Valid range is from 0 to 31. Defaults to 0 extra clocks.
- `uint8_t ptc_node_set_prescaler(cap_sensor_t *node, ptc_presc_t presc)`. It is possible to slow down the ADC/PTC clock by adjusting the Prescaler. The ADC/PTC Clock should be between 1 and 2 MHz. The library calculates the default based on F_CPU.
  - Valid values for Tiny are: `PTC_PRESC_DIV2_gc`, `PTC_PRESC_DIV4_gc`, `PTC_PRESC_DIV8_gc`, `PTC_PRESC_DIV16_gc`, `PTC_PRESC_DIV32_gc`, `PTC_PRESC_DIV64_gc`, `PTC_PRESC_DIV128_gc`, `PTC_PRESC_DIV256_gc`.
  - Valid values for DA are: `PTC_PRESC_DIV2_gc`, `PTC_PRESC_DIV4_gc`, `PTC_PRESC_DIV6_gc`, `PTC_PRESC_DIV8_gc`, `PTC_PRESC_DIV10_gc`, `PTC_PRESC_DIV12_gc`, `PTC_PRESC_DIV14_gc`,  `PTC_PRESC_DIV16_gc`.
- `uint8_t ptc_node_set_resistor(cap_sensor_t *node, ptc_rsel_t res)`. Allows to change the serial resistor between the Cc and the node. Fixed at 100k for Self-Cap. Defaults to 50k for Mutual-Cap.
  - Valid Values are: `RSEL_VAL_0`, `RSEL_VAL_20`, `RSEL_VAL_50`, `RSEL_VAL_70`, `RSEL_VAL_80` (DA only), `RSEL_VAL_100`, `RSEL_VAL_120` (DA only), `RSEL_VAL_200`.

If a node is not sensitive enough, you can increase the Analog Gain (if it becomes too sensitive, an increase of the thresholds might be needed). However it is better to have a bigger electrode to begin with because the bigger the area, the higher is the capacitance delta.

### Global settings of the State-machine
The state-machine, which changes the node's state between Calibration, touch, no touch, etc. uses some variables that are valid for all nodes, those are:
- `uint16_t force_recal_delta`. Each node has a threshold value that is used to calculate the delta. This Threshold value is drifting over time to adjust for environmental changes. If the threshold value drifts 512 +/- this value, a recalibration of CC is performed. Default: 150
- `uint8_t  touched_detect_nom`. Number of consecutive Measurements (Conversions) that are above the touch threshold until the node becomes "touched". Default: 3
- `uint8_t  untouched_detect_nom`. Number of consecutive measurements that are below the no-touch threshold until the node is fully untouched. Default: 3
- `uint8_t  touched_max_nom`. Number of consecutive measurements plus one in the touched state until a recalibration is forced. Can be disabled by writing 255 to it. Default: 200
- `uint8_t  drift_up_nom`. If the delta is higher then the reference, but lower then the threshold, the amount of consecutive measurements plus one, until the reference is incremented. Can be disabled with 255. Default: 20.
- `uint8_t  drift_down_nom`. If the delta is below the reference, the amount of consecutive measurements plus one until the reference is decremented. Can be disabled with 255. Default: 20.
