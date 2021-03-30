# PlatformIO

[PlatformIO](https://platformio.org) is an open source ecosystem for embedded development.
It has a built-in library manager and is Arduino compatible. It supports most operating systems; Windows, MacOS, Linux 32 and 64-bit, ARM and X86.

* [What is PlatformIO?](http://docs.platformio.org/en/latest/what-is-platformio.html)
* [PlatformIO IDE](https://platformio.org/platformio-ide)
* Getting started with [PlatformIO IDE](https://docs.platformio.org/en/latest/integration/ide/visualstudio.html) or [PlatformIO command line interface](https://docs.platformio.org/en/latest/core/index.html)
* [Advanced functionality](https://docs.platformio.org/en/latest/platforms/atmelmegaavr.html)
* [Project Examples](https://docs.platformio.org/en/latest/platforms/atmelmegaavr.html#examples)


## megaTinyCore + PlatformIO
megaTinyCore and PlatformIO works great together. You can upload using pyupdi dedicated UPDI programmer, and you can also let PlatformIO calculate the fuse values for you, just like Arduino IDE does!
Note that burning bootloader and uploading using the bootloader is not currently supported.

PlatformIO uses the information provided in platformio.ini to calculate what fuse bits and what bootloader file to load.
Simply provide enough information and run the following command:

```ini
; Only set fuses
pio run -t fuses -e set_fuses
```

You can find a platformio.ini template you can use when creating a project for a megaTinyCore compatible device below.
The most common functionality is available in this template. As you can see, the templated is divided into multiple environments.

* The default build environment are defined under *[platformio]*.
* All parameters that are common for all environments are defined under *[env]*.
* Use the *[env:Upload_UPDI]* to upload to your target.
* Use *[env:set_fuses]* to set the fuses or burn the bootloader.

More information on what each line means can be found further down on this page.


## platformio.ini template

```ini
; PlatformIO template configuration file for megaTinyCore
; https://github.com/SpenceKonde/megaTinyCore/
;
;   Build options: build flags, source filter
;   Hardware options: oscillator type, BOD, EEPROM retain
;   Upload options: custom upload port, speed, and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options
; https://github.com/SpenceKonde/megaTinyCore/blob/master/PlatformIO.md
; https://docs.platformio.org/page/projectconf.html
; https://docs.platformio.org/en/latest/platforms/atmelmegaavr.html

[platformio]
; Default build target
default_envs = Upload_UPDI

; Parameters used for all environments
[env]
platform = atmelmegaavr
framework = arduino

; Chip in use
board = ATtiny3217
; Clock frequency in [Hz]
board_build.f_cpu = 16000000L
; Oscillator type (internal or external)
board_hardware.oscillator = internal

; Unflag build flags
build_unflags =
; Extra build flags
build_flags =

; Monitor port is auto detected. Override here
;monitor_port =
; Serial monitor baud rate
monitor_speed = 9600


; Run the following command to upload with this environment
; pio run -e Upload_UPDI -t upload
[env:Upload_UPDI]
; Upload protocol for UPDI upload
upload_protocol = xplainedmini_updi
;upload_port = COMx
upload_flags =


; run the following command to set fuses
; pio run -t fuses -e set_fuses
[env:set_fuses]
; Upload protocol for used to set fuses
upload_protocol = ${env:Upload_UPDI.upload_protocol}
upload_flags =
; Hardware settings
board_hardware.bod = 2.7v
board_hardware.eesave = yes
board_hardware.updipin = updi

```


### `board`
PlatformIO requires the `board` parameter to be present. `board` is just the target, e.g `ATtiny412`.


### `board_build.f_cpu`
Specifies the clock frequency in [Hz].
Used to determine what oscillator option to choose. A capital *L* has to be added to the end of the frequency number.
Below is a table with supported clocks. Defaults to 16 MHz internal if not specified.

| Clock speed        | Oscillator | board_build.f_cpu     |
|--------------------|------------|-----------------------|
| 20 MHz             | Internal   | `20000000L`           |
| 16 MHz             | Internal   | `16000000L` (default) |
| 10 MHz             | Internal   | `10000000L`           |
| 8 MHz              | Internal   | `8000000L`            |
| 5 MHz              | Internal   | `5000000L`            |
| 4 MHz              | Internal   | `4000000L`            |
| 1 MHz              | Internal   | `1000000L`            |
| 20 MHz             | External   | `2000000L`            |
| 16 MHz             | External   | `16000000L`           |
| 10 MHz             | External   | `12000000L`           |
| 8 MHz              | External   | `8000000L`            |
| 24 MHz (overclock) | External   | `24000000L`           |
| 28 MHz (overclock) | External   | `28000000L`           |
| 32 MHz (overclock) | External   | `28000000L`           |


### `board_hardware.oscillator`
Specifies to use the internal or an external oscillator.

| Oscillator option    |
|----------------------|
| `internal` (default) |
| `external`           |


### `board_hardware.bod`
Specifies the hardware brown-out detection. Use `disabled` to disable.

| BOD              |
|------------------|
| `4.3v`           |
| `2.6v` (default) |
| `1.8v`           |
| `disabled`       |


### `board_hardware.eesave`
Specifies if the EEPROM memory should be retained when uploading using a programmer. Use `no` to disable.

| EEPROM retain   |
|-----------------|
| `yes` (default) |
| `no`            |


### `board_hardware.rstpin`
Specifies what functionality the UPDI pin should have. Be careful when selecting `gpio` or `reset`. The only way to recover the UPDI functionality is by using a 12V UPDI programmer!

| UPDI pin functionality    |
|---------------------------|
| `updi` (default)          |
| `gpio`                    |
| `reset`                   |


### `board_fuses`
You can also manually set the fuse values if you prefer this instead letting PlatformIO figuring them for you. The ones you don't manually specify _will_ be calculated by PlatformIO.

``` ini
[env:set_fuses]
; Upload protocol for used to set fuses
upload_protocol = ${env:Upload_UPDI.upload_protocol}
upload_flags =

; The following lines will override the calculated fuses
board_fuses.WDTCFG = [some value]
board_fuses.BODCFG = [some value]
board_fuses.OSCCFG = [some value]
board_fuses.TCD0CFG = [some value]
board_fuses.SYSCFG0 = [some value]
board_fuses.SYSCFG1 = [some value]
board_fuses.APPEND = [some value]
board_fuses.BOOTEND = [some value]
board_fuses.LOCKBIT = [some value]
```


### `build_unflags`
This parameter is used to unflag flags automatically set by the PlatformIO build environment.

**Example:**
`build_unflags =-flto -fpermissive`


### `build_flags`
This parameter is used to set compiler flags. This is useful if you want to for instance want to change the serial RX or TX buffer. Here's a list of the current available core files flags:

| Flag                        | Default size   | Description                                               |
|-----------------------------|----------------|-----------------------------------------------------------|
| -lprintf_flt                |                | Lets you print floats with printf (occupies ~1.5 kB)      |
| -Wall -Wextra               |                | Show on all compiler warnings                             |
| -DSERIAL_RX_BUFFER_SIZE=128 | 16 or 64 bytes | Sets the serial RX buffer to 128 bytes                    |
| -DSERIAL_TX_BUFFER_SIZE=128 | 16 or 64 bytes | Sets the serial TX buffer to 128 bytes                    |

**Example:**
`build_flags = -DSERIAL_RX_BUFFER_SIZE=128 -DSERIAL_TX_BUFFER_SIZE=128`


### `upload_port`
Holds the serial port used for uploading. Only needed if you're uploading using a JTAG2UPDI programmer. PlatformIO automatically detects the serial port. However, if you want to override this you can uncomment `upload_port`. Use `/dev/[port]` on Unix compatible systems, and use `COMx` on Windows.


### `upload_protocol`
Programmer used for uploading.

| Supported UPDI programmers in Avrdude | Notes                                                                                                            |
|---------------------------------------|------------------------------------------------------------------------------------------------------------------|
| `jtag2updi`                           | Requires upload port                                                                                             |
| `xplainedmini_updi`                   | Xplained Mini (mEDBG) and [microUPDI](https://www.tindie.com/products/MCUdude/microupdi-programmer/) programmers |
| `pkobn_updi`                          | On-board Curiosity nano programmer (nEDBG)                                                                       |
| `pickit4_updi`                        | PICkit4 programmer in UPDI mode                                                                                  |
| `snap_updi`                           | MPLAB SNAP programmer in UPDI mode                                                                               |
| `atmelice_updi`                       | Atmel ICE programmer in UPDI mode                                                                                |
| `xplainedpro_updi`                    | Xplained Pro in UPDI mode                                                                                        |
| `powerdebugger_updi`                  | Power Debugger in UPDI mode                                                                                      |


### `upload_flags`
Used to pass extra flags to Avrdude when uploading using a programmer.
Typical parameters are `-PUSB` and `-v`.
**Note that every flag has to be on its own line, and they have to be indented with two spaces:**
```ini
upload_flags =
  -PUSB
  -v
```


### `monitor_port`
PlatformIO detects serial ports automatically. However, if you want to override this you can uncomment `monitor_port`. Use `/dev/[port]` on Unix compatible systems, and use `COMx` on Windows.


### `monitor_speed`
Sets the serial monitor baud rate. Defaults to 9600 if not defined.


## pyupdi
[pyupdi](https://github.com/mraardvark/pyupdi) is a Python-based tool for programming tinyAVR and megaAVR devices with UPDI interface via a standard serial port. It can be installed directly in the PlatformIO virtual environment using the following command:

```
pip install https://github.com/mraardvark/pyupdi/archive/master.zip
```

Once pyupdi is installed it can be used as the uploader via a custom upload_command option:

```ini
[env:pyupdi_upload]
upload_protocol = custom
upload_speed = 115200
upload_port = /some/serial/port
upload_flags =
  -d
  $BOARD_MCU
  -c
  $UPLOAD_PORT
  -b
  $UPLOAD_SPEED
upload_command = pyupdi $UPLOAD_FLAGS -f $SOURCE
```
