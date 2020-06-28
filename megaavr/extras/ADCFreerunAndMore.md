# Introduction to free-running mode

ADCs can be configured to operate in what is called "free running" mode. In this mode, the ADC will measure the voltage on a specified input, and - as soon as that is done - start another measurement, or "conversion". For applications that require continually monitors an analog input (particularly a single one), this can be more convenient than requesting each read individually. More importantly, it is, effectively, much faster - analogRead() kicks off a single conversion, and then "busy-waits" for the conversion to complete - with the ADC sampling in the background with free-running mode, you can do other things during this time. This is critical when the sample rate is critical - and you must perform calculations on each reading as well.

Free-running mode is also particularly useful if you need to do something if the reading is above or below certain value - the modern AVR parts have a special feature just for this application: the Window Comparator mode - in this mode, you set a high and low threshold and put the ADC into free running mode - an interrupt will fire when the result meets the specified conditions (see below for more details). On the ATtiny1614, 1616, 1617, 3216, and 3217, there is actually a *second* ADC available. On these parts, free-running mode is even more useful - you can have the second ADC continually monitoring one input in free-running mode (either to use the results directly, or via the window comparator). 

## Enabling free-running mode 

## Reading Results

## 

# Faster ADC conversions?
If the signal you are measuring is low impedance, you 
