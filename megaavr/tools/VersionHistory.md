# SerialUPDI

## 1.3.y
(planned)
Error messages currently suck as bad as the avrdude ones. This is *not* acceptable. True verbose mode is so excessively verbose that it is useless to anyone but me, Quentin, and a few people at Microchip. and there is no reason for that to be the case other than

## 1.3.x
(planned)
Add EEPROM action to write the EEPROM data if requested.
Add LOCKEDUSERROW action to write to the userrow of a locked chip.
Fix unlock() so that when run interactively, it will check if the chip is locked and prompt the user if they try to unlock an unlocked chip unless a "yes I'm really sure" flag is passed.


8/18/2022
## 1.3.0
Add actions lock and unlock. Lock writes the value 0x00 to the lock fuse, unlock executes a chip erase. Chip erase erases the everything except fuses and (if EESAVE is set) EEPROM (my understanding is that userrow is not erased unless the chip is locked). Additionally and most importantly, the lockbits themselves are reset.

## 1.2.4 - Fix "Hashable" error on linux
8/2022
Due to an outdated version of YAML and Pythons failure to maintain consistent behavior, Serial UPDI was broken on fully updated linux systems. This is no longer the case.
Added AVR64DDxx device files.
Synthesized the AVR32/16DD files because Microchip hasn't released those despite having been shipping them for a month.

## 1.2.3 - Fix standalone read, verbose output
Jan 17, 2022 (text finally updated)
At the very end of 1.2.0 development, it was discovered that the D11C-as-serial-adapter for UPDI that was the reason for their involvement in the project... ah... didn't actually work. Investigation revealed that there was a certain size of data that if received or sent would crash the programmer. There was already a facility to control this for write, but it also happened for read, so some solution was needed in a hurry, and the -rc option was added. The standalone read action was not tested after that was added. The implementation there was botched, and reading could never be done. Only write and verify, which of course were tested. Oooops. This was reported on DxCore on 1/14/22, and the issue was not challenging to correct. Additionally, mistake in 1.2.2 relating to the clock speed of the UPDI manager on the target was corrected in order to improve robustness during programming at 460800 baud. This file was added.

Warning levels of the messages that make "INFO" really spammy were changed to DEBUG. A few new messages were added that will be printed if commands fail to give more information on the nature of the problem.
This should allow cores to enable verbose output without the user being buried under a mountain of gibberish. This will be available in megaTinyCore 2.5.7 and DxCore 1.4.6. Manually doing -v -v will still result in your being buried by messages. This is intended; for deep debugging of issues with SerialUPDI itself, that information is often useful.

## 1.2.2 - Fixing writedelay
Jan 10, 2022, text printed at startup still said 1.2.0.
write_delay did not have enough granularity, especially on windows. Typically a single millisecond would be enough to keep the write from failing... but even -wd 1 would add over 3ms of delay on many systems. This wound up adding several seconds to the programming time in some cases. It was modified to use a pause functionality that does not call sleep() and just busywaits until the condition is met.
Additionally, write delay was applied to fuses in addition to flash - we had observed failures at high baud rates similar to the ones encountered due to too-fast writes.
This was introduced in megaTinyCore 2.5.6; notably, it also added a crapload of programmer definitions to the core as it had been found through feedback that behavior between systems and adapters as to when writedelay is needed while the operating voltage impacts the maximum speed at which data should be uploaded. This version was never included in DxCore.

## 1.2.1 - Part support update
Mid-late 2021, text printed at startup not updated.
Part of the promise of using pymcuprog was a steady stream of updates to support new parts. Unfortunately that has not happened: While Microchip is keeping it updated internally, there is an in-house quality control process before it is released to github, and by all appearances this must be very thorough, because even in late 2021, long after the 64 and 32 DA and DB-series parts had been released, the device files for pymcuprog were still missing. And we're stuck with it's disadvantages, namely that the codebase is a sprawling mess written by multiple individuals none of whom speak python fluently (and you can be sure Spence hasn't helped matters w/rt code quality, his idea of a python development tool is google search.
The missing files were synthesized from existing files and publicly available information on the parts. Worse still, the performance enhancement push resulted in such large changes to the code thatmerging proved difficult.

## 1.2.0 - Performance Improvement push
By the early summer of 2021, those who had pushed for SerialUPDI were starting to recognize the issue with upload speed. A meeting was convened over Zoom to discuss the problem, and an intensive period of performance improvement ensued. During that week, mostly by Spence's hand. Performance improved by leaps and bounds. By the end of the week write speeds speeds had increased from 600-1k bytes/s to 20k, and by the time 1.2.0 was released, writes occurred at 22-24k/s on Dx-series, and up to 14-16k on tinyAVR (where the time was dominated by USB latency, data transfer time), while reads as high as 32k/s were documented at 460800 baud (since then, it has been demonstrated that 42k/s readas are entirely possible on DxCore, while; 24k/s is the fundamental limit for write.) This was included in DxCore 1.3.6 and megaTinyCore 2.3.2. This is also when options like -wd (write delay) and -rc (read chunks) were implemented.

### Background on chunking and delay
It was found almost immediately after the performance improvement push had concluded that, for a certain key serial adapter that was used in a particular use case that was highly relevant to some individuals helping with this project, and other compatibility problems with certain adapters were noted. This led to addition of -rc and -wd, thought the issue that led to -rc wound up being found to be a firmware bug in the serial adapter in question. -wd remained relevant, and while this was included in megaTinyCore 2.3.2 the core was not adapted to properly take advantage of it with menu options until 2.5.7


## 1.1.0 - Performance Improvement push
By the early summer of 2021, those who had pushed for SerialUPDI were starting to recognize the issue with upload speed. A meeting was convened over Zoom to discuss the problem, and an intensive period of performance improvement ensued. During that week, mostly by Spence's hand. Performance improved by leaps and bounds. By the end of the week write speeds speeds had increased from 600-1k bytes/s to 20k, and by the time 1.2.0 was released, writes occurred at 22-24k/s on Dx-series, and up to 14-16k on tinyAVR (where the time was dominated by USB latency, data transfer time), while reads as high as 32k/s were documented at 460800 baud (since then, it has been demonstrated that 42k/s reads are entirely possible on DxCore, while; 24k/s is the fundamental limit for write.) This was included in DxCore 1.3.6.

## 1.0.0 - initial version
The first version, based very tightly on pymcuprog, was released in late 2020 or early 2021.
This version worked, but it was not practical, as write speed rarely exceeded 1kb/s. The majority of this work was done by Quentin, as he was skilled in python Spence was most certainly not.
This was made available starting from megaTinyCore 2.2.0 and DxCore 1.3.0
