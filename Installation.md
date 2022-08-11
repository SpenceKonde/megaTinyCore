# Installing megaTinyCore

## Supported IDE versions 
megaTinyCore requires a version of the IDE later than 1.6.3; - it may require a much more recent version of the IDE; this has not been tested. It has been tested with 1.8.9 and 1.8.13. 1.8.14 introduced a serious regression that prevents all compilation ("panic no major version found" when 1) the major version uses variable substitution and 2) it only impacts manual installation). When it manifests, it effects ALL board definitions. Versions prior to 1.8.13 are impacted by a serious bug in the list of programmers they present (they assume all programmers can program all parts). Hence we recommended 1.8.13. To use with 1.8.14+, you need to patch platform.txt:
```diff
-version={versionnum.major}.{versionnum.minor}.{versionnum.patch}{versionnum.postfix}
+version=2.6.0
``` 
it will then work with V1.8.19.

Additional critical regressions are observed on 2.0.x beta and RC versions prior to RC9.2, those should not be used at all - board settings are randomly overwritten, fuses get set wrong, bad stuff happens.

### Alternative development environments
There are a number of other alternatives to the Arduino IDE as such, with their users making unflattering (and occasionally profane) comments about the Arduino IDE and it's myriad shortcomings. It is known that the core can be used on the following alternative platforms. Where these are links, this link describes use of this core on that IDE. If no link is provided, that's because I don't use that IDE, and nobody who does have volunteered a guide to installation, use, and any additional complications.
* [PlatformIO](megaavr/extras/PlatformIO.md) is very popular. You can thank @MCUDude for most of the work to get this core supported over there, and this guide. I don't know anything about PIO, other than that it's users often express disbelief that I could use the Arduino IDE and not go nuts. I do not make any claim to be sane, but any insanity predated my use of Arduino; if there is any causal relationship, it must have gone the other direction.
* [Arduino-cli](megaavr/extras/Arduino-cli.md) is a command line version of the Arduino IDE. It's still official Arduino, but is far more conducive to scripting and semi-automated use.
* VisualMicro is some alternative IDE. Numerous individuals here seem to use it and prefer it over Arduino. If any of you would like to write up more info, I welcome contributions - but I don't have time to learn a new IDE just to tell people how to use it. Cut me some slack, I'm running a business, working a day job, maintaining three cores.... and unlike most core authors I write real documentation, too, which takes an incredible amount of time.

If you use an alternative development environment, be aware that no official testing is performed on those platforms, and I am not capable of providing technical support (no knowledge of the relevant IDE) - though if you report a bug with enough information that you're telling me how to fix it, I will happily do so. Ideally, you would fix it for yourself and then provide a PR to fix it for everyone else. Be sure to check the version of the core when using Platform IO - the version they normally distribute is much older than the latest release, and many bugs have been fixed since then, so you should probably first try updating the core.

Of course, bugs in that are discovered in PlatformIO or VisualMicro, but also reproduce on Arduino IDE are normal bugs, and can be reported without that additional requirement!

## Boards Manager Installation (now strongly recommended unless helping with core development)

This board package can be installed via the board manager. The boards manager URL is:

`http://drazzy.com/package_drazzy.com_index.json`

1. File -> Preferences, enter the above URL in "Additional Boards Manager URLs"
2. Tools -> Boards -> Boards Manager...
3. Wait while the list loads (takes longer than one would expect, and refreshes several times).
4. Select "megaTinyCore by Spence Konde" and click "Install". For best results, choose the most recent version.

## Manual Installation
Manual installation allows the latest version of the core to be installed, with fixes that may not yet be available in the board manager version of the core. Manual installation is recommended if you are working on developing or modifying the core - however, the requirements are brutal.

* You must be using a copy of the Arduino IDE that has never had an AVR board definition package installed on it (typically this means the .zip archive, extract, and create a portable folder inside before first run)
* You must update the toolchain. Search the .json file above `"tools": [` Of the 4 hits, you're looking for the avr-gcc one.
  * Scroll down to the most recent version (currently azduino4b)
  * Download and decompress the version for your OS. If the most recent version has no listing for your OS, go to the next most recent (this happens when I release a bad package for certain OS's and have to release a fixed version for those OS's only).
  * You will find an 'avr' directory containing several subdirectories. This may or may not be enclosed in one or more directories depending on what program is used to decompress it..
    * The directory structure is *very* confusing; I have botched this three times in released versions. There are a hell of a lot of nested "avr" directories.
  * Copy this into `arduino root folder)/hardware/tools` - if you did this right, you'll be told that hundreds of files are different. Replace them all!
  * **NOTE:** if you also intend to use DxCore manually installed, check the json entries for that one too, and see if it specifies a later toolchain version. Use whichever one has higher version (Azduinio4 is higher than Azduino3)

* If you want SerialUPDI, you need to also follow [megaavr/tools/ManualPython.md](megaavr/tools/ManualPython.md).

Once that all is done, you've only got a minor step or two left - you need to create a "hardware" folder inside the sketchbook folder (inside portable assuming you went that route, which you should) amd then and only then should you install the core.

* Option 1: Download the .zip package (either the "released" version, or by downloading the .zip of master repo), extract, and place in the hardware folder inside your sketchbook folder (if there is no hardware folder, create it). You can find/set the location of the sketchbook folder in the Arduino IDE at File > Preferences -> Sketchbook location.

* Option 2: Download the github client, and sync this repo to the hardware subfolder of your sketchbook folder. For as portable windows installation, it's typically IDE location/portable/sketchbook/hardware (you will have to create hardware.)
