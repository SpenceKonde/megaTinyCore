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


CC	 = $(GCCROOT)avr-gcc
RCC	= $(abspath $(CC))
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

OBJCOPY	= $(GCCROOT)avr-objcopy
OBJDUMP	= "$(GCCROOT)avr-objdump"
SIZE	   = $(GCCROOT)avr-size -A

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
	$(CC) $(CFLAGS) $(CPU_OPTIONS) $(LED_OPTIONS) $(UART_OPTIONS) $(ENTRYCOND_CMD) $(COMMON_OPTIONS) $(LDFLAGS) $(PACK_OPT) -mmcu=$(TARGET) -o $@ $<
	$(SIZE) $@
	$(LISTING) $@ > optiboot_$*.lst


#---------------------------------------------------------------------------
# "Board-level Platform" targets.
# A "Board-level Platform" implies a manufactured platform with a particular
# serial port, LED, and so on.  Parameters are not particularly changeable from
# the "make" command line. These "board level platforms" are for the Microchip official boards.
#---------------------------------------------------------------------------
# The rest of these definitions are chip-level definitions, and while they assume an LED pin,
# they make no assumptions about anything else other than that the baud rate will by 115200.
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
# After realizing my error, the 8-pin bootloaders were amended to use PA3 as the LED.
# Anyway, 2-series parts use the same binaries as each other,and have 3 serial options.
# 2 USARTs but USART0's alt pins are USART1's default ones.... (presumably
# sharing some underlying hardware...)
#---------------------------------------------------------------------------

HELPTEXT += "target txyz_all8sec Generic 14~24 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: Any reset except WDT or BOR\n"
txyz_all8sec: TARGET = attiny3217
txyz_all8sec:
	$(MAKE) -f $(MF) optiboot_txyz_all8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target txyz_poronly8sec Generic 14~24 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: POR or UPDI reset\n"
txyz_poronly8sec: TARGET = attiny3217
txyz_poronly8sec:
	$(MAKE) -f $(MF) optiboot_txyz_poronly8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target txyz_swronly8sec Generic 14~24 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: Software or UPDI reset\n"
txyz_swronly8sec: TARGET = attiny3217
txyz_swronly8sec:
	$(MAKE) -f $(MF) optiboot_txyz_swronly8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target txyz_extronly Generic 14~24 pin 0/1-series dev board, default serial pins, 1 second timeout, LED on PA7. Entry: External or UPDI reset\n"
txyz_extronly: TARGET = attiny3217
txyz_extronly:
	$(MAKE) -f $(MF) optiboot_txyz_extronly.hex UARTTX=B2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target txyz_extr Generic 14~24 pin 0/1-series dev board, default serial pins, 1 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
txyz_extr: TARGET = attiny3217
txyz_extr:
	$(MAKE) -f $(MF) optiboot_txyz_extr.hex UARTTX=B2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txyz_extr8sec Generic 14~24 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
txyz_extr8sec: TARGET = attiny3217
txyz_extr8sec:
	$(MAKE) -f $(MF) optiboot_txyz_extr8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txyz_alt_all8sec Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: Any reset except WDT or BOR\n"
txyz_alt_all8sec: TARGET = attiny3217
txyz_alt_all8sec:
	$(MAKE) -f $(MF) optiboot_txyz_alt_all8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target txyz_alt_poronly8sec Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: POR or UPDI reset\n"
txyz_alt_poronly8sec: TARGET = attiny3217
txyz_alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_txyz_alt_poronly8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target txyz_alt_swronly8sec Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: Software or UPDI reset\n"
txyz_alt_swronly8sec: TARGET = attiny3217
txyz_alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_txyz_alt_swronly8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target txyz_alt_extronly Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 1 second timeout, LED on PA7. Entry: External or UPDI reset\n"
txyz_alt_extronly: TARGET = attiny3217
txyz_alt_extronly:
	$(MAKE) -f $(MF) optiboot_txyz_alt_extronly.hex UARTTX=A1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target txyz_alt_extr Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 1 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
txyz_alt_extr: TARGET = attiny3217
txyz_alt_extr:
	$(MAKE) -f $(MF) optiboot_txyz_alt_extr.hex UARTTX=A1 TIMEOUT=1 LED=A3 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txyz_alt_extr8sec Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
txyz_alt_extr8sec: TARGET = attiny3217
txyz_alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_txyz_alt_extr8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txy2_all8sec Generic 8 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA3. Entry: Any reset except WDT or BOR\n"
txy2_all8sec: TARGET = attiny412
txy2_all8sec:
	$(MAKE) -f $(MF) optiboot_txy2_all8sec.hex UARTTX=A6 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target txy2_poronly8sec Generic 8 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA3. Entry: POR or UPDI reset\n"
