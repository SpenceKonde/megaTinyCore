#!/bin/bash

if [ x$AVR_GCC_PATH == x"" ]; then
AVR_GCC_PATH=/bin/
fi

echo Compiling
${AVR_GCC_PATH}/avr-gcc -c -g -Os -w  -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -Wl,--gc-sections -w -mmcu=atmega4809 -DF_CPU=16000000L boot.c -o boot.o
${AVR_GCC_PATH}/avr-gcc  -g -Os -w  -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -nostartfiles  -Wl,--gc-sections  -w -mmcu=atmega4809 -DF_CPU=16000000L boot.o -o boot.elf

echo Extracting bin
${AVR_GCC_PATH}/avr-objcopy -O ihex -R .fuses boot.elf boot.hex
#${AVR_GCC_PATH}avr-objcopy -O binary -j .fuses --set-section-flags=.fuses=alloc,load --no-change-warnings --change-section-lma .fuses=0 boot.elf boot.fuses

echo Moving hex
mv boot.hex atmega4809_uart_bl.hex

#${AVR_GCC_PATH}/../avrdude/6.3.0-arduino14/bin/avrdude -C${AVR_GCC_PATH}/../avrdude/6.3.0-arduino14/etc/avrdude.conf -v -patmega4809 -cxplainedmini_updi -Pusb -Ufuses:w:boot.fuses:r  -Uflash:w:boot.bin:r
