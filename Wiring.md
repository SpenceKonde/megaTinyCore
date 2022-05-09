# Minimal Wiring Requirements

##These are the bare minimum requirements to bring the ATtiny chip to life and program it using the Arduino IDE:

* Vcc pin connected to supply voltage (1.8 - 5v depending on which ATtiny part, planned clock speed, and fuse settings)
* Gnd pin connected to Ground
* 0.1uf ceramic capacitor connected between Vcc and Gnd pins, as close to the part as practical. **This is NOT optional.** Per datasheet recommendations and best practices, an SMD MLCC capacitor on the same side of the PCB as the part is recommended, though a through-hole capacitor or an MLCC on the opposite of the PCB can be used as long as the leads and trace length are minimized.[^1]
* 100-470 ohm resistor between the UPDI pin and your UPDI programmer.[^2] (see: [Serial UDPI Programming](UPDI/jtag2updi.md))


[^1]:If the power supply is unstable, or the wires from the supply are long, it is highly recommended to put a larger capacitor between power and ground on the board - the appropriate size and type will depend on the loading, voltage, stability of the power supply, available space, etc. Values used might range from 4.7uf to 1000uf. Aluminum electrolytic caps are commonly used for this purpose, though modern ceramic capacitors are now available with values up to 100uF, are smaller and far cheaper (at least when <= 10uF), particularly for SMD configurations. Under typical conditions, a 4.7uF or 10uF cap is plenty for this purpose (though, I might suggest using one in at least 0805 package if using SMT because of how physical size effects the capacitance vs voltage curve).  Be aware that capacitance over 10uF may cause problems for the serial adapter being used for programming, due to "inrush current" when an empty capacitor is suddenly connected to a full one.  The adapter may reenumerate, and on some systems, the USB port might shut down requiring a host system reboot.

[^2]:This may be omitted if you are certain that the programmer you will always use has such a resistor in place. Total resistance between 220 ohms and around 6k ohms is expected to work; higher resistance may prevent programming, and lower resistance presents a (probably remote) risk of damage to the programmer and/or ATtiny device.