txy2_poronly8sec: TARGET = attiny412
txy2_poronly8sec:
	$(MAKE) -f $(MF) optiboot_txy2_poronly8sec.hex UARTTX=A6 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target txy2_swronly8sec Generic 8 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA3. Entry: Software or UPDI reset\n"
txy2_swronly8sec: TARGET = attiny412
txy2_swronly8sec:
	$(MAKE) -f $(MF) optiboot_txy2_swronly8sec.hex UARTTX=A6 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target txy2_extronly Generic 8 pin 0/1-series dev board, default serial pins, 1 second timeout, LED on PA7. Entry: External or UPDI reset\n"
txy2_extronly: TARGET = attiny412
txy2_extronly:
	$(MAKE) -f $(MF) optiboot_txy2_extronly.hex UARTTX=A6 TIMEOUT=1 LED=A3 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target txy2_extr Generic 8 pin 0/1-series dev board, default serial pins, 1 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
txy2_extr: TARGET = attiny412
txy2_extr:
	$(MAKE) -f $(MF) optiboot_txy2_extr.hex UARTTX=A6 TIMEOUT=1 LED=A3 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txy2_extr8sec Generic 8 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA3. Entry: External, software or UPDI reset\n"
txy2_extr8sec: TARGET = attiny412
txy2_extr8sec:
	$(MAKE) -f $(MF) optiboot_txy2_extr8sec.hex UARTTX=A6 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txy2_alt_all8sec Generic 8 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA3. Entry: Any reset except WDT or BOR\n"
txy2_alt_all8sec: TARGET = attiny412
txy2_alt_all8sec:
	$(MAKE) -f $(MF) optiboot_txy2_alt_all8sec.hex UARTTX=A1 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target txy2_alt_poronly8sec Generic 8 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA3. Entry: POR or UPDI reset\n"
txy2_alt_poronly8sec: TARGET = attiny412
txy2_alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_txy2_alt_poronly8sec.hex UARTTX=A1 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target txy2_alt_swronly8sec Generic 8 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA3. Entry: Software or UPDI reset\n"
txy2_alt_swronly8sec: TARGET = attiny412
txy2_alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_txy2_alt_swronly8sec.hex UARTTX=A1 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target txy2_alt_extronly Generic 8 pin 0/1-series dev board, alternate pins serial pins, 1 second timeout, LED on PA3. Entry: External or UPDI reset\n"
txy2_alt_extronly: TARGET = attiny412
txy2_alt_extronly:
	$(MAKE) -f $(MF) optiboot_txy2_alt_extronly.hex UARTTX=A1 TIMEOUT=1 LED=A3 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target txy2_alt_extr Generic 8 pin 0/1-series dev board, alternate pins serial pins, 1 second timeout, LED on PA3. Entry: External, software or UPDI reset\n"
txy2_alt_extr: TARGET = attiny412
txy2_alt_extr:
	$(MAKE) -f $(MF) optiboot_txy2_alt_extr.hex UARTTX=A1 TIMEOUT=1 LED=A3 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target txy2_alt_extr8sec Generic 8 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA3. Entry: External, software or UPDI reset\n"
txy2_alt_extr8sec: TARGET = attiny412
txy2_alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_txy2_alt_extr8sec.hex UARTTX=A1 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target tx2z_all8sec Generic 14~24 pin 2-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: Any reset except WDT or BOR\n"
tx2z_all8sec: TARGET = attiny3227
tx2z_all8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_all8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target tx2z_poronly8sec Generic 14~24 pin 2-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: POR or UPDI reset\n"
tx2z_poronly8sec: TARGET = attiny3227
tx2z_poronly8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_poronly8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target tx2z_swronly8sec Generic 14~24 pin 2-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: Software or UPDI reset\n"
tx2z_swronly8sec: TARGET = attiny3227
tx2z_swronly8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_swronly8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target tx2z_extronly Generic 14~24 pin 2-series dev board, default serial pins, 1 second timeout, LED on PA7. Entry: External or UPDI reset\n"
tx2z_extronly: TARGET = attiny3227
tx2z_extronly:
	$(MAKE) -f $(MF) optiboot_tx2z_extronly.hex UARTTX=B2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target tx2z_extr Generic 14~24 pin 2-series dev board, default serial pins, 1 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
