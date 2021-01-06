# Manual/Github use of pymcuprog (pyupdi-style)
In order to use the pyupdi-style uploading, with just a serial adapter and a resistor as a UPDI programmer, on a manual/github installation, it is necessary to also manually provide a copy of python3.

## Download the Python3 package for your OS:
* Windows: https://github.com/earlephilhower/esp-quick-toolchain/releases/download/2.5.0-4/python3-3.7.2.post1-embed-win32v2a.zip
* MacOS: https://github.com/earlephilhower/esp-quick-toolchain/releases/download/2.5.0-4/python3-macosx-portable.tar.gz
* All Linux: https://github.com/earlephilhower/esp-quick-toolchain/releases/download/2.5.0-4/python3-via-env.tar.gz

## Move/Copy to this location
That is, move that downloaded file to the location of this file in the manually installed core: `(sketchbook)/hardware/megaTinyCore/megaavr/tools`

## Extract it
You should end up with a folder with a name starting in python3, followed by a bunch of other stuff (depending on the version for your OS) - for example "python3-3.7.2.post1-embed-win32v2a", and inside that folder, there should be a bunch of files and folders. If there's a single folder in there, move that one level up into the aforementioned tools folder.

## Rename that folder to `python3`
The tools folder should now contain that `python3` folder, a `libs` folder, and `prog.py`

At this point - hopefully - it should now work. My understanding is that the linux one uses the system copy of python3; there may or may not be additional steps required on that end.
