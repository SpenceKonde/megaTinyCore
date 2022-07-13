# Event
A library for interfacing with the built-in Event system on the megaAVR-0 series MCUs, and ported to Dx-series and tinyAVR 0/1/2.
Developed by [MCUdude](https://github.com/MCUdude/).

**From the datasheet:**
> The Event System (EVSYS) enables direct peripheral-to-peripheral signaling. It allows a change in one peripheral (the event generator) to trigger actions in other peripherals (the event users) through event channels, without using the CPU. It is designed to provide short and predictable response times between peripherals, allowing for autonomous peripheral control and interaction, and also for synchronized timing of actions in several peripheral modules. It is thus a powerful tool for reducing the complexity, size, and execution time of the software.

## Overview
The event system allows routing of signals from event "generators" (outputs on various peripherals) through an event "channel", which one or more "event users" can be connected. When an event signal coming from the generator is is "active" or "high", the users connected to the same channel as the generator will perform certain specified actions depending on the peripheral. Generators are often the sorts of things that generate an interrupt if that is enabled - but some things can generate constant level events (such as following the state of a pin). The event users can do a wide variety of things. Op-amps can be enabled and disabled entirely through events, the ACD can kick off a staged measurement. Type A and B timers can count them, and type B timers can measure their duration or time between them with input capture. USARTs can even use them as their RX input! This is nice and all - but what really makes the Event system reach it's potential is CCL (configurable custom logic) which can use events as inputs, in addition to having access to several more internal sources of "event-like" signals - and being event generators in their own right.

More information about the Event system and how it works can be found in the [Microchip Application Note AN2451](http://ww1.microchip.com/downloads/en/AppNotes/DS00002451B.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf).

## Level vs. Pulse events
There are two types of events - a "pulse" interrupt, which lasts for the duration of a single clock cycle (either `CLK_PER` or a relevant (slower) clock - for example, the USART XCK generator provides a pulse event which lasts one XCK period, which is far slower than CLK_PER (the "peripheral; . while the TCD0-derived ones are) or a "level" interrupt which lasts for the duration of some condition. Often for a given even generator or user only one or the other makes sense. Less often, for some reason or another, you may need a level event, but all you have is a pulse event - or the other way around. A [CCL module (Logic.h)](../Logic/README.md) event between the two at the cost of the logic module and one event channel. In the case of timer WO (PWM) channels, the CCL already has level inputs to match the pulse inputs that the event system can get on compare match. Many of the pulse interrupts are generated at the same moment that interrupt flags get set - except that where an interrupt bit sticks around until serviced, the pulse bit is present generally for only a single clock cycle (meaning they are only *usable* for triggering other event system things or peripherals. s can be (they often let you do similar things faster with less CPU involvement, too), but unlike interrupts, they won't stick around until you explicitly clear them


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

Many parts with lower pincount do not have any options specific to a channel available. Level events are marked with an `L` on this chart; all others are pulses, with the clock domain named if it isn't CLK_PER (CPU clock speed)

| All Sync. (Event0 & Event1)| Event0 (EventSync0)      | Event1 (EventSync1)     | All Async. (Event2-5)   |   Event2 (EventAsync0)  |  Event3 (EventAsync1)   |  Event4 (EventAsync2)   |  Event5 (EventAsync3)    |
|----------------------------|--------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|--------------------------|
| `gens::disable`            | `gen0::disable`          | `gen1::disable`         | `gen::disable`          | `gen2::pin_pa0` LEVEL   | `gen3::pin_pb0` LEVEL   | `gen4::pin_pc0`  LEVEL  | `gen5::rtc_div8192` LEVEL|
| `gens::tcb0_capt`          | `gen0::pin_pc0` LEVEL    | `gen1::pin_pb0` LEVEL   | `gen::ccl_lut0` LEVEL   | `gen2::pin_pa1` LEVEL   | `gen3::pin_pb1` LEVEL   | `gen4::pin_pc1`  LEVEL  | `gen5::rtc_div4096` LEVEL|
| `gens::tca0_ovf_lunf`      | `gen0::pin_pc1` LEVEL    | `gen1::pin_pb1` LEVEL   | `gen::ccl_lut1` LEVEL   | `gen2::pin_pa2` LEVEL   | `gen3::pin_pb2` LEVEL   | `gen4::pin_pc2`  LEVEL  | `gen5::rtc_div2048` LEVEL|
| `gens::tca0_hunf`          | `gen0::pin_pc2` LEVEL    | `gen1::pin_pb2` LEVEL   | `gen::ac0_out` LEVEL    | `gen2::pin_pa3` LEVEL   | `gen3::pin_pb3` LEVEL   | `gen4::pin_pc3`  LEVEL  | `gen5::rtc_div1024` LEVEL|
| `gens::tca0_cmp0`          | `gen0::pin_pc3` LEVEL    | `gen1::pin_pb3` LEVEL   | `gen::tcd0_cmpbclr` TCD | `gen2::pin_pa4` LEVEL   | `gen3::pin_pb4` LEVEL   | `gen4::pin_pc4`  LEVEL  | `gen5::rtc_div512` LEVEL |
| `gens::tca0_cmp1`          | `gen0::pin_pc4` LEVEL    | `gen1::pin_pb4` LEVEL   | `gen::tcd0_cmpaset` TCD | `gen2::pin_pa5` LEVEL   | `gen3::pin_pb5` LEVEL   | `gen4::pin_pc5`  LEVEL  | `gen5::rtc_div256` LEVEL |
| `gens::tca0_cmp2`          | `gen0::pin_pc5` LEVEL    | `gen1::pin_pb5` LEVEL   | `gen::tcd0_cmpbset` TCD | `gen2::pin_pa6` LEVEL   | `gen3::pin_pb6` LEVEL   | `gen4::ac1_out`  LEVEL  | `gen5::rtc_div128` LEVEL |
|                            | `gen0::pin_pa0` LEVEL    | `gen1::pin_pb6` LEVEL   | `gen::tcd0_progev` TCD  | `gen2::pin_pa7` LEVEL   | `gen3::pin_pb7` LEVEL   | `gen4::ac2_out`  LEVEL  | `gen5::rtc_div64` LEVEL  |
|                            | `gen0::pin_pa1` LEVEL    | `gen1::pin_pb7` LEVEL   | `gen::rtc_ovf` CLK_RTC  | `gen2::updi` LEVEL      | `gen3::ac1_out` LEVEL   |                         | `gen5::ac1_out` LEVEL    |
|                            | `gen0::pin_pa2` LEVEL    | `gen1::tcb1_capt`       | `gen::rtc_cmp` CLK_RTC  | `gen2::ac1_out` LEVEL   | `gen3::ac2_out` LEVEL   |                         | `gen5::ac2_out` LEVEL    |
|                            | `gen0::pin_pa3` LEVEL    |                         |                         | `gen2::ac2_out` LEVEL   |                         |                         |                          |
|                            | `gen0::pin_pa4` LEVEL    |                         |                         |                         |                         |                         |                          |
|                            | `gen0::pin_pa5` LEVEL    |                         |                         |                         |                         |                         |                          |
|                            | `gen0::pin_pa6` LEVEL    |                         |                         |                         |                         |                         |                          |
|                            | `gen0::pin_pa7` LEVEL    |                         |                         |                         |                         |                         |                          |
|                            | `gen0::pin_pa2` LEVEL    |                         |                         |                         |                         |                         |                          |
|                            | `gen0::tcb1_capt`        |                         |                         |                         |                         |                         |                          |

Notes:
Yeah, all async channels have the AC1 and AC2 (assuming the part has it) but the numeric value isn't the same. Same goes for TCB1 and the sync channels.
There are no ADC event generators on 0/1-series. Nope, I dunno why either. It's unfortunate that on the only AVRs with 2 ADCs, we can't fire events with them!
There are no SPI or USART generators, though those are not as useful (note: you can still get them from the CCL)
The TCD events are async and **based on the TCD clock** *which may be faster than the peripheral clock*. If for example, you are feeding it to a CCL with the synchronizer enabled, events can be missed (this is considerably more concerning on the Dx-series parts, where the TCD has a PLL that can guarantee it is running faster than the system clock!)
RTC events are async pulse events, but they are based on the RTC clock domain, which is typically hundreds of times slower than the main clock.
While the analog comparator generators are available on every async channel, they are not interchangeable; they have different numerical values on each one!
There is a general appearance of the designers having made a few poor decisions, but not having had the time to correct them prior to release resulting in all this weirdness. This is supported by the fact that the megaAVR 0-series and all other chips released since have had an almost identical even system, to each other, and different from the tiny 0/1-series.

#### 2-series
On the 2-series parts, they put a synchronizer onto each channel, making it into two subchannels, a sync and async one, which is completely transparent to the user. The channels are far more flexible; like all other parts with an event system except the 0/1-series, only the port pins, and which set of taps off the PIT it gets are different between event channels. And they are delivered in pairs as they should be. Unlike, for example, the Dx-series, the event channels wrap around (the Dx-series has a number of channels that have no pin generators!)


|  All Pins                |  Event0                  |  Event1                  |  Event2                  |  Event3                 |  Event4                  |  Event5                 |
|--------------------------|--------------------------|--------------------------|--------------------------|-------------------------|--------------------------|-------------------------|
| `gen::disable`           | `gen0::disable`          | `gen1::disable`          | `gen2::disable`          | `gen3::disable`         | `gen4::disable`          | `gen5::disable`         |
| `gen::updi_synch` LEVEL  | `gen0::rtc_div8192` LEVEL| `gen1::rtc_div512` LEVEL | `gen2::rtc_div8192` LEVEL| `gen3::rtc_div512` LEVEL| `gen4::rtc_div8192` LEVEL| `gen5::rtc_div512` LEVEL|
| `gen::rtc_ovf` CLK_RTC   | `gen0::rtc_div4096` LEVEL| `gen1::rtc_div256` LEVEL | `gen2::rtc_div4096` LEVEL| `gen3::rtc_div256` LEVEL| `gen4::rtc_div4096` LEVEL| `gen5::rtc_div256` LEVEL|
| `gen::rtc_cmp` CLK_RTC   | `gen0::rtc_div2048` LEVEL| `gen1::rtc_div128` LEVEL | `gen2::rtc_div2048` LEVEL| `gen3::rtc_div128` LEVEL| `gen4::rtc_div2048` LEVEL| `gen5::rtc_div128` LEVEL|
| `gen::ccl0_out` LEVEL    | `gen0::rtc_div1024` LEVEL| `gen1::rtc_div64` LEVEL  | `gen2::rtc_div1024` LEVEL| `gen3::rtc_div64` LEVEL | `gen4::rtc_div1024` LEVEL| `gen5::rtc_div64` LEVEL |
| `gen::ccl1_out` LEVEL    | `gen0::pin_pa0` LEVEL    | `gen1::pin_pa0` LEVEL    | `gen2::pin_pc0` LEVEL    | `gen3::pin_pc0` LEVEL   | `gen4::pin_pb0` LEVEL    | `gen5::pin_pb0` LEVEL   |
| `gen::ccl2_out` LEVEL    | `gen0::pin_pa1` LEVEL    | `gen1::pin_pa1` LEVEL    | `gen2::pin_pc1` LEVEL    | `gen3::pin_pc1` LEVEL   | `gen4::pin_pb1` LEVEL    | `gen5::pin_pb1` LEVEL   |
| `gen::ccl3_out` LEVEL    | `gen0::pin_pa2` LEVEL    | `gen1::pin_pa2` LEVEL    | `gen2::pin_pc2` LEVEL    | `gen3::pin_pc2` LEVEL   | `gen4::pin_pb2` LEVEL    | `gen5::pin_pb2` LEVEL   |
| `gen::ac0_out`  LEVEL    | `gen0::pin_pa3` LEVEL    | `gen1::pin_pa3` LEVEL    | `gen2::pin_pc3` LEVEL    | `gen3::pin_pc3` LEVEL   | `gen4::pin_pb3` LEVEL    | `gen5::pin_pb3` LEVEL   |
| `gen::adc0_ready`        | `gen0::pin_pa4` LEVEL    | `gen1::pin_pa4` LEVEL    | `gen2::pin_pc4` LEVEL    | `gen3::pin_pc4` LEVEL   | `gen4::pin_pb4` LEVEL    | `gen5::pin_pb4` LEVEL   |
| `gen::adc0_sample`       | `gen0::pin_pa5` LEVEL    | `gen1::pin_pa5` LEVEL    | `gen2::pin_pc5` LEVEL    | `gen3::pin_pc5` LEVEL   | `gen4::pin_pb5` LEVEL    | `gen5::pin_pb5` LEVEL   |
| `gen::adc0_window`       | `gen0::pin_pa6` LEVEL    | `gen1::pin_pa6` LEVEL    | `gen2::pin_pa0` LEVEL    | `gen3::pin_pa0` LEVEL   | `gen4::pin_pb6` LEVEL    | `gen5::pin_pb6` LEVEL   |
| `gen::usart0_xck`  XCK0  | `gen0::pin_pa7` LEVEL    | `gen1::pin_pa7` LEVEL    | `gen2::pin_pa1` LEVEL    | `gen3::pin_pa1` LEVEL   | `gen4::pin_pb7` LEVEL    | `gen5::pin_pb7` LEVEL   |
| `gen::usart1_xck`  XCK1  | `gen0::pin_pb0` LEVEL    | `gen1::pin_pb0` LEVEL    | `gen2::pin_pa2` LEVEL    | `gen3::pin_pa2` LEVEL   | `gen4::pin_pc0` LEVEL    | `gen5::pin_pc0` LEVEL   |
| `gen::spi0_sck` SCK0     | `gen0::pin_pb1` LEVEL    | `gen1::pin_pb1` LEVEL    | `gen2::pin_pa3` LEVEL    | `gen3::pin_pa3` LEVEL   | `gen4::pin_pc1` LEVEL    | `gen5::pin_pc1` LEVEL   |
| `gen::tca0_ovf_lunf`     | `gen0::pin_pb2` LEVEL    | `gen1::pin_pb2` LEVEL    | `gen2::pin_pa4` LEVEL    | `gen3::pin_pa4` LEVEL   | `gen4::pin_pc2` LEVEL    | `gen5::pin_pc2` LEVEL   |
| `gen::tca0_hunf`         | `gen0::pin_pb3` LEVEL    | `gen1::pin_pb3` LEVEL    | `gen2::pin_pa5` LEVEL    | `gen3::pin_pa5` LEVEL   | `gen4::pin_pc3` LEVEL    | `gen5::pin_pc3` LEVEL   |
| `gen::tca0_cmp0`         | `gen0::pin_pb4` LEVEL    | `gen1::pin_pb4` LEVEL    | `gen2::pin_pa6` LEVEL    | `gen3::pin_pa6` LEVEL   | `gen4::pin_pc4` LEVEL    | `gen5::pin_pc4` LEVEL   |
| `gen::tca0_cmp1`         | `gen0::pin_pb5` LEVEL    | `gen1::pin_pb5` LEVEL    | `gen2::pin_pa7` LEVEL    | `gen3::pin_pa7` LEVEL   | `gen4::pin_pc5` LEVEL    | `gen5::pin_pc5` LEVEL   |
| `gen::tca0_cmp2`         | `gen0::pin_pb6` LEVEL    | `gen1::pin_pb6` LEVEL    |                          |                         |                          |                         |
| `gen::tcb0_capt`         | `gen0::pin_pb7` LEVEL    | `gen1::pin_pb7` LEVEL    |                          |                         |                          |                         |
| `gen::tcb0_ovf`          |                          |                          |                          |                         |                          |                         |
| `gen::tcb1_capt`         |                          |                          |                          |                         |                          |                         |
| `gen::tcb1_ovf`          |                          |                          |                          |                         |                          |                         |

Notes:
USART and SPI generators are pulses the length of a the clock on that peripheral.
RTC generators are pulses the length of one RTC clock cycle.
## Event User table
Below is a table with all of the event users for the tinyAVR 0/1/2-series parts

| Event users             |                                    |
|-------------------------|------------------------------------|
| `user::ccl0_event_a`    | "input 0" for 0/1-series           |
| `user::ccl0_event_b`    | "input 1" for 0/1-series           |
| `user::ccl1_event_a`    | "input 0" for 0/1-series           |
| `user::ccl1_event_b`    | "input 1" for 0/1-series           |
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

### Notes
* The `evoutN_pin_pN7` is the same as `evoutN_pin_pN2` but where the pin is swapped from 2 to 7. This means that for instance, `evouta_pin_pa2` can't be used in combination with `evouta_pin_pa7`.
* Many of these refer to specific pins or peripherals - on smaller pin-count devices, some of these event users are not available; Attempting to set a user that doesn't exist will result in a compile error.
* This library papers over the fact that the 0/1-series numbered event channels to a peripheral 0 and 1, and everything else uses a and b. a and b are used here exclusively to refer to what the 0/1-series datasheets call 0 and 1.

| Part Series | Event Channels | Chan/port | Layout  | 1port ch | pinless  |
|-------------|----------------|-----------|---------|----------|----------|
| AVR DB/DA   | 8 <48pin or 10 |         2 | Uniform |       2  |       2  |
| AVR DD      |   Always 6     |         2 | Uniform |    *2-4  |       0  |
| AVR EA      | Supposedly 6   |  Likely 2 | Unknown |2-4 likely|       0  |
| megaAVR 0   | 6 <48pin or 8  |         2 | Uniform |  2 or 0  |  0 or 2  |
| tinyAVR 2   |   Always 6     |         4 |AB-CA-BC |  2 or 0  |       0  |
| tinyAVR 1   | 2 sync 4 async |         2 |  Wacky  |       4  |1,2 or 4  |
| tinyAVR 0   | 1 sync 2 async |    1 or 2 |V. Wacky |  1 or 0  |       0  |



## Okay, so how do I read the state of these event channels?
You don't? Even though each channel on the 2-series supposedly has a synchronizer on it, they for some reason didn't pipe those synchronized bits to a register that we could read from software, that would seem to be the natural thing to do. I'm not sure why, or whether there was a well-reasoned decision making process around this, or it was simply an oversight. An argument can certainly be made "what are you doing reading it from software?" (to which the obvious answer is "Because it's not working and I'm trying to figure out why").

The best you can do is pipe the outputs to a pin and read the pin....


## The Event class
Class for interfacing with the Event systemn (`EVSYS`). Each event channel has its own object.
Use the predefined objects `Event0`, `Event1`, `Event2`, `Event3`, `Event4`, `Event5`. These, alternately, are known as `EventSync0`, `EventSync1`, `EventAsync0`, `EventAsync1`, `EventAsync2`, `EventAsync3` on the tinyAVR 0-series and 1-series. Additionally, there is an `Event_empty` that is returned whenever you call a method that returns an Event reference, but it can't fulfil your request. Note that not all generators are available on all channels; see the tables above. On 2-series, it's fairly simple, and, all else being equal, the first channels are the most useful, so the standard rule of thumb of "whenever you use a generator that can go anywhere, use the highest number port" holds. With so many options for each pin, relative to the number of pins, that's unlikely to be a problem. The 0 and 1-series oparts are more complicated. Obviously,on 8 pin parts, the two channels that can do PORTA are the most valuable, and the 1 or 3 channels without them a better choice, when you have a choice. The sixth channel, present only on 1-series, has the RTC division options in place of pins. It is either critical to your application, or the least useful, with no middle ground.


### Class Overview

| Method            | Argument/Return Type                                                     | Function                                |
|-------------------|--------------------------------------------------------------------------|-----------------------------------------|
| get_channel_number() | Returns channel number as a uint8_t                                   | For working with references             |
| get_generator()   | returns uint8_t indicating generator number.                             |                                         |
| set_generator()   | gen::, genN:: or pin number                                              | Sets the specified source as generator  |
| set_user()        | user::user_t                                                             | Sets specified user to this channel     |
| set_user_pin();   | valid EVOUT pin number, returns -1 if not valid pin, else user::user_t   | Set the event user to a pin             |
| soft_event()      |                                                                          | Inverts the channel for one clock cycle |
| long_soft_event() | duration of event, 4, 6, 10, or 16 - in clock cycles.                    | Inverts the channel for longer          |
| start()           |                                                                          | Enables this channel                    |
| stop()            |                                                                          | Disables this channel                   |


These class methods return a reference to an event channel (an Event&), or Event_empty if none can be found.
| Class Method                    | Argument Type     | Function                                      |
|---------------------------------|-------------------|-----------------------------------------------|
| Event::get_channel()            | uint8_t           | Get channel by number                         |
| Event::get_generator_channel()  | gen::generator_t  | Get channel by generator (first match only)   |
| Event::get_generator_channel()  | uint8_t           | Get channel by pin number                     |
| Event::get_user_channel()       | user::user_t      | Get channel that user is set to               |
| Event::assign_generator()       | gen::generator_   | return channel that has that generator,<br/> pick one and set if none currently set |
| Event::assign_generator_pin()   | uint8_t           | return channel that has that pin as generator,<br/> pick one and set if none currently set |

Class methods for working with users or looking up generator or user numbers
| Class Method                     | Argument Types               |                                                  |
|----------------------------------|------------------------------|--------------------------------------------------|
| Event::get_user_channel_number() | user::user_t, returns int8_t | Returns the event channel number user set to.    |
| Event::clear_user()              | user::user_t                 | Disconnect specified user from any event channel |
| Event::gen_from_peripheral()     | TCA_t, TCB_t, `CCL`, or AC_t | Return the generator for this peripheral <br/> second argument specified which generator, if several |
| Event::user_from_peripheral()    | TCA_t, TCB_t, `CCL`, or USART_t | Return the user for this peripheral <br/> second argument specifies which user, if several |

For the CCL, when looking up a generator, the second argument is the logic block number, for looking up a user, it is twice the logic block number for input A



### get_channel_number()
Method to get the current channel number. Useful if the channel object has been passed to a function as reference.

#### Usage
``` c++
uint8_t what_channel(Event& event_chan) {
  event_chan.get_channel_number();
}
```
### get_channel()
Static method. This returns the event object of that number. This is useful when you are programmatically choosing channels at runtime; It's very hard to get from the integer 3, for example, to the Event3 token you'd need to use to refer to it without this method, all you can do is have a big long if/elseif or switch/case structure to find the one you want. If the channel is not valid, you are instead given `Event_empty`

#### Usage
```c++

&Event MyEventChannel = Event::get_channel(MyUsedChannelCount);
if (MyEventChannel.get_channel_number() == 255) {
  // damn, out of channels.
  return;
} else {
  MyUsedChannelCount++;
}
```

### get_user_channel_number()
Method to get what event channel a user is connected to. Returns -1 if not connected to any channel. Note that we use `user::` as prefix when we refer to event users. This is a static method - you don't have to specify an object to determine what channel the user is connected to. An event channel, and hence an event generator, can have as many event users are you want - but an event user can only have one event generator. You cannot get a list or count of all users connected to a generator except by iterating over the list.

#### Usage
```c++
uint8_t connected_to = Event::get_user_channel_number(user::ccl0_event_a); // Returns the channel number ccl0_event_a is connected to
```

### get_user_channel()  **Changed in 1.2.0**
What used to be called this is now get_user_channel_number();
Method to get the channel a user is connected to; unlike get_user_channel_number() this returns an `&Event` - that is, a reference to the Event object.

#### Usage
```c++
&Event MyEventChannel = Event::get_user_channel(user::ccl0_event_a); // don't know what that channel is nor do care, as long I can point the AC at it instead.
MyEventChannel.set_generator(gen::ac0_out);

```

### set_generator()
Method to set the event generator of a channel. This is member method, and can only be called on an existing event object (contrast with the assigmn_generator(), which also finds a suitable channel) Note that we use the prefix genN:: (where N is the channel number) when referring to generators unique to this particular channel. we use gen:: when referring to generators available on all generators.

This can ALSO take as it's argument an arduino pin number. Assuming the pin exists and is a valid generator for that channel, this will set it as the input. The codepath for set_generator(pin) is more efficient than that for set_generator(genN::pin_pxn);

#### Usage
```c++
Event0.set_generator(gen::ccl0_out); // Use the output of logic block 0 (CCL0) as an event generator for Event0
Event2.set_generator(gen2::pin_pc0); // Use pin PC0 as an event generator for Event2
Event2.set_generator(PIN_PC0);       // Use pin PC0 as an event generator for Event2, more efficiently.
```

### get_generator()
Method to get the generator used for a particular channel.

#### Usage
```c++
uint8_t generator_used = Event0.get_generator();
if (generator_used == gen::ccl0_out) {
  Serial.println("We're using gen::ccl0_out as generator");
}
```

### assign_generator()
This is a static method it can be called without referencing a specific channel. You pass the name of any generator to it. The "Assign" methods perform basic event channel management. This method will check all applicable channels in the hopes of finding another channel that is already pointed at that generator - if it does, it will return a reference to that Event channel. Failing that, it will check every channel, looking for ones that are not connected to any other generators. Assuming it finds one, it will set it to use that generator and return a reference to that channel. If there are no channels pointing at that generator, nor any that are not connected to any generators, this will return Event_empty.

```c++
&Event MyEventChannel = Event::assign_generator(gen::ccl1_out);
if MyEventChannel.get_channel_number() != 255 {
  doStuffWithIt();
}
```
While it is legal to pass things like `gen1::pin_pa2` that is less efficient than just calling.....

### assign_generator_pin()
This static method works exactly the same as assign_generator - but it is meant to operate on pin numbers, not generator names. If assign_generator determines that that's what you passed it, it will call this, but it's obviously more efficient to call it directly. Passing invalid pins will also get you Empty_event.

### Usage
```c
&Event pinEvent = Event::assign_generator_pin(PIN_PC2);
if pinEvent.get_channel_number() != 255 {
  Serial.println("Oh laaaaame! No channels that can use the pin are available?!");
}
```

### set_user()
Method to connect an event user to an event generator. Note that a generator can have multiple users.

#### Usage
```c++
Event0.set_generator(gen0::pin_pa0); // Set pin PA0` as event generator for Event0
Event0.set_user(user::evoutc);       // Set EVOUTC (pin PC2) as event user
Event0.set_user(user::ccl0_event_a); // Also set CCL0 Event 0/Event A in event user (See Logic library)
```

### set_user_pin()
This is the analog of set_user above - the difference being that it takes a pin number as an argument. This must be a pin that supports event output - the number of the pin within the port must be 2 or 7, and you cannot use both pin 2 and pin 7 of the same port at the same time.

```c++
Event0.set_user_pin(PIN_PB2); //
```
Note: if you're totally out of event out pins, but not CCL blocks, you can set up a CCL to use the event channel as an input, and and use a CCL output pin; for example, if you set input 0 to be the event channel and masked the other inputs (and if this is all you're doing with it, that's what you'd do), you would want to set the truth table to 0x02.


### clear_user()
Method to detach a user from a channel. This is a static method - you don't need to know what channel to detach from, simply use `Event::clear_user()`.

#### Usage
```c++
Event::clear_user(user::evouta); // Remove the user::evouta from whatever event channel it is connected to.
```

### soft_event()
Creates a single software event - users connected to that channel will react to it in the same way as they would to one caused by the generator the channel is connected to. This can be used either to force something which could occur naturally (ex: for testing) - or the your use case may be predicated on pulses being entirely software generated in this way. Note that a software event only lasts a single system clock cycle - so it will not pass through a CCL's filter (for example) which could be useful, but is more likely to be undesirable. The software events will invert the channel, and so will trigger something regardless of whether it needs a the event channel to go high or low.

#### Usage
```c++
Event0.soft_event(); // Create a single software event on Event0
```

### long_soft_event()
soft_event() is only one system clock long. Often you need something a little longer. 2 clocks? 4 clocks maybe? Maybe it needs to get past the 'filter' on a CCL... Or you've got 2 things that need to happen a fraction of a microsecond apart; you've tried triggering one on the rising edge of a soft_event and the other on the falling edge, but it's just a hair too fast, and the other obvious solutions aren't viable due to the specifics of your scheme. The only way that a soft-event can last multiple system clocks is if the register is written every clock cycle. There is no time for a loop; only brute force (ie, a contiguous block of 'ST'ore instructions) will do the trick. Now in many ways this defeats the point of the event system; however, there are times when it is not entirely unreasonable to do this (as noted above), particularly if doing weird things with the CCLs, event system, and other peripherals.

The lengths that are available are 2, 4, 6, 10 and 16 (any number less than 4 will give 2 clock-long pulse, only 4 will give a 4 clock long one. Anything between 4 and 10 will give 6, exactly 10 will give 10, and anything larger will give 16). Those numbers were chosen arbitrarily to keep the size small, and give a selection that covered the reasonable use cases I could think of. If you need longer, you shouold definitely use a different approach.

#### Usage
```c++
Event0.long_soft_event(4); // will invert the state of the event channel for 4 system clock cycles (200ns at 20 MHz)
Event0.long_soft_event(10); // will invert the state of the event channel for 10 system clock cycles (500ns at 20 MHz)

```
Don't forget that this is an invert, not a "high" or "low". It should be entirely possible for the event that normally drives it to occur resulting in the state changing during that pulse, depending on it's configuration. Note also that the overhead of long_soft_event is typically several times the length of the pulse due to calculating the bitmask to write; it's longer with higher numbered channels. if that is a problem, whatever your use case is, it is not one of the ones this was intended for...

#### Usage
```c++
Event0.soft_event(); // Create a single software event on Event0
```

### start()
Starts an event generator channel by writing the generator selected by `set_generator()` method to the `EVSYS.CHANNELn` register.

#### Usage
```c++
Event0.start(); // Starts the Event0 generator channel
```

### stop()
Stops an event generator channel by clearing the `EVSYS.CHANNELn` register. The `EventN` object retains memory of what generator it was previously set to.

#### Usage
```c++
Event0.stop(); // Stops the Event0 generator channel
```

### **gen_from_peripheral()** and **user_from_peripheral()**
These two static methods allow you to pass a reference to a peripheral module, and get back the generator or user associated with it. In this context the "Peripheral Modules" are the structs containing the registers, defined in the io headers; for example `TCB0` or `USART1` or `CCL`.

This is most useful if you are writing portable (library) code that uses the Event library to interact with the event system. Say you made a library that lets users make one-shot pulses with timerB. You're not particularly interesting in getting your hands too dirty with the event system especially considering just how filthy the event system is on the 0/1-series. So you use the Event library to handle that part. You would of course need to know which timer to use - the natural way would be to ask the user to pass a reference or pointer. But then what? The fact that you've got the pointer to something which, as it happens, is TCB0 (which itself is annoying to determine from an unknown pointer)... though even KNOWING THAT, you're not able to use it with the event library, since it needs user::tcb0 (or user::tcb0_capt). As of the version distributed with megaTinyCore 2.5.0 and DxCore 1.4.0, there's a method for that. As the names imply, one gives generators, the other gives users. They take 2 arguments, the first being a pointer to a peripheral struct. The second, defaulting to 0, is the "type" of generator or user. Some peripherals have more than one event input or output. These are ordered in the same order as they are in the tables here and in the datasheet listings.

#### Usage
```c
// Here we see a typical use case - you get the generator, and immediately ask Event to assign a channel to it and give that to you. After getting it, you test to make sure it's not Event_empty, which indicates that either gen_from_peripheral failed, or assign_generator was out of event channels. Either way that's probably the user's fault, so you decide to return an error code.
uint8_t init(TCB_t* some_timer, /*and more arguments, most likely */) { // User could pass any TCB, and will expect your code to work!
  &Event_TCBnCapt = Event::assign_generator(Event::gen_from_peripheral(some_timer,0));
  if (_TCBnCapt.get_channel_number() == 255)
    return MY_ERROR_INVALID_TIMER_OR_NO_FREE_EVENT;
  doMoreCoolStuff();
}

```

Shown below, generators/user per instance  (second argument should be less than this; zero-indexed), and the number of instances (for reference)

| Peripheral |   tiny0  |   tiny1  |   tiny2  |   mega0  |  DA/DB/DB   |     EA     |
|------------|----------|----------|----------|----------|-------------|------------|
| TCAn       | 5 / 1 x1 | 5/1 x1   | 5 / 2 x1 | 5 / 1 x1 |  5 / 2 x1-2 | 5 / 2 x2   |
| TCBn       |    ***   |    ***   | 2 / 2 x2 | 1/1 x3-4 |  2 / 2 x2-5 | 2 / 2 x3-4 |
| CCL **     | 2 / 4    | 2 / 4    | 4 / 8    | 4 / 8    |4-6 / 8-12   | 4 / 8      |
| ACn        | 1 / 0 x1 | */0 x1-3 | 1 / 0 x1 | 1 / 0 x1 |  1 / 0 x1-3 | 1 / 0 x1-2 |
| USARTn     | 0 / 1 x1 | 0 / 1 x1 | ! / 1 x2 | !/1 x3-4 |  ! / 1 x2-6 | ! / 1 x2-3 |

`*` - the tiny1 parts with 1 AC work normally. This is unfortunately not supported for tiny1 parts with the triple-AC configuration:
`**` - There is only one CCL peripheral, with multiple logic blocks. Each logic block has 1 event generator and 2 event users. If using the logic library, get the Logic instance number. The output generator is that number. The input is twice that number, and twice that number + 1.
`***` - This peripheral is not supported because the generator numbers are channel dependent.

`!` - These parts do have an option, but we didn't bother to implement it because it isn't particularly useful. But the Event RX mode combined with the TX input to the CCL permit arbitrary remapping of RX and very flexible remapping of TX.

And what they are:

| Peripheral |   TCAn   | TCBn |  CCL**  | ACn | USARTn  |
|------------|----------|------|---------|-----|---------|
| gen 0      | OVF/LUNF | CAPT | LUT0OUT | OUT |    !    |
| gen 1      |     HUNF | *OVF | LUT1OUT |     |         |
| gen 2      |     CMP0 |      | LUT2OUT |     |         |
| gen 3      |     CMP1 |      | LUT3OUT |     |         |
| gen 4      |     CMP2 |      |   ETC.  |     |         |
| user 0     |   EVACTA | CAPT | LUT0EVA |  -  | EVENTRX |
| user 1     | * EVACTB | *CNT | LUT0EVB |     |         |
| user 2     |          |      | LUT1EVA |     |         |
| user 2     |          |      | LUT1EVB |     |         |
| user 2     |          |      |   ETC.  |     |         |

`*` - indicates that this is not available on all parts
`**` - Since there's only one CCL, the pointer (or rather, its type) is just used to select which implementation is used. But this does mean that the CCL can have an insane number of options. But that's fine, because there are plenty of numbers between 0 and 255.
`!` - These parts do have an option, but we didn't bother to implement it because it isn't particularly useful. But the Event RX mode combined with the TX input to the CCL permit arbitrary remapping of RX and very flexible remapping of TX!


Asking for a generator that doesn't exist will return 0 (disabled); be sure to check for this in some way. Asking for a user that doesn't exist will return 255, which the library is smart enough not to accept. The most likely way this will happen is if you request with code written for TCA or TCB that needs one of the new features added with 2-series and Dx-series.
