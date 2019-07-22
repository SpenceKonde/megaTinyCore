## Supported IDE versions
megaTinyCore requires a version of the IDE later than 1.6.3; - it may require a much more recent version of the IDE; this has not been tested. It has been tested with 1.8.9. 

For all manual installations, and board manager installation of 1.0.1 and earlier, **The Official Arduino megaAVR board package must be installed using board manager in order for this package to work**. Board manager of 1.0.2 no longer requires this step. 

## Boards Manager Installation

This board package can be installed via the board manager. The boards manager URL is:

`http://drazzy.com/package_drazzy.com_index.json`

1. File -> Preferences, enter the above URL in "Additional Boards Manager URLs"
2. Tools -> Boards -> Boards Manager...
3. Select "megaTinyCore by Spence Konde" and click "Install".
4. If you installed version 1.0.1 or 1.0.0 of megaTinyCore, find the "Official Arduino megaAVR boards" package, and install the most recent version. You may skip this step if you installed version 1.0.2. 

## Manual Installation
Manual installation allows the latest version of the core to be installed, with fixes that may not yet be available in the board manager version of the core. **You must use board manager to install the latest version of the Official Arduino megaAVR board package** for manual installation to work. Manual installation is recommended if you are working on developing or modifying the core, or if you need the latest fixes which are not in the released version. 

For installing megaTinyCore, there are two options:

Option 1: Download the .zip package (either the "released" version, or by downloading the .zip of master repo), extract, and place in the hardware folder inside your sketchbook folder (if there is no hardware folder, create it). You can find/set the location of the sketchbook folder in the Arduino IDE at File > Preferences -> Sketchbook location.

Option 2: Download the github client, and sync this repo to the hardware subfolder of your sketchbook folder.

