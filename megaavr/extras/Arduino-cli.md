# Installation & Management through arduino-cli

If you are using the Arduino-cli, megaTinyCore can be used without much difficulty. The only complication is that you need to select values from all the tools menus... from the command line. This is done using the FQBN. It is not recommended to omit menus from that list. The rest of the core is written under the assumption that all are specified. Some have implicit default. Others do not. Any implicit default may not match normal menu default or your expectations. Future versions of the core may explicitly prevent compilation when menus are omitted in this way, as it suggests an invalid core configuration.
Credit for the following guide goes to @dattasaurabh82 - THANKS!

## Install latest arduino-cli

[Install latest Arduino-cli](https://arduino.github.io/arduino-cli/latest/installation/)
Or
[upgrade to latest](https://arduino.github.io/arduino-cli/latest/UPGRADING/) (on Mac you can use `brew upgrade …`)

## Install megaTinyCore

Setting an additional Boards Manager URL:

```bash
arduino-cli core update-index --additional-urls http://drazzy.com/package_drazzy.com_index.json
```

Also one can edit the config file.

First get the PATH:

```bash
arduino-cli config dump --verbose
```

If there is no config file, create one first:

```bash
arduino-cli config init
arduino-cli config dump --verbose
```

Then edit/add:

```yaml
aboard_manager:
  additional_urls:
  - http://drazzy.com/package_drazzy.com_index.json
```

Search and install core:

```bash
arduino-cli core update-index
arduino-cli core search megaTinyCore
arduino-cli core install megaTinyCore:megaavr
arduino-cli core upgrade
arduino-cli core update-index
```

__Note:__ Sometimes in environments like Github Actions, arduino-cli is not able to find the megaTinyCore with the above command.
The reason could be many folds. In runners like Github Actions, if the arduino-cli is not able to fetch the JSON file automatically, we can manually download the necessary JSON file and place it in the expected directory before running the core search or install command.

example:

```bash
wget http://drazzy.com/package_drazzy.com_index.json -O /home/runner/.arduino15/package_drazzy.com_index.json
arduino-cli core search megaTinyCore
arduino-cli core install megaTinyCore:megaavr
# ...
```

## Compile

Migrate to your sketch folder.

You can find `FQBN`(necessary for compiling) for your speecific ATtiny using:

```bash
arduino-cli board search megaTinyCore
```

For example: FQBN for `ATtint1607` without optiboot is:

```bash
megaTinyCore:megaavr:atxy7
```

__You ask:__ _FQBN is fine, but now, how do I specify the chip in cmdline like I select from the Arduino IDE's dropdown_?

You can find the Chip and other fuse details, like from Arduino IDE's _board dropboard_, in the _boards.txt_. On my mac the file path is: \

```bash
/Users/<user>/Library/Arduino15/packages/megaTinyCore/hardware/megaavr/<version>/boards.txt
```

You can also look from terminal the available menu options as list by using the command against your specific FQBN:

```bash
arduino-cli board details -b <YOUR FQBN>
```

For my example of ATTINY1607 without optiboot:

```bash
arduino-cli board details -b megaTinyCore:megaavr:atxy7
```

After going through the output list and picking on the fuses options, below chip and fuse options are the ones I want to use:

```yaml
  chip:           1607
  clock:          5internal
  millis:         enabled
  startuptime:    0
  bodvoltage:     1v8
  bodmode:        disabled
  eesave:         enable
  resetpin:       UPDI
  printf:         minimal
  wiremode:       mors
  WDTtimeout:     disabled
  WDTwindow:      disabled
  PWMmux:         A_default
  attach:         allenabled
```

Now, we have to basically chain these key value pairs, comma separating each key value pair, staring with a ":" after the supplied FQBN.

```bash
...FQBN:<fuse_uption1>=<fuse_uotion1_value>,<fuse_uption2>=<fuse_uotion2_value>,..
```

So, expanding on the example further, the options that I listed above, if I had to chain the specific fuse detail, following will be the command to compile the sketch in the sketch directory:

```bash
arduino-cli compile -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,millis=enabled,startuptime=0,bodvoltage=1v8,bodmode=disabled,eesave=enable,resetpin=UPDI,printf=minimal,wiremode=mors,WDTtimeout=disabled,WDTwindow=disabled,PWMmux=A_default,attach=allenabled --build-path $(pwd)/build
```

Here I chose the build directory to be inside my sketch directory, where all the hex files etc will be exported.

## Upload

When we used the command above,

```bash
arduino-cli board details -b <YOUR FQBN>
```

we will see see possible fuses and chip selection options.

If we scroll down further and look for `Programmers:` section, we will see the programmers available for our specified FQBN. This information will come handy for uploading code.

I would be using `serialupdi921k` as my programmer option.

Before we go further, have a look [HERE](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) to make sure of your physical programming jig setup.

From the sketch directory that has `build/` from previous step (if you have compiled once already) or else do not forget to compile first and then:

```bash
arduino-cli upload -b FQBN:fuseKey=fuseValue,.. -p <Serial UPDI uploader PORT> -P <PROGRAMMER> -t
```

For our example:

```bash
arduino-cli upload -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,wiremode=mors,printf=minimal,attach=allenabled -p /dev/tty.usbserial-A10KHTR4 -P serialupdi921k -t
```

## If you want to compile and then upload right after compilation

Do the following. From your sketch directory:

```bash
arduino-cli compile -b FQBN:fuseKey=fuseValue,fuseKey=fuseValue,.. --output-dir ./build/ -u -p <Serial UPDI uploader PORT> -P <PROGRAMMER> -t
```

For our example that would be:

```bash
arduino-cli compile -b -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,millis=enabled,startuptime=0,bodvoltage=1v8,bodmode=disabled,eesave=enable,resetpin=UPDI,printf=minimal,wiremode=mors,WDTtimeout=disabled,WDTwindow=disabled,PWMmux=A_default,attach=allenabled --build-path $(pwd)/build -u -p /dev/tty.usbserial-A10KHTR4 -P serialupdi921k -t
```
