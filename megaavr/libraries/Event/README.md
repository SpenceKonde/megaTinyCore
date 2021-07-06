# Event
A library for interfacing with the built-in Event system on the megaAVR-0 series MCUs.
Developed by [MCUdude](https://github.com/MCUdude/).

**From the datasheet:**
> The Event System (EVSYS) enables direct peripheral-to-peripheral signaling. It allows a change in one peripheral (the event generator) to trigger actions in other peripherals (the event users) through event channels, without using the CPU. It is designed to provide short and predictable response times between peripherals, allowing for autonomous peripheral control and interaction, and also for synchronized timing of actions in several peripheral modules. It is thus a powerful tool for reducing the complexity, size, and execution time of the software.

More information about the Event system and how it works can be found in the [Microchip Application Note AN2451](http://ww1.microchip.com/downloads/en/AppNotes/DS00002451B.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf).

## Level vs. Pulse events
There are two types of events - a "pulse" interrupt, which lasts for the duration of a single clock cycle (either `CLK_PER` or a relevant (slower) clock - for example, the USART XCK generator provides a pulse event which lasts one XCK period, whuich is far slower than CLK_PER)q1                                                                                                               , or a "level" interrupt which lasts for the duration of some condition. Often for a given even generator or user only one or the other makes sense. Less often, for some reason or another, you may need a level event, but all you have is a pulse event - or the other way around. A[CCL module (Logic.h)](../Logic/README.md) event between the two at the cost of the logic module and one event channel. In the case of timer WO (PWM) channels, the CCL already has level inputs to match the pulse inputs that the event system can get on compare match. Many of the pulse interrupts are generated at the same moment that interrupt flags get set - except that where an interrupt bit sticks around until serviced, the pulse bit is present generally for only a single clock cycle (meaning they are only *usable* for triggering other event system things or peripherals. s can be (they often let you do similar things faster with less CPU involvement, too), but unlike interrupts, they won't stick around until you explicitly clear them


## Synchronization
The 0/1-series are weird here - read the 2-series section first.

### 2-series
The event system, under the hood, is asynchronous - it can react faster than the system clock (often a lot faster). On the 2-series, these work like the Dx-series and presumably future releases will. Per the datasheet for the 2-series:

>Events can be either synchronous or asynchronous to the peripheral clock. Each Event System channel has two subchannels: one asynchronous and one synchronous.

>The asynchronous subchannel is identical to the event output from the generator. If the event generator generates a signal asynchronous to the peripheral clock, the signal on the asynchronous subchannel will be asynchronous. If the event generator generates a signal synchronous to the peripheral clock, the signal on the asynchronous subchannel
>will also be synchronous.

>The synchronous subchannel is identical to the event output from the generator, if the event generator generates a signal synchronous to the peripheral clock. If the event generator generates a signal asynchronous to the peripheral clock, this signal is first synchronized before being routed onto the synchronous subchannel. Depending on when the event occurs, synchronization will delay the it by two to three clock cycles. The Event System automatically perform this synchronization if an asynchronous generator is selected for an event channel.

The fact that it is asynchronous usually doesn't matter - it's either "faster than anything else" (if the user is async), or "2-3 clock cycles behind", but it is one of the things one should keep in mind when using these features, because every so often it does.

### 0/1-series
Unlike the 2-series, where sync vs async is handled transparently, there are no hidden subchannels here. There are two channels with synchronizers and 4 without. Two of the peripherals care: USART0 and TCA0 can only use the sync channels. Everything else can use either kind. The layout of channels is also totally different.

## Generator table
Below is a table with all possible generators for each channel. The event system on the 0/1-series and on the 2-series is very different in this regard
The generators associated with pins are only available on parts that have that pin, and those associated with a peripheral. Similarly are only defined where that peripheral exists. .

### 0-series and 1-series
Yes, this is a terribly messy setup compared to the 2-series (and every other product line with an event system - this was the first, and they realized their mistake - albeit too late for these). There are two kinds of generators, synchronous and asynchronous. Synchronous users (TCA0, USART0) can only use synchronous generators, while async. users can use any generator. The synchronization or lack thereof is w/regards to `CLK_PER`Note that in cases where the same generator is listed for multiple channels, but prefixed with gen followed by a number, they are NOT interchangible. `gen4::ac1_out` is not the same as `gen2::ac2_out`... the values are all over the place.

Only the 1-series parts have Event1 (Sync channel 1), Event4, and Event5;

Many parts with lower pincount do not have any options specific to a channel available. Level events are marked with an `L` on this chart; all others are pulses, `P` indicates a "pulse" event, but one of a non-

| All Sync. (Event0 & Event1)| Event0 (EventSync0)      | Event1 (EventSync1)     | All Async. (Event2-5)   |   Event2 (EventAsync0)  |  Event3 (EventAsync1)   |  Event4 (EventAsync2)   |  Event5 (EventAsync3)   |
|----------------------------|--------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|
| `gens::disable`            | `gen0::disable`          | `gen1::disable`         | `gen::disable`          | `gen2::pin_pa0` L       | `gen3::pin_pb0` L       | `gen4::pin_pc0`         | `gen5::rtc_div8192`     |
| `gens::tcb0_capt`          | `gen0::pin_pc0`          | `gen1::pin_pb0`         | `gen::ccl_lut0`         | `gen2::pin_pa1` L       | `gen3::pin_pb1` L       | `gen4::pin_pc1`         | `gen5::rtc_div4096`     |
| `gens::tca0_ovf_lunf`      | `gen0::pin_pc1`          | `gen1::pin_pb1`         | `gen::ccl_lut1`         | `gen2::pin_pa2` L       | `gen3::pin_pb2` L       | `gen4::pin_pc2`         | `gen5::rtc_div2048`     |
| `gens::tca0_hunf`          | `gen0::pin_pc2`          | `gen1::pin_pb2`         | `gen::ac0_out`          |
| `gens::tca0_cmp0`          | `gen0::pin_pc3`          | `gen1::pin_pb3`         | `gen::tcd0_cmpbclr`     |
| `gens::tca0_cmp1`          | `gen0::pin_pc4`          | `gen1::pin_pb4`         | `gen::tcd0_cmpaset`     | `gen2::pin_pa4` L       | `gen3::pin_pb4` L       | `gen4::pin_pc4`         | `gen5::rtc_div512`      |
| `gens::tca0_cmp2`          | `gen0::pin_pc5`          | `gen1::pin_pb5`         | `gen::tcd0_cmpbset`     | `gen2::pin_pa5` L       | `gen3::pin_pb5` L       | `gen4::pin_pc5`         | `gen5::rtc_div256`      |
|                            | `gen0::pin_pa0`          | `gen1::pin_pb6`         | `gen::tcd0_progev`      | `gen2::pin_pa6` L       | `gen3::pin_pb6` L       | `gen4::ac1_out`         | `gen5::rtc_div128`      |
|                            | `gen0::pin_pa1`          | `gen1::pin_pb7`         | `gen::rtc_ovf` P        | `gen2::pin_pa7` L       | `gen3::pin_pb7` L       | `gen4::ac2_out`         | `gen5::rtc_div64`       |
|                            | `gen0::pin_pa2`          | `gen1::tcb1_capt`       | `gen::rtc_cmp` P        | `gen2::updi`    L       | `gen3::ac1_out` L       |                         | `gen5::ac1_out`         |
|                            | `gen0::pin_pa3`          |                         |                         | `gen2::ac1_out` L       | `gen3::ac2_out` L       |                         | `gen5::ac2_out`         |
|                            | `gen0::pin_pa4`          |                         |                         | `gen2::ac2_out` L       |                         |                         |                         |
|                            | `gen0::pin_pa5`          |                         |                         |                         |                         |                         |                         |
|                            | `gen0::pin_pa6`          |                         |                         |                         |                         |                         |                         |
|                            | `gen0::pin_pa7`          |                         |                         |                         |                         |                         |                         |
|                            | `gen0::pin_pa2`          |                         |                         |                         |                         |                         |                         |
|                            | `gen0::tcb1_capt`        |                         |                         |                         |                         |                         |                         |


#### 2-series
On the 2-series parts, they put synchronizer onto each channel, making it into two subchannels, a sync and async one, which is completely transparent to the user. The channels are far more flexible; like all other parts with an event system, only the port pins, and which set of taps off the PIT it gets are different between event channels. And they are delivered in pairs as they should be.


|  All Pins                |  Event0                 |  Event1                 |  Event2                 |  Event3                 |  Event4                 |  Event5                 |
|--------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|
| `gen::disable`           | `gen0::disable`         | `gen1::disable`         | `gen2::disable`         | `gen3::disable`         | `gen4::disable`         | `gen5::disable`         |
| `gen::updi_synch`        | `gen0::rtc_div8192`     | `gen1::rtc_div512`      | `gen2::rtc_div8192`     | `gen3::rtc_div512`      | `gen4::rtc_div8192`     | `gen5::rtc_div512`      |
| `gen::rtc_ovf`           | `gen0::rtc_div4096`     | `gen1::rtc_div256`      | `gen2::rtc_div4096`     | `gen3::rtc_div256`      | `gen4::rtc_div4096`     | `gen5::rtc_div256`      |
| `gen::rtc_cmp`           | `gen0::rtc_div2048`     | `gen1::rtc_div128`      | `gen2::rtc_div2048`     | `gen3::rtc_div128`      | `gen4::rtc_div2048`     | `gen5::rtc_div128`      |
| `gen::ccl0_out`          | `gen0::rtc_div1024`     | `gen1::rtc_div64`       | `gen2::rtc_div1024`     | `gen3::rtc_div64`       | `gen4::rtc_div1024`     | `gen5::rtc_div64`       |
| `gen::ccl1_out`          | `gen0::pin_pa0`         | `gen1::pin_pa0`         | `gen2::pin_pc0`         | `gen3::pin_pc0`         | `gen4::pin_pb0`         | `gen5::pin_pb0`         |
| `gen::ccl2_out`          | `gen0::pin_pa1`         | `gen1::pin_pa1`         | `gen2::pin_pc1`         | `gen3::pin_pc1`         | `gen4::pin_pb1`         | `gen5::pin_pb1`         |
| `gen::ccl3_out`          | `gen0::pin_pa2`         | `gen1::pin_pa2`         | `gen2::pin_pc2`         | `gen3::pin_pc2`         | `gen4::pin_pb2`         | `gen5::pin_pb2`         |
| `gen::ac0_out`           | `gen0::pin_pa3`         | `gen1::pin_pa3`         | `gen2::pin_pc3`         | `gen3::pin_pc3`         | `gen4::pin_pb3`         | `gen5::pin_pb3`         |
| `gen::adc0_ready`        | `gen0::pin_pa4`         | `gen1::pin_pa4`         | `gen2::pin_pc4`         | `gen3::pin_pc4`         | `gen4::pin_pb4`         | `gen5::pin_pb4`         |
| `gen::adc0_sample`       | `gen0::pin_pa5`         | `gen1::pin_pa5`         | `gen2::pin_pc5`         | `gen3::pin_pc5`         | `gen4::pin_pb5`         | `gen5::pin_pb5`         |
| `gen::adc0_window`       | `gen0::pin_pa6`         | `gen1::pin_pa6`         | `gen2::pin_pa0`         | `gen3::pin_pa0`         | `gen4::pin_pb6`         | `gen5::pin_pb6`         |
| `gen::usart0_xck`        | `gen0::pin_pa7`         | `gen1::pin_pa7`         | `gen2::pin_pa1`         | `gen3::pin_pa1`         | `gen4::pin_pb7`         | `gen5::pin_pb7`         |
| `gen::usart1_xck`        | `gen0::pin_pb0`         | `gen1::pin_pb0`         | `gen2::pin_pa2`         | `gen3::pin_pa2`         | `gen4::pin_pc0`         | `gen5::pin_pc0`         |
| `gen::spi0_sck`          | `gen0::pin_pb1`         | `gen1::pin_pb1`         | `gen2::pin_pa3`         | `gen3::pin_pa3`         | `gen4::pin_pc1`         | `gen5::pin_pc1`         |
| `gen::tca0_ovf_lunf`     | `gen0::pin_pb2`         | `gen1::pin_pb2`         | `gen2::pin_pa4`         | `gen3::pin_pa4`         | `gen4::pin_pc2`         | `gen5::pin_pc2`         |
| `gen::tca0_hunf`         | `gen0::pin_pb3`         | `gen1::pin_pb3`         | `gen2::pin_pa5`         | `gen3::pin_pa5`         | `gen4::pin_pc3`         | `gen5::pin_pc3`         |
| `gen::tca0_cmp0`         | `gen0::pin_pb4`         | `gen1::pin_pb4`         | `gen2::pin_pa6`         | `gen3::pin_pa6`         | `gen4::pin_pc4`         | `gen5::pin_pc4`         |
| `gen::tca0_cmp1`         | `gen0::pin_pb5`         | `gen1::pin_pb5`         | `gen2::pin_pa7`         | `gen3::pin_pa7`         | `gen4::pin_pc5`         | `gen5::pin_pc5`         |
| `gen::tca0_cmp2`         | `gen0::pin_pb6`         | `gen1::pin_pb6`         |                         |                         |                         |                         |
| `gen::tcb0_capt`         | `gen0::pin_pb7`         | `gen1::pin_pb7`         |                         |                         |                         |                         |
| `gen::tcb0_ovf`          |                         |                         |                         |                         |                         |                         |
| `gen::tcb1_capt`         |                         |                         |                         |                         |                         |                         |
| `gen::tcb1_ovf`          |                         |                         |                         |                         |                         |                         |



## Classes

### Event class
Class for interfacing with the Event systemn (`EVSYS`). Each event channel has its own object.
Use the predefined objects `Event0`, `Event1`, `Event2`, `Event3`, `Event4`, `Event5`. These, alternately, are known as `EventSync0`, `EventSync1`, `EventAsync0`, `EventAsync1`, `EventAsync2`, `EventAsync3` on the tinyAVR 0-series and 1-series. Note that not all generators are available on all channels, so make sure you use the right channel for the task.

#### get_channel_number()
Function to get the current channel number. Useful if the channel object has been passed to a function as reference.

##### Usage
``` c++
uint8_t what_channel(Event& event_chan) {
  event_chan.get_channel_number();
}
```

#### get_user_channel()
Function to get what event channel a user is connected to. Returns -1 if not connected to any channel. Note that we use `user::` as prefix when we refer to event users. This is a static method - you don't have to specify an object to determine what channel the user is connected to. An event channel, and hence an event generator, can have as many event users are you want - but an event user can only have one event generator. You cannot get a list or count of all users connected to a generator except by iterating over the list.

##### Usage
```c++
uint8_t connected_to = Event::get_user_channel(user::ccl0_event_a); // Returns the channel number ccl0_event_a is connected to
```


#### set_generator()
Function to assign an event generator to a channel. Note that we use the prefix genN:: (where N is the channel number) when referring to generators unique to this particular channel. we use gen:: when referring to generators available on all generators.

##### Usage
```c++
Event0.set_generator(gen::ccl0_out); // Use the output of logic block 0 (CCL0) as an event generator for Event0
Event2.set_generator(gen2::pin_pc0); // Use pin PC0 as an event generator for Event2
```

## get_generator()
Function to get the generator used for a particular channel.

### Usage
```c++
uint8_t generator_used = Event0.get_generator();
if(generator_used == gen::ccl0_out) {
  Serial.println("We're using gen::ccl0_out as generator");
}
```


## set_user()
Function to connect an event user to an event generator. Note that a generator can have multiple users.

### Usage
```c++
Event0.set_generator(gen0::pin_pa0); // Set pin PA0` as event generator for Event0
Event0.set_user(user::evoutc);       // Set EVOUTC (pin PC2) as event user
Event0.set_user(user::evoutd);       // Set evoutD (pin PD2) as event user
```

### Event User table
Below is a table with all of the event users for the tinyAVR 0/1/2-series parts
#### Notes:
* The `evoutN_pin_pN7` is the same as `evoutN_pin_pN2` but where the pin is swapped from 2 to 7. This means that for instance, `evouta_pin_pa2` can't be used in combination with `evouta_pin_pa7`.
* Many of these refer to specific pins or peripherals - on smaller pin-count devices, some of these event users are not available; Attempting to set a user that doesn't exist will result in a compile error.
* This library papers over the fact that the 0/1-series numbered event channels to a peripheral 0 and 1, and everything else uses a and b. a and b are used here exclusively to refer to what the 0/1-series datasheets call 0 and 1.

| Event users             |                                    |
|-------------------------|------------------------------------|
| `user::ccl0_event_a`    | input 0 for 0/1-series             |
| `user::ccl0_event_b`    | input 1 for 0/1-series             |
| `user::ccl1_event_a`    | input 0 for 0/1-series             |
| `user::ccl1_event_b`    | input 1 for 0/1-series             |
| `user::ccl2_event_a`    | 2-series only                      |
| `user::ccl2_event_b`    | 2-series only                      |
| `user::ccl3_event_a`    | 2-series only                      |
| `user::ccl3_event_b`    | 2-series only                      |
| `user::adc0_start`      |                                    |
| `user::adc1_start`      | 16k and 32k 1-series only          |
| `user::evouta_pin_pa2`  |                                    |
| `user::evouta_pin_pa7`  | *evouta_pin_pa2 with pin swapped*  |
| `user::evoutb_pin_pb2`  |                                    |
| `user::evoutb_pin_pb7`  | *evoutb_pin_pb2 with pin swapped*  |
| `user::evoutc_pin_pc2`  |                                    |
| `user::evoutc_pin_pc7`  | *evoutc_pin_pc2 with pin swapped*  |
| `user::usart0_irda`     | sync channels only on 0/1-series   |
| `user::usart1_irda`     | 2-series only                      |
| `user::tca0_cnt_a`      | count on event or set direction, sync channels only on 0/1-series |
| `user::tca0_cnt_b`      | restart on event or set direction, 2-series only  |
| `user::tcb0_capt`       |                                    |
| `user::tcb0_cnt`        | 2-series only                      |
| `user::tcb1_capt`       | 2-series and 16/32k 1-series only  |
| `user::tcb1_cnt`        | 2-series only                      |
| `user::tcd0_in_a`       | input 0, 1-series only             |
| `user::tcd0_in_b`       | input 1, 1-series only             |


## clear_user()
Function to detach a user from a channel. Note that you don't need to know what channel to detach from, simply use `Event::clear_user()`.

### Usage
```c++
Event::clear_user(user::evouta); // Remove the user::evouta from whatever event channel it is connected to.
```

## soft_event()
Creates a single software event - users connected to that channel will react to it in the same way as they would to one caused by the generator the channel is connected to. Great if you have to force trigger something. Note that a software event only lasts a single system clock cycle, so it's rather fast! The software events will invert the channel, and so will trigger something regardless of whether it needs a the event channel to go high or low.

### Usage
```c++
Event0.soft_event(); // Create a single software event on Event0
```

## start()
Starts an event generator channel by writing the generator selected by `set_generator()` method to the `EVSYS.CHANNELn` register.

### Usage
```c++
Event0.start(); // Starts the Event0 generator channel
```

## stop()
Stops an event generator channel by clearing the `EVSYS.CHANNELn` register. The `EventN` object retains memory of what generator it was previously set to.=

### Usage
```c++
Event0.stop(); // Stops the Event0 generator channel
```
