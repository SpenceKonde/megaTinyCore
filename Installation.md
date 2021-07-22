# Installing megaTinyCore

## Supported IDE versions
megaTinyCore requires a version of the IDE later than 1.6.3; - it may require a much more recent version of the IDE; this has not been tested. It has been tested with 1.8.9 and 1.8.13. Due to a regression in 1.8.15 megaTinyCore is not compatible with that IDE version. It errors out because it says there's no version, when the version as specified in platform.txt has been confirmed to be entirely valid.


## Boards Manager Installation (now strongly recommended unless helping with core developoment)

This board package can be installed via the board manager. The boards manager URL is:

`http://drazzy.com/package_drazzy.com_index.json`

1. File -> Preferences, enter the above URL in "Additional Boards Manager URLs"
2. Tools -> Boards -> Boards Manager...
3. Wait while the list loads (takes longer than one would expect, and refreshes several times).
4. Select "megaTinyCore by Spence Konde" and click "Install". For best results, choose the most recent version.

## Manual Installation
Manual installation allows the latest version of the core to be installed, with fixes that may not yet be available in the board manager version of the core. Manual installation is recommended if you are working on developing or modifying the core - however, the reqirements are brutal.

* You must be using a copy of the Arduino IDE that has never had an AVR board definition package installed on it (typically this means the .zip archive, extract, and create a portable folder inside before first run)
* You must update the toolchain. Search the .json file above `"tools": [` Of the 4 hits, you're looking for the avr-gcc one.
  * Scroll down to the most recent version (currently azduino4b)
  * Download and decompress the version for your OS. If the most recent version has no listing for your OS, go to the next most recent (this happens when I release a bad package for certain OS's and have to release a fixed version for those OS's only).
  * You will find an 'avr' directory containing several subdirectories. This may or may not be enclosed in one or more directories depending on what program is used to decompress it..
    * The directory structure is *very* confusing; I have botched this three times in released versions. There are a hell of a lot of nested "avr" directories.
  * Copy this into `arduino root folder)/hardware/tools` - if you did this right, you'll be told that hundreds of files are different. Replace them all!
  * **NOTE:** if you also intend to use DxCore manually installed, check the json entries for that one too, and see if it specifies a later toolchain version. Use whichever one has higher version (Azduinio4 is higher than Azduino3)

* If you want SerialUPFI, you need to also follow [megaavr/tools/ManualPython.md](megaavr/tools/ManualPython.md).

Once that all is done, you've only got a minor step or two left - you need to create a "hardware" folder inside the sketchbook folder (inside portable assumingh you went that route, which you should) amd then and only then should you instll the core.

* Option 1: Download the .zip package (either the "released" version, or by downloading the .zip of master repo), extract, and place in the hardware folder inside your sketchbook folder (if there is no hardware folder, create it). You can find/set the location of the sketchbook folder in the Arduino IDE at File > Preferences -> Sketchbook location.

* Option 2: Download the github client, and sync this repo to the hardware subfolder of your sketchbook folder. For as portable windows installation, it's typically IDE location/portable/sketchbook/hardware (you will have to create hardware.)


## [Arduino CLI] Installation & Management through CLI
### 1. Install latest Arduino-cli 
[Install latest Arduino-cli](https://arduino.github.io/arduino-cli/latest/installation/)
Or 
[upgrade to latest](https://arduino.github.io/arduino-cli/latest/UPGRADING/) (on Mac you can use `brew upgrade …`)

### 2. Install megaTinyCore
Setting an additional Boards Manager URL:

`arduino-cli core update-index --additional-urls http://drazzy.com/package_drazzy.com_index.json`

Also one can edit the config file. 

First get the PATH:
`arduino-cli config dump --verbose`

If there is no config file, create one first:
```
arduino-cli config init
arduino-cli config dump --verbose
```
Then edit/add:
```
aboard_manager:
  additional_urls:
  - http://drazzy.com/package_drazzy.com_index.json
```
Search and install core:
```
arduino-cli core search megaTinyCore
arduino-cli core install megaTinyCore:megaavr
```

### 3. Compile using `arduino-cli` for a `megaTinyCore` based chip:
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
```
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

3.3. So expanding on the example further, the options that I listed above, if I had to chain the specific fuse detail), following will be the command to compile teh sketch in the sketch directory:

```
arduino-cli compile -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,uartvoltage=skip --output-dir ./build/
```
Here I chose the build directory to be inside my sketch directory, where all the hex files etc will be exported.


### 4. Upload using `arduino-cli`:

When we used the command `arduino-cli board details -b <YOUR FQBN>` above, to see possible fuses and chip selection options, if we scroll down further and look for `Programmers:` section, we will see the programmers available for our specified FQBN. This information will come handy for uploading code.
i would be using `pyupdi57k` as my programmer option. 

Before we go further, have a look [HERE](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) to make sure off of your physical programming jig setup. 

From the sketch directory that has `build/` from previous step (if you have compiled once already) or else do not forget to compile first and then:

`arduino-cli upload -b FQBN:fuseKey=fuseValue,.. -p <Serial UPDI uploader PORT> -P <PROGRAMMER> -t`

For our example: 

```
arduino-cli upload -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,uartvoltage=skip -p /dev/tty.usbserial-A10KHTR4 -P pyupdi57k -t
```

### If you want to compile and then upload right after compilation:
Do the following. From your sketch directory:
```
arduino-cli compile -b FQBN:fuseKey=fuseValue,fuseKey=fuseValue,.. --output-dir ./build/ -u -p <Serial UPDI uploader PORT> -P <PROGRAMMER> -t
```
For our example that would be: 
```
arduino-cli compile -b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,uartvoltage=skip --output-dir ./build/ -u -p /dev/tty.usbserial-A10KHTR4 -P pyupdi57k -t
```
