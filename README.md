# Arduino Core for ATMEGA4809 CPU

This repository contains the source code and configuration files of the Arduino Core
for Microchip's ATMEGA4809 processor (used on the Arduino Uno WiFi Rev2 boards).

## Installation on Arduino IDE

This core is available as a package in the Arduino IDE cores manager.
Just open the "Boards Manager" and install the package called:

"Arduino megaAVR Boards"

## Support

There is a dedicated section of the Arduino Forum for general discussion and project assistance:

http://forum.arduino.cc/index.php?board=126.0

## Bugs or Issues

If you find a bug you can submit an issue here on github:

https://github.com/arduino/ArduinoCore-megaavr/issues

Before posting a new issue, please check if the same problem has been already reported by someone else
to avoid duplicates.

## Contributions

Contributions are always welcome. The preferred way to receive code cotribution is by submitting a 
Pull Request on github.

## Developing

1. Create an `<SKETCHBOOK>/hardware/arduino` folder. Where `<SKETCHBOOK>` is the location of your
  Arduino sketchbook.
1. Change directories: `cd <SKETCHBOOK>/hardware/arduino`
1. Clone this repo: `git clone https://github.com/arduino/ArduinoCore-megaavr.git megaavr`
1. Change directories: `cd megaavr/cores/arduino`
1. Copy or symlink the `api` folder from the [ArduinoCore-API](https://github.com/arduino/ArduinoCore-API) repo.
1. Restart the IDE

## License and credits

This core has been developed by Arduino SA in collaboration with Microchip.

```
  Copyright (c) 2018 Arduino SA.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
```