tx2z_extr: TARGET = attiny3227
tx2z_extr:
	$(MAKE) -f $(MF) optiboot_tx2z_extr.hex UARTTX=B2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target tx2z_extr8sec Generic 14~24 pin 2-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
tx2z_extr8sec: TARGET = attiny3227
tx2z_extr8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_extr8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target tx2z_alt_all8sec Generic 14~24 pin 2-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: Any reset except WDT or BOR\n"
tx2z_alt_all8sec: TARGET = attiny3227
tx2z_alt_all8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_all8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target tx2z_alt_poronly8sec Generic 14~24 pin 2-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: POR or UPDI reset\n"
tx2z_alt_poronly8sec: TARGET = attiny3227
tx2z_alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_poronly8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target tx2z_alt_swronly8sec Generic 14~24 pin 2-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: Software or UPDI reset\n"
tx2z_alt_swronly8sec: TARGET = attiny3227
tx2z_alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_swronly8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target tx2z_alt_extronly Generic 14~24 pin 2-series dev board, alternate pins serial pins, 1 second timeout, LED on PA7. Entry: External or UPDI reset\n"
tx2z_alt_extronly: TARGET = attiny3227
tx2z_alt_extronly:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_extronly.hex UARTTX=A1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target tx2z_alt_extr Generic 14~24 pin 2-series dev board, alternate pins serial pins, 1 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
tx2z_alt_extr: TARGET = attiny3227
tx2z_alt_extr:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_extr.hex UARTTX=A1 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target tx2z_alt_extr8sec Generic 14~24 pin 2-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
tx2z_alt_extr8sec: TARGET = attiny3227
tx2z_alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_extr8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target tx2z_u1alt_all8sec Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 8 second timeout, LED on PA7. Entry: Any reset except WDT or BOR\n"
tx2z_u1alt_all8sec: TARGET = attiny3227
tx2z_u1alt_all8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_all8sec.hex UARTTX=C2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target tx2z_u1alt_poronly8sec Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 8 second timeout, LED on PA7. Entry: POR or UPDI reset\n"
tx2z_u1alt_poronly8sec: TARGET = attiny3227
tx2z_u1alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_poronly8sec.hex UARTTX=C2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target tx2z_u1alt_swronly8sec Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 8 second timeout, LED on PA7. Entry: Software or UPDI reset\n"
tx2z_u1alt_swronly8sec: TARGET = attiny3227
tx2z_u1alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_swronly8sec.hex UARTTX=C2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target tx2z_u1alt_extronly Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 1 second timeout, LED on PA7. Entry: External or UPDI reset\n"
tx2z_u1alt_extronly: TARGET = attiny3227
tx2z_u1alt_extronly:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_extronly.hex UARTTX=C2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target tx2z_u1alt_extr Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 1 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
tx2z_u1alt_extr: TARGET = attiny3227
tx2z_u1alt_extr:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_extr.hex UARTTX=C2 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target tx2z_u1alt_extr8sec Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 8 second timeout, LED on PA7. Entry: External, software or UPDI reset\n"
tx2z_u1alt_extr8sec: TARGET = attiny3227
tx2z_u1alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_extr8sec.hex UARTTX=C2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target curiosity3217_all8sec target curiosity3217 - Microchip ATtiny3217 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: Any reset except WDT or BOR\n"
curiosity3217_all8sec: TARGET = attiny3217
curiosity3217_all8sec:
	$(MAKE) -f $(MF) optiboot_curiosity3217_all8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target curiosity3217_poronly8sec target curiosity3217 - Microchip ATtiny3217 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: POR or UPDI reset\n"
curiosity3217_poronly8sec: TARGET = attiny3217
curiosity3217_poronly8sec:
	$(MAKE) -f $(MF) optiboot_curiosity3217_poronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target curiosity3217_swronly8sec target curiosity3217 - Microchip ATtiny3217 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: Software or UPDI reset\n"
curiosity3217_swronly8sec: TARGET = attiny3217
curiosity3217_swronly8sec:
	$(MAKE) -f $(MF) optiboot_curiosity3217_swronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target curiosity1607_all8sec target curiosity1607 - Microchip ATtiny1607 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: Any reset except WDT or BOR\n"
