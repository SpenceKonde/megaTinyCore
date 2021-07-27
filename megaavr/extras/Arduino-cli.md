# [Arduino CLI] Installation & Management through CLI
If you are using the Arduino-cli, megaTinyCore can be used without much difficulty. The only complication is that you need to select values from all the tools menus... from the command line. This is done using the FQBN. It is not recommended to omit menus from that list. The rest of the core is written under the assumption that all are specified. Some have implicit default. Others do not. Any implicit default may not match normal menu default or your expectations. Future versions of the core may explicitly prevent compilation when menus are omitted in this way, as it suggests an invalid core configuration.
Credit for the following guide goes to @dattasaurabh82 - THANKS!

## 1. Install latest Arduino-cli
[Install latest Arduino-cli](https://arduino.github.io/arduino-cli/latest/installation/)
Or
[upgrade to latest](https://arduino.github.io/arduino-cli/latest/UPGRADING/) (on Mac you can use `brew upgrade …`)

## 2. Install megaTinyCore
Setting an additional Boards Manager URL:

`arduino-cli core update-index --additional-urls http://drazzy.com/package_drazzy.com_index.json`

Also one can edit the config file.

First get the PATH:
`arduino-cli config dump --verbose`

If there is no config file, create one first:
```sh
arduino-cli config init
arduino-cli config dump --verbose
```
Then edit/add:
```text
aboard_manager:
  additional_urls:
  - http://drazzy.com/package_drazzy.com_index.json
```
Search and install core:
```sh
arduino-cli core search megaTinyCore
arduino-cli core install megaTinyCore:megaavr
```

## 3. Compile using `arduino-cli` for a `megaTinyCore` based chip:
Migrate to your sketch folder.

You can find `FQBN`(necessary for compiling) for your speecific ATtiny using:
`arduino-cli board search megaTinyCore`

For example: FQBN for `ATtint1607` without optiboot is `megaTinyCore:megaavr:atxy7`

**You ask:** _FQBN is fine, but now, how do I specify the chip in cmdline like I select from the Arduino IDE's dropdown_?

You can find the Chip and other fuse details, like from Arduino IDE's _board dropboard_, in the **_boards.txt_**. On my mac the file path is: `/Users/<user>/Library/Arduino15/packages/megaTinyCore/hardware/megaavr/<version>/boards.txt`

From there, you can see All the details as per specific chips and pin numbers etc.
So let's say, for our example's sake, I want to use a:
3.1. 24 pin ATTINY1607 without optiboot
3.2. After testing from Arduino IDE, the fuse settings, the options I'm happy with, are as below:
![Screenshot 2021-07-21 at 12 04 24 AM](https://user-images.githubusercontent.com/4619862/126485512-83e0ff2a-48d6-4c63-86a7-12910361e4ef.png)

You can also look from terminal the available menu options as list by using the command against your specific FQBN:
`arduino-cli board details -b <YOUR FQBN>`

For my example of ATTINY1607 without optiboot: `arduino-cli board details -b megaTinyCore:megaavr:atxy7`
After going through the output list and picking on the fuses options, below chip and fuse options are the ones I want to use:
```text
Options:
  chip=1607
  clock=5internal
  bodvoltage=1v8
  bodmode=disabled
  eesave=enable
  millis=enabled
  resetpin=UPDI
  startuptime=0
  uartvoltage=skip
```
Now we have to basically chain these key value pairs, comma separating each key value pair, staring with a ":" after the supplied FQBN.

`...FQBN:<fuse_uption1>=<fuse_uotion1_value>,<fuse_uption2>=<fuse_uotion2_value>,..`

3.3. So expanding on the example further, the options that I listed above, if I had to chain the specific fuse detail), following will be the command to compile the sketch in the sketch directory:

```sh
arduino-cli compile -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,uartvoltage=skip --output-dir ./build/
```
Here I chose the build directory to be inside my sketch directory, where all the hex files etc will be exported.


## 4. Upload using `arduino-cli`:

When we used the command `arduino-cli board details -b <YOUR FQBN>` above, to see possible fuses and chip selection options, if we scroll down further and look for `Programmers:` section, we will see the programmers available for our specified FQBN. This information will come handy for uploading code.
i would be using `pyupdi57k` as my programmer option.

Before we go further, have a look [HERE](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) to make sure off of your physical programming jig setup.

From the sketch directory that has `build/` from previous step (if you have compiled once already) or else do not forget to compile first and then:

`arduino-cli upload -b FQBN:fuseKey=fuseValue,.. -p <Serial UPDI uploader PORT> -P <PROGRAMMER> -t`

For our example:

```sh
arduino-cli upload -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,uartvoltage=skip -p /dev/tty.usbserial-A10KHTR4 -P pyupdi57k -t
```

## If you want to compile and then upload right after compilation:
Do the following. From your sketch directory:
```sh
arduino-cli compile -b FQBN:fuseKey=fuseValue,fuseKey=fuseValue,.. --output-dir ./build/ -u -p <Serial UPDI uploader PORT> -P <PROGRAMMER> -t
```
For our example that would be:
```sh
arduino-cli compile -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,uartvoltage=skip --output-dir ./build/ -u -p /dev/tty.usbserial-A10KHTR4 -P pyupdi57k -t
```
