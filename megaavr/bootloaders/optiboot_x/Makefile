# Makefile for AVR Mega-0 (4809), Tiny-0, and Tiny-1 version of Optiboot
# Bill Westfield, 2019
# $Id$
#
# Edit History
# Sep-2019 refactor from the normal AVR Makefile.
# * Copyright 2013-2019 by Bill Westfield.  Part of Optiboot.
# * This software is licensed under version 2 of the Gnu Public Licence.
# * See optiboot.c for details.

HELPTEXT = "\n"
#----------------------------------------------------------------------
#
# program name should not be changed...
PROGRAM    = optiboot_x
MF:= $(MAKEFILE_LIST)

# export symbols to recursive makes (for ISP)
export

# defaults
MCU_TARGET = atmega4809

LDSECTIONS  = -Wl,-section-start=.text=0 \
	      -Wl,--section-start=.application=0x200 \
	      -Wl,--section-start=.version=0x1fe

BAUD_RATE=115200

# If we have a PACKS directory specified, we should use it...
ifeq ($(ENV), arduino)
# For Arduino, we assume that we're connected to the optiboot directory
# included with the arduino distribution, which means that the full set
# of avr-tools are "right up there" in standard places.
# (except that in 1.5.x, there's an additional level of "up")
TESTDIR := $(firstword $(wildcard ../../../tools/*))
ifeq (,$(TESTDIR))
# Arduino 1.5.x tool location compared to optiboot dir
  TOOLROOT = ../../../../tools
else
# Arduino 1.0 (and earlier) tool location
  TOOLROOT = ../../../tools
endif
GCCROOT = $(TOOLROOT)/avr/bin/

ifeq ($(OS), windows)
# On windows, SOME of the tool paths will need to have backslashes instead
# of forward slashes (because they use windows cmd.exe for execution instead
# of a unix/mingw shell?)  We also have to ensure that a consistent shell
# is used even if a unix shell is installed (ie as part of WINAVR)
fixpath = $(subst /,\,$1)
SHELL = cmd.exe
SH = sh
endif

else ifeq ($(ENV), arduinodev)
# Arduino IDE source code environment.  Use the unpacked compilers created
# by the build (you'll need to do "ant build" first.)
ifeq ($(OS), macosx)
TOOLROOT = ../../../../build/macosx/work/Arduino.app/Contents/Resources/Java/hardware/tools
endif
ifeq ($(OS), windows)
TOOLROOT = ../../../../build/windows/work/hardware/tools
endif

GCCROOT = $(TOOLROOT)/avr/bin/
AVRDUDE_CONF = -C$(TOOLROOT)/avr/etc/avrdude.conf

else
GCCROOT =
AVRDUDE_CONF =
endif

#
# End of build environment code.


CC         = $(GCCROOT)avr-gcc
RCC        = $(abspath $(CC))
#$(info wildcard ("$(wildcard $(CC))",""))
ifndef PRODUCTION
$(info Using Compiler at: ${RCC})
endif


# If we have a PACKS directory specified, we should use it...
ifdef PACKS
PACK_OPT= -I $(PACKS)/include/ -B $(PACKS)/gcc/dev/$(TARGET)
ifndef PRODUCTION
$(info   and Chip-defining PACKS at ${PACKS})
endif
endif


OPTIMIZE = -Os -fno-split-wide-types -mrelax

# Override is only needed by avr-lib build system.

override CFLAGS  = -g -Wall $(OPTIMIZE)
override LDFLAGS = $(LDSECTIONS) -Wl,--relax -nostartfiles -nostdlib

OBJCOPY        = $(GCCROOT)avr-objcopy
OBJDUMP        = "$(GCCROOT)avr-objdump"
SIZE           = $(GCCROOT)avr-size

include parse_options.mk

.PRECIOUS: optiboot_%.elf

ifndef PRODUCTION
LISTING= $(OBJDUMP) -S
else
LISTING= @true
endif



#---------------------------------------------------------------------------
# "Chip-level Platform" targets.
# A "Chip-level Platform" compiles for a particular chip, but probably does
# not have "standard" values for things like clock speed, LED pin, etc.
# Makes for chip-level platforms should usually explicitly define their
# options like: "make atmega4809 UARTTX=A4 LED=D0"
#---------------------------------------------------------------------------
#
# Mega0, tiny0, tiny1 don't really have any chip-specific requirements.
#
# Note about fuses:
#  The fuses are defined in the source code.  There are 9!
#  Be sure to use a programmer that will program the fuses from the object file.
#
#---------------------------------------------------------------------------
#

HELPTEXT += "\n-------------\n\n"


optiboot_%.hex: optiboot_%.elf
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O ihex $< $@

optiboot_%.elf:	optiboot_x.c FORCE
	$(CC) $(CFLAGS) $(CPU_OPTIONS) $(LED_OPTIONS) $(UART_OPTIONS) $(POR_CMD) $(COMMON_OPTIONS) $(LDFLAGS) $(PACK_OPT) -mmcu=$(TARGET) -o $@ $<
	$(SIZE) $@
	$(LISTING) $@ > optiboot_$*.lst


#---------------------------------------------------------------------------
# "Board-level Platform" targets.
# A "Board-level Platform" implies a manufactured platform with a particular
# AVR_FREQ, LED, and so on.  Parameters are not particularly changeable from
# the "make" command line.
# Most of the board-level platform builds should envoke make recursively
#  appropriate specific options
#---------------------------------------------------------------------------
# This distinction isn't really well suited for the tinyAVR 0/1/2-series as
# there aren't all that many plausible configurations. And in any event,
# we build for all of them, and it's not clear how to use this otherwise.
# 0 and 1 series are entirely interchangible, nothing specific to one or the
# other in Optiboot. We're assuming A7 as the LED pin except on 8-pin parts
# when using the default serial pins, which include A7. A7 was picked, uh
# I think because that's what MegaCoreX does? Anyway, we had to pick
# something, having it be as universal as possible was good, and I was working
# from the preliminary datashet when I picked it, which showed only an
# alternate serial port location "well okay, I guess these only work with
# PORTMUX set to the alternate position". Despite the fact that the real
# datasheet had been available for months. Somehow that preliminary one was
# very widely circulated, because I am not the only one who made that mistake.
# Even in 2020, I've corrected someone who had been using the preliminary
# datasheet!
# Anyway, 2-series parts use the same binaries as each other. 3 serial options.
# 2 USARTs but USART0's alt pins are USART1's default ones.... (presumably
# sharing some underlying hardware...)
#---------------------------------------------------------------------------

HELPTEXT += "Generic 14~24 pin dev board, default serial pins, 8 second timeout, LED on A7\n"
tinyxyz: TARGET = attiny3217
tinyxyz:
	$(MAKE) -f $(MF) optiboot_txyz.hex UARTTX=B2 TIMEOUT=8 LED=A7

HELPTEXT += "Generic 8 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on A3\n"
tinyxy2: TARGET = attiny412
tinyxy2:
	$(MAKE) -f $(MF) optiboot_txy2.hex UARTTX=A6 TIMEOUT=8 LED=A3

HELPTEXT += "Generic 0/1-series dev board, alternate serial pins, 8 second timeout, LED on A7\n"
tinyxyzalt: TARGET = attiny3217
tinyxyzalt:
	$(MAKE) -f $(MF) optiboot_txyz_alt.hex UARTTX=A1 TIMEOUT=8 LED=A7


HELPTEXT += "Generic 14~24 pin dev board, default serial pins, 1 second timeout, doesn't run on POR, LED on A7\n"
tinyxyzrst: TARGET = attiny3217
tinyxyzrst:
	$(MAKE) -f $(MF) optiboot_txyz_rst.hex START_APP_ON_POR=1 UARTTX=B2 TIMEOUT=1 LED=A7

HELPTEXT += "Generic 8 pin 0/1-series dev board, default serial pins, 1 second timeout, doesn't run on POR, LED on A3\n"
tinyxy2rst: TARGET = attiny412
tinyxy2rst:
	$(MAKE) -f $(MF) optiboot_txy2_rst.hex START_APP_ON_POR=1 UARTTX=A6 TIMEOUT=1 LED=A3

HELPTEXT += "Generic 0/1-series dev board, alternate serial pins, 1 second timeout, doesn't run on POR, LED on A7\n"
tinyxyzaltrst: TARGET = attiny3217
tinyxyzaltrst:
	$(MAKE) -f $(MF) optiboot_txyz_alt_rst.hex START_APP_ON_POR=1 UARTTX=A1 TIMEOUT=1 LED=A7


HELPTEXT += "Generic 14~24 pin dev board, default serial pins, 1 second timeout, LED on A7\n"
tinyxyz1sec: TARGET = attiny3217
tinyxyz1sec:
	$(MAKE) -f $(MF) optiboot_txyz_1sec.hex UARTTX=B2 TIMEOUT=1 LED=A7

HELPTEXT += "Generic 8 pin 0/1-series dev board, default serial pins, 1 second timeout, LED on A3\n"
tinyxy21sec: TARGET = attiny412
tinyxy21sec:
	$(MAKE) -f $(MF) optiboot_txy2_1sec.hex UARTTX=A6 TIMEOUT=1 LED=A3

HELPTEXT += "Generic 0/1-series dev board, alternate serial pins, 1 second timeout, LED on A7\n"
tinyxyzalt1sec: TARGET = attiny3217
tinyxyzalt1sec:
	$(MAKE) -f $(MF) optiboot_txyz_alt_1sec.hex UARTTX=A1 TIMEOUT=1 LED=A7


## And now the 2-series....

HELPTEXT += "Generic 2-series board, USART0 default serial pins, 8 second timeout, LED on A7\n"
tinyx2z: TARGET = attiny1627
tinyx2z:
	$(MAKE) -f $(MF) optiboot_tx2z.hex UARTTX=B2 TIMEOUT=8 LED=A7

HELPTEXT += "Generic 2-series board, USART0 alternate serial pins, 8 second timeout, LED on A7\n"
tinyx2zalt: TARGET = attiny1627
tinyx2zalt:
	$(MAKE) -f $(MF) optiboot_tx2z_alt.hex UARTTX=A1 TIMEOUT=8 LED=A7

HELPTEXT += "Generic 2-series board, USART1 alternate serial pins, 8 second timeout, LED on A7\n"
tinyx2zu1alt: TARGET = attiny1627
tinyx2zu1alt:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt.hex UARTTX=C2 TIMEOUT=8 LED=A7


HELPTEXT += "Generic 2-series board, USART0 default serial pins, 1 second timeout, doesn't run on POR, LED on A7\n"
tinyx2zrst: TARGET = attiny1627
tinyx2zrst:
	$(MAKE) -f $(MF) optiboot_tx2z_rst.hex START_APP_ON_POR=1 UARTTX=B2 TIMEOUT=1 LED=A7

HELPTEXT += "Generic 2-series board, USART0 alternate serial pins, 1 second timeout, doesn't run on POR, LED on A7\n"
tinyx2zaltrst: TARGET = attiny1627
tinyx2zaltrst:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_rst.hex START_APP_ON_POR=1 UARTTX=A1 TIMEOUT=1 LED=A7

HELPTEXT += "Generic 2-series board, USART1 alternate serial pins, 1 second timeout, doesn't run on POR, LED on A7\n"
tinyx2zu1altrst: TARGET = attiny1627
tinyx2zu1altrst:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_rst.hex START_APP_ON_POR=1 UARTTX=C2 TIMEOUT=1 LED=A7


HELPTEXT += "Generic 2-series board, USART0 default serial pins, 1 second timeout, LED on A7\n"
tinyx2z1sec: TARGET = attiny1627
tinyx2z1sec:
	$(MAKE) -f $(MF) optiboot_tx2z_1sec.hex UARTTX=B2 TIMEOUT=1 LED=A7

HELPTEXT += "Generic 2-series board, USART0 alternate serial pins, 1 second timeout, LED on A7\n"
tinyx2zalt1sec: TARGET = attiny1627
tinyx2zalt1sec:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_1sec.hex UARTTX=A1 TIMEOUT=1 LED=A7

HELPTEXT += "Generic 2-series board with 20 or 24 pins, USART1 alternate serial pins, 1 second timeout, LED on A7\n"
tinyx2zu1alt1sec: TARGET = attiny1627
tinyx2zu1alt1sec:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_1sec.hex UARTTX=C2 TIMEOUT=1 LED=A7





HELPTEXT += "target curiosity1607 - Microchip ATtiny1607 Curiosity Nano.\n"
curiosity1607: TARGET = attiny1607
curiosity1607:
	$(MAKE) -f $(MF) optiboot_curiosity1607.hex UARTTX=B2 TIMEOUT=8 LED=B7 LED_INVERT=1

HELPTEXT += "target curiosity1627 - Microchip ATtiny1627 Curiosity Nano.\n"
curiosity1627: TARGET = attiny1627
curiosity1627:
	$(MAKE) -f $(MF) optiboot_curiosity1627.hex UARTTX=B2 TIMEOUT=8 LED=B7 LED_INVERT=1

HELPTEXT += "target xplained416   - Microchip ATtiny416 Xplained Nano.\n"
xplained416: TARGET = attiny1607
xplained416:
	$(MAKE) -f $(MF) optiboot_xplained416.hex UARTTX=A1 TIMEOUT=8 LED=B5 LED_INVERT=1 BAUD_RATE=57600

HELPTEXT += "target xplained817   - Microchip ATtiny817 Xplained Mini.\n"
xplained817: TARGET = attiny1607
xplained817:
	$(MAKE) -f $(MF) optiboot_xplained817.hex UARTTX=B2 TIMEOUT=8 LED=C0 BAUD_RATE=57600


#---------------------------------------------------------------------------
#
# Generic build instructions
#

FORCE:

isp: $(TARGET) FORCE
	"$(MAKE)" -f Makefile.isp isp TARGET=$(TARGET)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

#windows "rm" is dumb and objects to wildcards that don't exist
clean:
	@touch  __temp_.o __temp_.elf __temp_.lst __temp_.map
	@touch  __temp_.sym __temp_.lss __temp_.eep __temp_.srec
	@touch __temp_.bin __temp_.hex __temp_.tmp.sh
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex *.tmp.sh

clean_asm:
	rm -rf *.lst

%.lst: %.elf FORCE
	$(OBJDUMP) -h -S $< > $@

%.srec: %.elf FORCE
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O srec $< $@

%.bin: %.elf FORCE
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O binary $< $@

help:
	@echo -e $(HELPTEXT)
