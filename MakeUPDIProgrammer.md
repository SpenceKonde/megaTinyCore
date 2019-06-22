# Step-by-step guide to turn a uno/nano/pro mini into a UPDI programmer

The ATtiny 0- and 1-series are programmed through the Unified Program and Debug Interface (UPDI). This is a 1-wire interface using the reset pin on the ATtiny. The ATtiny does not have a bootloader, so an programmer is required to upload the sketches.
An Arduino sketch is available to turn most ATMEGA based Arduino’s, like the Arduino UNO and Nano, into an UPDI programmer. Follow the following steps for more information:
1.	The UPDI programmer sketch can be found here:
https://github.com/ElTangas/jtag2updi
Use your favorite method to clone or download this repository to you local machine. 
2.	Browse to the download location and open the jtag2updi folder
3.	Inside the jtag2updi there is a folder called “source”, rename this folder to jtag2updi and open the folder
4.	Open the sketch jtag2updi.ino, inside this folder, in Arduino
5.	Upload the sketch to your Arduino
6.	Connect the ATtiny UPDI pin (see pinout diagrams) to digital pin #6 on the Arduino using a 4.7KΩ resistor. 
7.	Connect the ground of the ATtiny to the ground pin of the Arduino
8.	If the ATtiny does not have its own power source, then the 3.3V or 5V from the Arduino can be used. 
9.	Change to the megaTinyCore board in the board manager.
10.	Upload a sketch to the ATtiny