curiosity1607_all8sec: TARGET = attiny1607
curiosity1607_all8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1607_all8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target curiosity1607_poronly8sec target curiosity1607 - Microchip ATtiny1607 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: POR or UPDI reset\n"
curiosity1607_poronly8sec: TARGET = attiny1607
curiosity1607_poronly8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1607_poronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target curiosity1607_swronly8sec target curiosity1607 - Microchip ATtiny1607 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: Software or UPDI reset\n"
curiosity1607_swronly8sec: TARGET = attiny1607
curiosity1607_swronly8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1607_swronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target curiosity1627_all8sec target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: Any reset except WDT or BOR\n"
curiosity1627_all8sec: TARGET = attiny1627
curiosity1627_all8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1627_all8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target curiosity1627_poronly8sec target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: POR or UPDI reset\n"
curiosity1627_poronly8sec: TARGET = attiny1627
curiosity1627_poronly8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1627_poronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target curiosity1627_swronly8sec target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: Software or UPDI reset\n"
curiosity1627_swronly8sec: TARGET = attiny1627
curiosity1627_swronly8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1627_swronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target curiosity1627_extronly target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 1 second timeout, LED on B7. Entry: External or UPDI reset\n"
curiosity1627_extronly: TARGET = attiny1627
curiosity1627_extronly:
	$(MAKE) -f $(MF) optiboot_curiosity1627_extronly.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=1 LED=B7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target curiosity1627_extr target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 1 second timeout, LED on B7. Entry: External, software or UPDI reset\n"
curiosity1627_extr: TARGET = attiny1627
curiosity1627_extr:
	$(MAKE) -f $(MF) optiboot_curiosity1627_extr.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=1 LED=B7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target curiosity1627_extr8sec target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: External, software or UPDI reset\n"
curiosity1627_extr8sec: TARGET = attiny1627
curiosity1627_extr8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1627_extr8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target xplainedpro3217_all8sec target xplained3217pro - Microchip ATtiny3217 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: Any reset except WDT or BOR\n"
xplainedpro3217_all8sec: TARGET = attiny3217
xplainedpro3217_all8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro3217_all8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target xplainedpro3217_poronly8sec target xplained3217pro - Microchip ATtiny3217 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: POR or UPDI reset\n"
xplainedpro3217_poronly8sec: TARGET = attiny3217
xplainedpro3217_poronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro3217_poronly8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target xplainedpro3217_swronly8sec target xplained3217pro - Microchip ATtiny3217 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: Software or UPDI reset\n"
xplainedpro3217_swronly8sec: TARGET = attiny3217
xplainedpro3217_swronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro3217_swronly8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target xplainedpro817_all8sec target xplainedpro817 - Microchip ATtiny817 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: Any reset except WDT or BOR\n"
xplainedpro817_all8sec: TARGET = attiny817
xplainedpro817_all8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro817_all8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target xplainedpro817_poronly8sec target xplainedpro817 - Microchip ATtiny817 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: POR or UPDI reset\n"
xplainedpro817_poronly8sec: TARGET = attiny817
xplainedpro817_poronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro817_poronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target xplainedpro817_swronly8sec target xplainedpro817 - Microchip ATtiny817 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: Software or UPDI reset\n"
xplainedpro817_swronly8sec: TARGET = attiny817
xplainedpro817_swronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro817_swronly8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target xplainedmini817_all8sec target xplainedmini817 - Microchip ATtiny817 Xplained Mini. default serial pins, 8 second timeout, LED on C0. Entry: Any reset except WDT or BOR\n"
xplainedmini817_all8sec: TARGET = attiny817
xplainedmini817_all8sec:
	$(MAKE) -f $(MF) optiboot_xplainedmini817_all8sec.hex UARTTX=B2 TIMEOUT=8 LED=C0 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target xplainedmini817_poronly8sec target xplainedmini817 - Microchip ATtiny817 Xplained Mini. default serial pins, 8 second timeout, LED on C0. Entry: POR or UPDI reset\n"
xplainedmini817_poronly8sec: TARGET = attiny817
xplainedmini817_poronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainedmini817_poronly8sec.hex UARTTX=B2 TIMEOUT=8 LED=C0 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target xplainedmini817_swronly8sec target xplainedmini817 - Microchip ATtiny817 Xplained Mini. default serial pins, 8 second timeout, LED on C0. Entry: Software or UPDI reset\n"
xplainedmini817_swronly8sec: TARGET = attiny817
xplainedmini817_swronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainedmini817_swronly8sec.hex UARTTX=B2 TIMEOUT=8 LED=C0 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target xplainednano416_all8sec target xplainednano416 - Microchip ATtiny416 Xplained Nano. default serial pins, 8 second timeout, LED on B5. Entry: Any reset except WDT or BOR\n"
xplainednano416_all8sec: TARGET = attiny416
xplainednano416_all8sec:
	$(MAKE) -f $(MF) optiboot_xplainednano416_all8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B5 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target xplainednano416_poronly8sec target xplainednano416 - Microchip ATtiny416 Xplained Nano. default serial pins, 8 second timeout, LED on B5. Entry: POR or UPDI reset\n"
