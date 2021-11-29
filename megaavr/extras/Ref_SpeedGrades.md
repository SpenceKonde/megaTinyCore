# Speed Grades

The datasheets for these parts specify the maximum officially supported operating frequency (F_CPU) for these parts over various voltage ranges. These speeds all have a BOD threshold voltage above which the parts are "guaranteed" to work. Like all AVR microcontrollers, the operating frequency can usually be pushed significantly above the official specs at room temperature (this shouldn't be relied upon in production, of course). Both the automotive and non-automotive versions are available in a -40-105C rated version, and a -40-125C version. These are identified by a letter at the end of the part number - an N or F (respectively) for non-automotive parts, and B and Z for the automotive versions; colloquially, they are sometimes referred to as "N-spec" or "F-spec" parts.

*Observing these limits is strongly recommended if you depend on reliable functioning of the device* and **mandatory for safety critical applications (which shouldn't be written in Arduino at all)**
Where failure or occasional hang or error is acceptable - it is possible to greatly exceed these limits.

## Official Specs

Voltage | BOD voltage | Max F_CPU (-40~105C)  | Max F_CPU (>105C, or any automotive)
--------|-------------|-----------------------|----------------------
1.8~5.5 | 1.8V        | 5 MHz                 | See note              |
2.7~5.5 | 2.6V        | 10 MHz                | 8 MHz                 |
4.5~5.5 | 4.2V        | 20 MHz                | 16 MHz                |

Note: Automotive parts, and extended temperature range parts in their extended temperature range, are not rated for operation below 2.7V (or rather, below the 2.6V BOD voltage).


There are also a number of other BOD settings which exist, but are not tested in production (as I understand that term to mean), and hence were removed from the datasheet so they wouldn't be claiming it did something if they didn't verify that it did. They do seem to work though - but they don't correspond to a specific maximum speed like the officially acknowleged ones do. The Nominal Vdd column indicates what the typical operation voltage one would pick this setting at.

| BOD Threshold | BOD code | Max F_CPU     |Nominal Vdd |
|---------------|----------|---------------|------------|
|          1.8V |      000 |  5 MHz/4 MHz  |2.0-2.5
|          2.1V |      001 | Unofficial    |
|          2.6V |      010 | 10 MHz/8 MHz  |3.0-3.3
|          2.9V |      001 | Unofficial    |
|          3.3V |      001 | Unofficial    |
|          3.7V |      001 | Unofficial    |
|          4.0V |      001 | Unofficial    |
|          4.2V |      111 | 20 MHz/10 MHz |5.0


Notice how the unofficial BOD settings (described as "unqualified" in a changelog about their removal from the io headers) roughly separate into 3 groups. One imagines that the chip was designed in the hopes that the lowest option in each range worked, and then they tested to determine which one was truly reliable under pessimal conditions; in this explanation, only the speeds > 10 MHz fell short of their best hopes.

Max F_CPU is the maximum speed that is guaranteed to work above the specified BOD voltage.

## Calculating for other voltages
The datasheet specifies that the maximum frequency is linear over the operating voltage range.
For all non-automotive parts, between -40 and 105C, maximum frequency is `5*(V/0.9-1) MHz` within 1.8 < V < 4.5
For F-spec parts above 105C, and all automotive parts maximum frequency is `8*(V/1.8-0.5) MHz` within 2.7 < V < 4.5

* Per datasheet, operation at each speed grade is guaranteed down to the specified BOD threshold.
* Automotive parts do not have a 20 MHz internal oscillator option at all. Both temperature ranges of non-automotive parts do.
* Both temperature ranges of non-automotive parts are rated for operation from 1.8 ~ 2.7V and 20 MHz at 4.5 ~ 5.5V, from -40 ~ 105C.
* Both the voltage and speed specs of the "F-spec" parts are reported in a confusing manner in many online catalogs.

## In practice, these go **way** faster
The speed grades above are specified with the expectation that customers will be putting them into devices where a malfunction could have considerable consequences for the end user, (such as a horrible death in an automotive crash as the antilock break controller freezes when it's actually activated and the airbag controller pulses the passenger airbag instead of the driver one, or industrial accident when the robot welding doesn't realize it's in maintenance mode, that that's a repair man coming towards it instead of a car door panel, or the chemical process equipment miscalculates the pressure and overfills a reaction chamber leading to an explosion spreading toxic fumes. The impacts of these on the user is severe, and through subsequent litigation and reputational damage,to  so is that on the manufacturer of the product and from there, Microchip itself. So with their corporate balance, jobs, and the lives of innocents on the line, they spec these things conservatively. Don't forget that the extended temperature ones They are also rated at those speeds up to temperatures in excess of the boiling point of water.

These parts can be clocked MUCH higher than this at room temperature and 5V. Even from the internal oscillator, many 1-series will hit 30, and it's rare to find a 2-series can can't do 32.  With an external clock, 1-series parts will normally work at 32 (at 5V and room temp). When an AVR is clocked too aggressively and begins malfunctioning, that typically manifests as the wrong results being calulated from correctly read instructions, but sometimes will manifest as wrong instructions as well. The usual end result is a "dirty reset" or hang.

I recommend sticking to these limits when the consequences of the part failing in an arbitrary way anything more severe than a mild nuisance, and even if a demon posessed the device, and caused it to malfunction in a maximally perverse way that was most likely to cause damage, you might be annoyed by the chip, but it's malfunctioning particularly badly couldn't damage anything, start a fire, or kill or injure anyone. I've never heard of the chip actually failing - just misbehaving if warm. I would, for example, not be very comfortable overclocking something that was tucked into a difficult to reach location. Certainly not aggressively.

However, in home and hobby settings, you will not generally * be making devices where malfunction would cause some catastrophe that would put lives (or worse yet, your company's stock price) at risk, and most projects are also likely to be running at temperatures not far north of room temperature. Under those conditions, it is possible to run these chips at significantly higher clock speeds and/or lower voltages at a given clock speed - At room temp and 5v, the 0/1-series usually give out somewhere between 30 and 32 running from the internal oscillator (by aggressive tuning). The 2-series can usually do 32 internal @ 5v & room temp, and fail around 34 or 35. Extended temperature range parts usually overclock a little better than the non-extended-temperature ones, and external oscillators overclock better than tuned intoernal oscillator. Running from the internal oscillator and slowly increasing the calibration value, if you also do some math (maybe, say, getting micros or millis, you will find that the math starts coming out wrong at the weird clock speeds expected 1's become 0's, etc).

`*` If you *were* planning to make safety-critical devices and program them with this core, **please do not do that**. Let me be perfectly clear: **Neither this software package nor the Arduino IDE is appropriate** - the chips may be Functional Safety rated when run within manufacturer's specifications and programmed using their certified tools, likely according to a list of conditions as long as their datasheets. But I'm quite certain that my core is NOT on that list. Nor is Arduino, or any of the similar hobbyist grade development tools.

That's why the licenses all have that boilerplate about having no warranty express or implied. I think I've make this abundantly clear about the core in multiple places.

That means **you** would be the one who is liable when the device you never should have made malfunctions and kills someone. Please limit yourself to projects which will cause nothing worse than frustration and inconvenience in the event of failure, at least while using my tools.
