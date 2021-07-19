# Speed Grades

The datasheets for these parts specify the maximum officially supported operating frequency (F_CPU) for these parts over various voltage ranges. These speeds all have a BOD threshold voltage above which the parts are "guaranteed" to work. Like all AVR microcontrollers, the operating frequency can usually be pushed significantly above the official specs at room temperature (this shouldn't be relied upon in production, of course). Both the automotive and non-automotive versions are available in a -40-105C rated version, and a -40-125C version. These are identified by a letter at the end of the part number - an N or F (respectively) for non-automotive parts, and B and Z for the automotive versions; colloquially, they are sometimes referred to as "N-spec" or "F-spec" parts.

## Official Specs

Voltage | BOD voltage | Max F_CPU (-40~105C) | Max F_CPU (>105C, or any automotive)
--------|-------------|----------------------|----------------------
1.8~5.5 | 1.8 | 5 MHz | n/a
2.7~5.5 | 2.6 | 10 MHz | 8 MHz
4.5~5.5 | 4.2 | 20 MHz | 16 MHz

## Calculating for other voltages
The datasheet specifies that the maximum frequency is linear over the operating voltage range.
For all non-automotive parts, between -40 and 105C, maximum frequency is `5*(V/0.9-1) MHz` within 1.8 < V < 4.5
For F-spec parts above 105C, and all automotive parts maximum frequency is `8*(V/1.8-0.5) MHz` within 2.7 < V < 4.5

* Per datasheet, operation at each speed grade is guaranteed down to the specified BOD threshold.
* Automotive parts do not have a 20 MHz internal oscillator option at all. Both temperature ranges of non-automotive parts do.
* Both temperature ranges of non-automotive parts are rated for operation from 1.8 ~ 2.7V and 20 MHz at 4.5 ~ 5.5V, from -40 ~ 105C.
* Both the voltage and speed specs of the "F-spec" parts are reported in a confusing manner in many online catalogs.

## In practice, these can be run significantly faster
The speed grades above are specified with the expectation that customers will be putting them into devices where a malfunction could have considerable consequences for the end user, including but not limited to a horrible death in an automotive crash or industrial accident, and through subsequent litigation, the manufacturer and Microchip. They are also rated at those speeds up to temperatures in excess of the boiling point of water. I recommend sticking to these limits when the consequences of the part failing in an arbitrary way (imagine it could execute any instruction, correctly or incorrectly, in any order at any time) would result in anything beyond mild annoyance (I'd hesitate to use an overclocked part even if the only strike against it was that it was inconvenient to access to sertvice).

However, in home and hobby settings, you will not generally * be making devices where malfunction would cause some catastrophe that would put lives (or worse yet, your company's stock price) at risk, and most projects are also likely to be running at temperatures not far north of room temperature. Under those conditions, it is possible to run these chips at significantly higher clock speeds and/or lower voltages at a given clock speed - At room temp and 5v, the 0/1-series usually give out somewhere between 30 and 32 running from the internal oscillator (by aggressive tuning). The 2-series can usually do 32 internal @ 5v & room temp, and fail around 34 or 35. Extended temperature range parts usually overclock a little better than the non-extended-temperature ones, and external oscillators overclock better than tuned intoernal oscillator. Running from the internal oscillator and slowly increasing the calibration value, if you also do some math (maybe, say, getting micros or millis, and writing that value to a listener device on SPI, like

`*` If you *were* planning to make safety-critical devices and program them with this core, **please do not do that**. Let me be perfectly clear: **Neither this software package nor the Arduino IDE is appropriate** - the chips may be Functional Safety rated when run within manufacturer's specifications and programmed using their certified tools, likely according to a whole litany of conditions. That doesn't mean shit if you're using this core, or using Arduino, or any of the similar hobbyist grade development tools. That's why the licenses all have that boilerplate about having no warranty express or implied. I think I've make this abundantly clear about the core in multiple places. That means **you** would be the one who is liable when the device you never should have made malfunctions and kills someone. Please limit yourself to projects which will cause nothing worse than frustration and inconvenience in the event of failure, at least while using my tools.
