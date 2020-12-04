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

## In practice, they can run WAY faster!
The speed grades above are specified with the expectation that customers will be putting them into devices where a malfunction could have considerable consequences for the end user - and through subsequent litigation - Microchip and their customers. In home and hobby settings - where a hardware crash couldn't result in a literal, ah, hardware crash, industrial disaster, or other catastrophe that would put lives (or worse yet, your company's stock price) at risk - it is possible to run these chips at significantly higher clock speeds and/or lower voltages at a given clock speed - at least in the vicinity of room temperature.