xplainednano416_poronly8sec: TARGET = attiny416
xplainednano416_poronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainednano416_poronly8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B5 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target xplainednano416_swronly8sec target xplainednano416 - Microchip ATtiny416 Xplained Nano. default serial pins, 8 second timeout, LED on B5. Entry: Software or UPDI reset\n"
xplainednano416_swronly8sec: TARGET = attiny416
xplainednano416_swronly8sec:
	$(MAKE) -f $(MF) optiboot_xplainednano416_swronly8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B5 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target txyz_porswr8sec Generic 14~24 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: POR, software or UPDI reset\n"
txyz_porswr8sec: TARGET = attiny3217
txyz_porswr8sec:
	$(MAKE) -f $(MF) optiboot_txyz_porswr8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target txyz_alt_porswr8sec Generic 14~24 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: POR, software or UPDI reset\n"
txyz_alt_porswr8sec: TARGET = attiny3217
txyz_alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_txyz_alt_porswr8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target txy2_porswr8sec Generic 8 pin 0/1-series dev board, default serial pins, 8 second timeout, LED on PA3. Entry: POR, software or UPDI reset\n"
txy2_porswr8sec: TARGET = attiny412
txy2_porswr8sec:
	$(MAKE) -f $(MF) optiboot_txy2_porswr8sec.hex UARTTX=A6 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target txy2_alt_porswr8sec Generic 8 pin 0/1-series dev board, alternate pins serial pins, 8 second timeout, LED on PA3. Entry: POR, software or UPDI reset\n"
txy2_alt_porswr8sec: TARGET = attiny412
txy2_alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_txy2_alt_porswr8sec.hex UARTTX=A1 TIMEOUT=8 LED=A3 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target tx2z_porswr8sec Generic 14~24 pin 2-series dev board, default serial pins, 8 second timeout, LED on PA7. Entry: POR, software or UPDI reset\n"
tx2z_porswr8sec: TARGET = attiny3227
tx2z_porswr8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_porswr8sec.hex UARTTX=B2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target tx2z_alt_porswr8sec Generic 14~24 pin 2-series dev board, alternate pins serial pins, 8 second timeout, LED on PA7. Entry: POR, software or UPDI reset\n"
tx2z_alt_porswr8sec: TARGET = attiny3227
tx2z_alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_alt_porswr8sec.hex UARTTX=A1 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target tx2z_u1alt_porswr8sec Generic 14~24 pin 2-series dev board, USART1 alternate pins serial pins, 8 second timeout, LED on PA7. Entry: POR, software or UPDI reset\n"
tx2z_u1alt_porswr8sec: TARGET = attiny3227
tx2z_u1alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_tx2z_u1alt_porswr8sec.hex UARTTX=C2 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target curiosity3217_porswr8sec target curiosity3217 - Microchip ATtiny3217 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: POR, software or UPDI reset\n"
curiosity3217_porswr8sec: TARGET = attiny3217
curiosity3217_porswr8sec:
	$(MAKE) -f $(MF) optiboot_curiosity3217_porswr8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target curiosity1607_porswr8sec target curiosity1607 - Microchip ATtiny1607 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: POR, software or UPDI reset\n"
curiosity1607_porswr8sec: TARGET = attiny1607
curiosity1607_porswr8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1607_porswr8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target curiosity1627_porswr8sec target curiosity1627 - Microchip ATtiny1627 Curiosity Nano. default serial pins, 8 second timeout, LED on B7. Entry: POR, software or UPDI reset\n"
curiosity1627_porswr8sec: TARGET = attiny1627
curiosity1627_porswr8sec:
	$(MAKE) -f $(MF) optiboot_curiosity1627_porswr8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target xplainedpro3217_porswr8sec target xplained3217pro - Microchip ATtiny3217 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: POR, software or UPDI reset\n"
xplainedpro3217_porswr8sec: TARGET = attiny3217
xplainedpro3217_porswr8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro3217_porswr8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target xplainedpro817_porswr8sec target xplainedpro817 - Microchip ATtiny817 Xplained Pro. default serial pins, 8 second timeout, LED on B4. Entry: POR, software or UPDI reset\n"
xplainedpro817_porswr8sec: TARGET = attiny817
xplainedpro817_porswr8sec:
	$(MAKE) -f $(MF) optiboot_xplainedpro817_porswr8sec.hex UARTTX=B2 LED_INVERT=1 TIMEOUT=8 LED=B4 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target xplainedmini817_porswr8sec target xplainedmini817 - Microchip ATtiny817 Xplained Mini. default serial pins, 8 second timeout, LED on C0. Entry: POR, software or UPDI reset\n"
