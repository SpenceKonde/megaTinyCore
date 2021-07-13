# Installing megaTinyCore

## Supported IDE versions
megaTinyCore requires a version of the IDE later than 1.6.3; - it may require a much more recent version of the IDE; this has not been tested. It has been tested with 1.8.9 and 1.8.13. Due to a regression in 1.8.15 megaTinyCore is not compatible with that IDE version. It bombs ouit because it says there's no version, when the version as specified in platform.txt has been confirmed to be entirely valid.


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
* You must update the toolchain. Search the .json file above for the latest version, and it will list under tools-dependencies a version of avr-gcc. Search the file for that and you'll wind up at a block of links.
  * Download and decompress the version for your OS.
  * You will find an 'avr' folder containing several more folders.
    * The directory structure is verty confusing. There are a hell of a lot of nested "avr" folders/directories.
  * Copy this into (ardino instance which has never had board manager installations root folder)/hardware/tools - if you did this right, you'll be told that hundreds of files are different. Replase them all!
  * **NOTE:** if you also intend to use DxCore manually installed, check that one too, and see if it specifies a later toolchain version. Use whichever one has higher version (Azduinio4 is higher than Azduino3)

* If you want SerialUPFI, you need to also follow megaavr/tools/ManualPython.md.

Once that all is done, you've only got a minor step or two left - you need to create a "hardware" folder inside the sketchbook folder (inside portable assumingh you went that route, which you should) amd then and only then should you instll the core.

* Option 1: Download the .zip package (either the "released" version, or by downloading the .zip of master repo), extract, and place in the hardware folder inside your sketchbook folder (if there is no hardware folder, create it). You can find/set the location of the sketchbook folder in the Arduino IDE at File > Preferences -> Sketchbook location.

* Option 2: Download the github client, and sync this repo to the hardware subfolder of your sketchbook folder. For as portable windows installation, it's typically IDE location/portable/sketchbook/hardware (you will have to create hardware.)
