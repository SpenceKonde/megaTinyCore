### Minimal Wiring Requirements:

These are the bare minimum requirements to bring the chip to life and program it using the Arduino IDE. The 0.1uf capacitors are *not* optional - you must use these or the parts will behave erratically, and may not work at all; these should be placed as close to the chip as possible.

* Vcc pin(s) connected to supply voltage (1.8~5v depending on part, clock speed, and fuse settings)
* Gnd pin(s) connected to Ground
* 0.1uf ceramic capacitor connected between Vcc and Gnd pins, as close to the part as practical.
* 4.7k resistor from programmer pin 6 to UPDI pin (see: [Making a cheap UPDI programmer](MakeUPDIProgrammer.md))

If the power supply is unstable, or the wires from the supply are long, it is highly recommended to put a larger capacitor between power and ground on the board - the appropriate size and type will depend on the loading, voltage, stability of the power supply, and available space; typical values might range from 10 uf to 1000uf. Aluminum electrolytic caps are commonly used for this purpose.