xplainedmini817_porswr8sec: TARGET = attiny817
xplainedmini817_porswr8sec:
	$(MAKE) -f $(MF) optiboot_xplainedmini817_porswr8sec.hex UARTTX=B2 TIMEOUT=8 LED=C0 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target xplainednano416_porswr8sec target xplainednano416 - Microchip ATtiny416 Xplained Nano. default serial pins, 8 second timeout, LED on B5. Entry: POR, software or UPDI reset\n"
xplainednano416_porswr8sec: TARGET = attiny416
xplainednano416_porswr8sec:
	$(MAKE) -f $(MF) optiboot_xplainednano416_porswr8sec.hex UARTTX=A1 LED_INVERT=1 TIMEOUT=8 LED=B5 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_porswr8sec- megaAVR 0-series (m4809 et. al.) USART0, default serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser0_porswr8sec: TARGET = atmega4809
mega0_ser0porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_porswr8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser0_porswr8sec - megaAVR 0-series (m4809 et. al.) USART0, default serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser0_porswr8sec: TARGET = atmega4809
mega0_ser0_porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_porswr8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser0_all8sec - megaAVR 0-series (m4809 et. al.) USART0,  default serial pins, 8 second timeout, LED on A7. Entry: Any reset except WDT or BOR\n"
mega0_ser0_all8sec: TARGET = atmega4809
mega0_ser0_all8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_all8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target mega0_ser0_poronly8sec - megaAVR 0-series (m4809 et. al.) USART0,  default serial pins, 8 second timeout, LED on A7. Entry: POR or UPDI reset\n"
mega0_ser0_poronly8sec: TARGET = atmega4809
mega0_ser0_poronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_poronly8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target mega0_ser0_swronly8sec - megaAVR 0-series (m4809 et. al.) USART0,  default serial pins, 8 second timeout, LED on A7. Entry: Software or UPDI reset\n"
mega0_ser0_swronly8sec: TARGET = atmega4809
mega0_ser0_swronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_swronly8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target mega0_ser0_extronly - megaAVR 0-series (m4809 et. al.) USART0,  default serial pins, 1 second timeout, LED on A7. Entry: External or UPDI reset\n"
mega0_ser0_extronly: TARGET = atmega4809
mega0_ser0_extronly:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_extronly.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target mega0_ser0_extr - megaAVR 0-series (m4809 et. al.) USART0,  default serial pins, 1 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser0_extr: TARGET = atmega4809
mega0_ser0_extr:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_extr.hex UARTTX=A0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser0_extr8sec - megaAVR 0-series (m4809 et. al.) USART0,  default serial pins, 8 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser0_extr8sec: TARGET = atmega4809
mega0_ser0_extr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_extr8sec.hex UARTTX=A0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser0_alt_porswr8sec - megaAVR 0-series (m4809 et. al.) USART0, alternate serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser0_alt_porswr8sec: TARGET = atmega4809
mega0_ser0_alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_porswr8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser0_alt_all8sec - megaAVR 0-series (m4809 et. al.) USART0,  alternate serial pins, 8 second timeout, LED on A7. Entry: Any reset except WDT or BOR\n"
mega0_ser0_alt_all8sec: TARGET = atmega4809
mega0_ser0_alt_all8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_all8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target mega0_ser0_alt_poronly8sec - megaAVR 0-series (m4809 et. al.) USART0,  alternate serial pins, 8 second timeout, LED on A7. Entry: POR or UPDI reset\n"
mega0_ser0_alt_poronly8sec: TARGET = atmega4809
mega0_ser0_alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_poronly8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target mega0_ser0_alt_swronly8sec - megaAVR 0-series (m4809 et. al.) USART0,  alternate serial pins, 8 second timeout, LED on A7. Entry: Software or UPDI reset\n"
mega0_ser0_alt_swronly8sec: TARGET = atmega4809
mega0_ser0_alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_swronly8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target mega0_ser0_alt_extronly - megaAVR 0-series (m4809 et. al.) USART0,  alternate serial pins, 1 second timeout, LED on A7. Entry: External or UPDI reset\n"
mega0_ser0_alt_extronly: TARGET = atmega4809
mega0_ser0_alt_extronly:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_extronly.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target mega0_ser0_alt_extr - megaAVR 0-series (m4809 et. al.) USART0,  alternate serial pins, 1 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser0_alt_extr: TARGET = atmega4809
mega0_ser0_alt_extr:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_extr.hex UARTTX=A4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser0_alt_extr8sec - megaAVR 0-series (m4809 et. al.) USART0,  alternate serial pins, 8 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser0_alt_extr8sec: TARGET = atmega4809
mega0_ser0_alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser0_alt_extr8sec.hex UARTTX=A4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser1_porswr8sec - megaAVR 0-series (m4809 et. al.) USART1, default serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser1_porswr8sec: TARGET = atmega4809
mega0_ser1_porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_porswr8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser1_all8sec - megaAVR 0-series (m4809 et. al.) USART1,  default serial pins, 8 second timeout, LED on A7. Entry: Any reset except WDT or BOR\n"
mega0_ser1_all8sec: TARGET = atmega4809
mega0_ser1_all8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_all8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target mega0_ser1_poronly8sec - megaAVR 0-series (m4809 et. al.) USART1,  default serial pins, 8 second timeout, LED on A7. Entry: POR or UPDI reset\n"
mega0_ser1_poronly8sec: TARGET = atmega4809
mega0_ser1_poronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_poronly8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target mega0_ser1_swronly8sec - megaAVR 0-series (m4809 et. al.) USART1,  default serial pins, 8 second timeout, LED on A7. Entry: Software or UPDI reset\n"
mega0_ser1_swronly8sec: TARGET = atmega4809
mega0_ser1_swronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_swronly8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target mega0_ser1_extronly - megaAVR 0-series (m4809 et. al.) USART1,  default serial pins, 1 second timeout, LED on A7. Entry: External or UPDI reset\n"
mega0_ser1_extronly: TARGET = atmega4809
mega0_ser1_extronly:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_extronly.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target mega0_ser1_extr - megaAVR 0-series (m4809 et. al.) USART1,  default serial pins, 1 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser1_extr: TARGET = atmega4809
mega0_ser1_extr:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_extr.hex UARTTX=C0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser1_extr8sec - megaAVR 0-series (m4809 et. al.) USART1,  default serial pins, 8 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser1_extr8sec: TARGET = atmega4809
mega0_ser1_extr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_extr8sec.hex UARTTX=C0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser1_alt_porswr8sec - megaAVR 0-series (m4809 et. al.) USART1, alternate serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser1_alt_porswr8sec: TARGET = atmega4809
mega0_ser1_alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_porswr8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser1_alt_all8sec - megaAVR 0-series (m4809 et. al.) USART1,  alternate serial pins, 8 second timeout, LED on A7. Entry: Any reset except WDT and BOR\n"
mega0_ser1_alt_all8sec: TARGET = atmega4809
mega0_ser1_alt_all8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_all8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target mega0_ser1_alt_poronly8sec - megaAVR 0-series (m4809 et. al.) USART1,  alternate serial pins, 8 second timeout, LED on A7. Entry: POR or UPDI reset\n"
mega0_ser1_alt_poronly8sec: TARGET = atmega4809
mega0_ser1_alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_poronly8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target mega0_ser1_alt_swronly8sec - megaAVR 0-series (m4809 et. al.) USART1,  alternate serial pins, 8 second timeout, LED on A7. Entry: Software or UPDI reset\n"
mega0_ser1_alt_swronly8sec: TARGET = atmega4809
mega0_ser1_alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_swronly8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target mega0_ser1_alt_extronly - megaAVR 0-series (m4809 et. al.) USART1,  alternate serial pins, 1 second timeout, LED on A7. Entry: External or UPDI reset\n"
mega0_ser1_alt_extronly: TARGET = atmega4809
mega0_ser1_alt_extronly:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_extronly.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target mega0_ser1_alt_extr - megaAVR 0-series (m4809 et. al.) USART1,  alternate serial pins, 1 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser1_alt_extr: TARGET = atmega4809
mega0_ser1_alt_extr:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_extr.hex UARTTX=C4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser1_alt_extr8sec - megaAVR 0-series (m4809 et. al.) USART1,  alternate serial pins, 8 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser1_alt_extr8sec: TARGET = atmega4809
mega0_ser1_alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser1_alt_extr8sec.hex UARTTX=C4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser2_porswr8sec - megaAVR 0-series (m4809 et. al.) USART2, default serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser2_porswr8sec: TARGET = atmega4809
mega0_ser2_porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_porswr8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser2_all8sec - megaAVR 0-series (m4809 et. al.) USART2,  default serial pins, 8 second timeout, LED on A7. Entry: Any reset except WDT or BOR\n"
mega0_ser2_all8sec: TARGET = atmega4809
mega0_ser2_all8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_all8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target mega0_ser2_poronly8sec - megaAVR 0-series (m4809 et. al.) USART2,  default serial pins, 8 second timeout, LED on A7. Entry: POR or UPDI reset\n"
mega0_ser2_poronly8sec: TARGET = atmega4809
mega0_ser2_poronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_poronly8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target mega0_ser2_swronly8sec - megaAVR 0-series (m4809 et. al.) USART2,  default serial pins, 8 second timeout, LED on A7. Entry: Software or UPDI reset\n"
mega0_ser2_swronly8sec: TARGET = atmega4809
mega0_ser2_swronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_swronly8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target mega0_ser2_extronly - megaAVR 0-series (m4809 et. al.) USART2,  default serial pins, 1 second timeout, LED on A7. Entry: External or UPDI reset\n"
mega0_ser2_extronly: TARGET = atmega4809
mega0_ser2_extronly:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_extronly.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target mega0_ser2_extr - megaAVR 0-series (m4809 et. al.) USART2,  default serial pins, 1 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser2_extr: TARGET = atmega4809
mega0_ser2_extr:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_extr.hex UARTTX=F0 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser2_extr8sec - megaAVR 0-series (m4809 et. al.) USART2,  default serial pins, 8 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser2_extr8sec: TARGET = atmega4809
mega0_ser2_extr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_extr8sec.hex UARTTX=F0 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser2_alt_porswr8sec - megaAVR 0-series (m4809 et. al.) USART2, alternate serial pins, 8 second timeout, LED on A7. Entry: POR, software or UPDI reset\n"
mega0_ser2_alt_porswr8sec: TARGET = atmega4809
mega0_ser2_alt_porswr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_porswr8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x31

HELPTEXT += "target mega0_ser2_alt_all8sec - megaAVR 0-series (m4809 et. al.) USART2,  alternate serial pins, 8 second timeout, LED on A7. Entry: Any reset except WDT or BOR\n"
mega0_ser2_alt_all8sec: TARGET = atmega4809
mega0_ser2_alt_all8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_all8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x35

HELPTEXT += "target mega0_ser2_alt_poronly8sec - megaAVR 0-series (m4809 et. al.) USART2,  alternate serial pins, 8 second timeout, LED on A7. Entry: POR or UPDI reset\n"
mega0_ser2_alt_poronly8sec: TARGET = atmega4809
mega0_ser2_alt_poronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_poronly8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x21

HELPTEXT += "target mega0_ser2_alt_swronly8sec - megaAVR 0-series (m4809 et. al.) USART2,  alternate serial pins, 8 second timeout, LED on A7. Entry: Software or UPDI reset\n"
mega0_ser2_alt_swronly8sec: TARGET = atmega4809
mega0_ser2_alt_swronly8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_swronly8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x30

HELPTEXT += "target mega0_ser2_alt_extronly - megaAVR 0-series (m4809 et. al.) USART2,  alternate serial pins, 1 second timeout, LED on A7. Entry: External or UPDI reset\n"
mega0_ser2_alt_extronly: TARGET = atmega4809
mega0_ser2_alt_extronly:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_extronly.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x24

HELPTEXT += "target mega0_ser2_alt_extr - megaAVR 0-series (m4809 et. al.) USART2,  alternate serial pins, 1 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser2_alt_extr: TARGET = atmega4809
mega0_ser2_alt_extr:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_extr.hex UARTTX=F4 TIMEOUT=1 LED=A7 ENTRYCOND_REQUIRE=0x34

HELPTEXT += "target mega0_ser2_alt_extr8sec - megaAVR 0-series (m4809 et. al.) USART2,  alternate serial pins, 8 second timeout, LED on A7. Entry: External, software or UPDI reset\n"
mega0_ser2_alt_extr8sec: TARGET = atmega4809
mega0_ser2_alt_extr8sec:
	$(MAKE) -f $(MF) optiboot_mega0_ser2_alt_extr8sec.hex UARTTX=F4 TIMEOUT=8 LED=A7 ENTRYCOND_REQUIRE=0x34


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
	@touch  __temp_.bin __temp_.hex __temp_.tmp.sh
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
