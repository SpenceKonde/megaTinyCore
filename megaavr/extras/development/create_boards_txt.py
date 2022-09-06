import re
subsections={
  "buildparams":{
    "heading":"#                                        #\n# BUILD PARAMETERS                       #\n#________________________________________#",
    "entries":[
      "build.text_section_start=.text=0x0",
      "build.millistimer=D0",
      "build.clocksource=0",
      "build.clocksource=0",
      "build.core=megatinycore",
      "build.tuned=",
      "build.printf=",
      "build.wire=MORS",
      "build.attachmode=-DCORE_ATTACH_ALL",
      "build.mcu=attiny{build.attiny}",
      "build.f_cpu={build.speed}000000L",
      "build.export_merged_output=false"
    ],
  },
  "bootparams_updi":{
    "heading":"#----------------------------------------#\n# BOOTLOADING PARAMETERS (FUSES)         #\n#________________________________________#",
    "entries":[
      "bootloader.WDTCFG=0x00",
      "bootloader.BODCFG=0b{bootloader.bodlevbits}{bootloader.bodmodebits}",
      "bootloader.OSCCFG=0x01",
      "bootloader.TCD0CFG_avrdude=",
      "bootloader.TCD0CFG_serialupdi=",
      "bootloader.SYSCFG0=0b1100{bootloader.resetpinbits}0{bootloader.eesavebit}",
      "bootloader.APPEND=0x00",
      "bootloader.BOOTEND=0x00",
      "bootloader.resetpinbits=01",
      "bootloader.tool=avrdude"
      "bootloader.avrdudestring=",
      "bootloader.serialupdistring=-aerase",
    ],
  },
  "bootparams_opti":{
    "heading":"#----------------------------------------#\n# BOOTLOADING PARAMETERS (FUSES)         #\n#________________________________________#",
    "entries":[
      "bootloader.WDTCFG=0x00",
      "bootloader.BODCFG=0b{bootloader.bodlevbits}{bootloader.bodmodebits}",
      "bootloader.OSCCFG=0x01",
      "bootloader.TCD0CFG_avrdude=",
      "bootloader.TCD0CFG_serialupdi=",
      "bootloader.SYSCFG0=0b1100{bootloader.resetpinbits}0{bootloader.eesavebit}",
      "bootloader.APPEND=0x00",
      "bootloader.BOOTEND=0x01",
      "bootloader.resetpinbits=01",
      "bootloader.tool=avrdude",
    ],
  },
  "exportparams":{
    "heading":"#----------------------------------------#\n# Exported hex/lst/map names will        #\n# indicate selected options. These       #\n# are only to do that, nothing else      #\n#________________________________________#",
    "entries":[
      "build.bootload=",
      "build.printfabr=",
      "build.attachabr=",
      "build.wireabr=",
      "build.pwmabr="
    ],
  },
  "uploadparams_updi":{
    "heading":"#----------------------------------------#\n# ALL UPLOADING VIA UPDI                 #\n#________________________________________#",
    "entries":[
      "upload.tool=avrdude",
      "upload.workaround=",
      "upload.prog_interlock="
    ],
  },
  "uploadparams_opti":{
    "heading":"#----------------------------------------#\n# ALL UPLOADING VIA OPTIBOOT             #\n#________________________________________#",
    "entries":[
      "upload.tool=avrdude",
      "upload.protocol=arduino",
      "upload.prog_interlock=\"ERROR: Upload using programmer is not supported on optiboot boards\"",
      "upload.extra_params=-D -P{serial.port} -b115200"
    ],
  },
  "chipmenu":{"                 SUBMENUS                #\n#_____________________________________________#\n# CHIP                                   #\n#________________________________________#",
  },
  "clockmenu":{
    "heading":"#----------------------------------------#\n# CLOCK SOURCE                           #\n#________________________________________#",
    "entries":[
      "menu.clock.20internal=20 MHz internal",
      "menu.clock.20internal.build.clocksource=0",
      "menu.clock.20internal.build.speed=20",
      "menu.clock.20internal.bootloader.OSCCFG=0x02",
      "menu.clock.16internal=16 MHz internal",
      "menu.clock.16internal.build.speed=16",
      "menu.clock.16internal.build.clocksource=0",
      "menu.clock.10internal=10 MHz internal",
      "menu.clock.10internal.build.clocksource=0",
      "menu.clock.10internal.build.speed=10",
      "menu.clock.10internal.bootloader.OSCCFG=0x02",
      "menu.clock.8internal=8 MHz internal",
      "menu.clock.8internal.build.speed=8",
      "menu.clock.8internal.build.clocksource=0",
      "menu.clock.20extclock=20 MHz external clock",
      "menu.clock.20extclock.build.speed=20",
      "menu.clock.20extclock.build.clocksource=2",
      "menu.clock.16extclock=16 MHz external clock",
      "menu.clock.16extclock.build.speed=16",
      "menu.clock.16extclock.build.clocksource=2",
      "menu.clock.12extclock=12 MHz external clock",
      "menu.clock.12extclock.build.speed=12",
      "menu.clock.12extclock.build.clocksource=2",
      "menu.clock.10extclock=10 MHz external clock",
      "menu.clock.10extclock.build.speed=10",
      "menu.clock.10extclock.build.clocksource=2",
      "menu.clock.8extclock=8 MHz external clock",
      "menu.clock.8extclock.build.speed=8",
      "menu.clock.8extclock.build.clocksource=2",
      "menu.clock.24extclock=24 MHz external clock - overclocked",
      "menu.clock.24extclock.build.speed=24",
      "menu.clock.24extclock.build.clocksource=2",
      "menu.clock.25extclock=25 MHz external clock - overclocked",
      "menu.clock.25extclock.build.speed=24",
      "menu.clock.25extclock.build.clocksource=2",
      "menu.clock.30extclock=30 MHz external clock - overclocked, may be unstable",
      "menu.clock.30extclock.build.speed=30",
      "menu.clock.30extclock.build.clocksource=2",
      "menu.clock.32extclock=32 MHz external clock - overclocked, may be unstable",
      "menu.clock.32extclock.build.speed=32",
      "menu.clock.32extclock.build.clocksource=2",
      "menu.clock.20internaltuned=20 MHz internal - tuned",
      "menu.clock.20internaltuned.build.clocksource=0",
      "menu.clock.20internaltuned.build.speed=20",
      "menu.clock.20internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.20internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.16internaltuned=16 MHz internal - tuned",
      "menu.clock.16internaltuned.build.speed=16",
      "menu.clock.16internaltuned.build.clocksource=0",
      "menu.clock.16internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.24internaltuned=24 MHz internal - tuned, overclocked",
      "menu.clock.24internaltuned.build.clocksource=0",
      "menu.clock.24internaltuned.build.speed=24",
      "menu.clock.24internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.24internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.25internaltuned=25 MHz internal - tuned, overclocked",
      "menu.clock.25internaltuned.build.clocksource=0",
      "menu.clock.25internaltuned.build.speed=25",
      "menu.clock.25internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.25internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.30internaltuned=30 MHz internal - tuned, overclocked, may be unstable, req. 20 MHz fuse",
      "menu.clock.30internaltuned.build.clocksource=0",
      "menu.clock.30internaltuned.build.speed=30",
      "menu.clock.30internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.30internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.32internaltuned=32 MHz internal - tuned, 2-series only, overclocked, may be unstable, req. 20 MHz fuse",
      "menu.clock.32internaltuned.build.clocksource=0",
      "menu.clock.32internaltuned.build.speed=32",
      "menu.clock.32internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.32internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.12internaltuned=12 MHz internal - tuned",
      "menu.clock.12internaltuned.build.clocksource=0",
      "menu.clock.12internaltuned.build.speed=12",
      "menu.clock.12internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.12internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.10internaltuned=10 MHz internal - tuned",
      "menu.clock.10internaltuned.build.clocksource=0",
      "menu.clock.10internaltuned.build.speed=10",
      "menu.clock.10internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.10internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.8internaltuned=8 MHz internal - tuned",
      "menu.clock.8internaltuned.build.speed=8",
      "menu.clock.8internaltuned.build.clocksource=0",
      "menu.clock.8internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.6internaltuned=6 MHz internal - tuned",
      "menu.clock.6internaltuned.build.speed=6",
      "menu.clock.6internaltuned.build.clocksource=0",
      "menu.clock.6internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.5internaltuned=5 MHz internal - tuned",
      "menu.clock.5internaltuned.build.clocksource=0",
      "menu.clock.5internaltuned.build.speed=5",
      "menu.clock.5internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.5internaltuned.bootloader.OSCCFG=0x02",
      "menu.clock.4internaltuned=4 MHz internal - tuned",
      "menu.clock.4internaltuned.build.speed=4",
      "menu.clock.4internaltuned.build.clocksource=0",
      "menu.clock.4internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.2internaltuned=2 MHz internal - tuned",
      "menu.clock.2internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clock.2internaltuned.build.speed=2",
      "menu.clock.2internaltuned.build.clocksource=0",
      "menu.clock.1internaltuned=1 MHz internal - tuned",
      "menu.clock.1internaltuned.build.speed=1",
      "menu.clock.1internaltuned.build.clocksource=0",
      "menu.clock.1internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
    ]
  },
  "clockomenu": {
    "entries":[
      "menu.clocko.20internal=20 MHz internal",
      "menu.clocko.20internal.build.clocksource=0",
      "menu.clocko.20internal.build.speed=20",
      "menu.clocko.20internal.bootloader.OSCCFG=0x02",
      "menu.clocko.16internal=16 MHz internal",
      "menu.clocko.16internal.build.speed=16",
      "menu.clocko.16internal.build.clocksource=0",
      "menu.clocko.10internal=10 MHz internal",
      "menu.clocko.10internal.build.clocksource=0",
      "menu.clocko.10internal.build.speed=10",
      "menu.clocko.10internal.bootloader.OSCCFG=0x02",
      "menu.clocko.8internal=8 MHz internal",
      "menu.clocko.8internal.build.speed=8",
      "menu.clocko.8internal.build.clocksource=0",
      "menu.clocko.20extclock=20 MHz external clock",
      "menu.clocko.20extclock.build.speed=20",
      "menu.clocko.20extclock.build.clocksource=2",
      "menu.clocko.16extclock=16 MHz external clock",
      "menu.clocko.16extclock.build.speed=16",
      "menu.clocko.16extclock.build.clocksource=2",
      "menu.clocko.12extclock=12 MHz external clock",
      "menu.clocko.12extclock.build.speed=12",
      "menu.clocko.12extclock.build.clocksource=2",
      "menu.clocko.10extclock=10 MHz external clock",
      "menu.clocko.10extclock.build.speed=10",
      "menu.clocko.10extclock.build.clocksource=2",
      "menu.clocko.8extclock=8 MHz external clock",
      "menu.clocko.8extclock.build.speed=8",
      "menu.clocko.8extclock.build.clocksource=2",
      "menu.clocko.24extclock=24 MHz external clock - overclocked",
      "menu.clocko.24extclock.build.speed=24",
      "menu.clocko.24extclock.build.clocksource=2",
      "menu.clocko.25extclock=25 MHz external clock - overclocked",
      "menu.clocko.25extclock.build.speed=24",
      "menu.clocko.25extclock.build.clocksource=2",
      "menu.clocko.30extclock=30 MHz external clock - overclocked, may be unstable",
      "menu.clocko.30extclock.build.speed=30",
      "menu.clocko.30extclock.build.clocksource=2",
      "menu.clocko.32extclock=32 MHz external clock - overclocked, may be unstable",
      "menu.clocko.32extclock.build.speed=32",
      "menu.clocko.32extclock.build.clocksource=2",
      "menu.clocko.20internaltuned=20 MHz internal - tuned",
      "menu.clocko.20internaltuned.build.clocksource=0",
      "menu.clocko.20internaltuned.build.speed=20",
      "menu.clocko.20internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.20internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.16internaltuned=16 MHz internal - tuned",
      "menu.clocko.16internaltuned.build.speed=16",
      "menu.clocko.16internaltuned.build.clocksource=0",
      "menu.clocko.16internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.24internaltuned=24 MHz internal - tuned, overclocked",
      "menu.clocko.24internaltuned.build.clocksource=0",
      "menu.clocko.24internaltuned.build.speed=24",
      "menu.clocko.24internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.24internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.25internaltuned=25 MHz internal - tuned, overclocked",
      "menu.clocko.25internaltuned.build.clocksource=0",
      "menu.clocko.25internaltuned.build.speed=25",
      "menu.clocko.25internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.25internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.30internaltuned=30 MHz internal - tuned, overclocked, may be unstable, req. 20 MHz fuse",
      "menu.clocko.30internaltuned.build.clocksource=0",
      "menu.clocko.30internaltuned.build.speed=30",
      "menu.clocko.30internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.30internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.32internaltuned=32 MHz internal - tuned, 2-series only, overclocked, may be unstable, req. 20 MHz fuse",
      "menu.clocko.32internaltuned.build.clocksource=0",
      "menu.clocko.32internaltuned.build.speed=32",
      "menu.clocko.32internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.32internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.12internaltuned=12 MHz internal - tuned",
      "menu.clocko.12internaltuned.build.clocksource=0",
      "menu.clocko.12internaltuned.build.speed=12",
      "menu.clocko.12internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.12internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.10internaltuned=10 MHz internal - tuned",
      "menu.clocko.10internaltuned.build.clocksource=0",
      "menu.clocko.10internaltuned.build.speed=10",
      "menu.clocko.10internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.10internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.8internaltuned=8 MHz internal - tuned",
      "menu.clocko.8internaltuned.build.speed=8",
      "menu.clocko.8internaltuned.build.clocksource=0",
      "menu.clocko.8internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.6internaltuned=6 MHz internal - tuned",
      "menu.clocko.6internaltuned.build.speed=6",
      "menu.clocko.6internaltuned.build.clocksource=0",
      "menu.clocko.6internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.5internaltuned=5 MHz internal - tuned",
      "menu.clocko.5internaltuned.build.clocksource=0",
      "menu.clocko.5internaltuned.build.speed=5",
      "menu.clocko.5internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.5internaltuned.bootloader.OSCCFG=0x02",
      "menu.clocko.4internaltuned=4 MHz internal - tuned",
      "menu.clocko.4internaltuned.build.speed=4",
      "menu.clocko.4internaltuned.build.clocksource=0",
      "menu.clocko.4internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.2internaltuned=2 MHz internal - tuned",
      "menu.clocko.2internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "menu.clocko.2internaltuned.build.speed=2",
      "menu.clocko.2internaltuned.build.clocksource=0",
      "menu.clocko.1internaltuned=1 MHz internal - tuned",
      "menu.clocko.1internaltuned.build.speed=1",
      "menu.clocko.1internaltuned.build.clocksource=0",
      "menu.clocko.1internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
    ]
  },
  "bodmenu":{
    "heading":"#----------------------------------------#\n# BOD VOLTAGE                            #\n#________________________________________#",
    "entries":[
      "menu.bodvoltage.1v8=1.8V (5 MHz or less)",
      "menu.bodvoltage.2v6=2.6V (10 MHz or less)",
      "menu.bodvoltage.4v2=4.2V (20 MHZ or less)",
      "menu.bodvoltage.2v1=2.1V (Unofficial)",
      "menu.bodvoltage.2v9=2.9V (Unofficial)",
      "menu.bodvoltage.3v3=3.3V (Unofficial)",
      "menu.bodvoltage.3v7=3.7V (Unofficial)",
      "menu.bodvoltage.4v0=4.0V (Unofficial)",
      "menu.bodvoltage.1v8.bootloader.bodlevbits=000",
      "menu.bodvoltage.2v1.bootloader.bodlevbits=001",
      "menu.bodvoltage.2v6.bootloader.bodlevbits=010",
      "menu.bodvoltage.2v9.bootloader.bodlevbits=011",
      "menu.bodvoltage.3v3.bootloader.bodlevbits=100",
      "menu.bodvoltage.3v7.bootloader.bodlevbits=101",
      "menu.bodvoltage.4v0.bootloader.bodlevbits=110",
      "menu.bodvoltage.4v2.bootloader.bodlevbits=111",
    ],
  },
  "bodmodemenu":{
    "heading":"#----------------------------------------#\n# BOD MODE                               #\n#________________________________________#",
    "entries":[
      "menu.bodmode.disabled=Disabled/Disabled",
      "menu.bodmode.enabled=Enabled/Enabled",
      "menu.bodmode.ensampfast=Enabled/Sampled (1 kHz)",
      "menu.bodmode.ensampslow=Enabled/Sampled (125 Hz)",
      "menu.bodmode.sampledfast=Sampled/Sampled (1 kHz)",
      "menu.bodmode.sampledslow=Sampled/Sampled (125 Hz)",
      "menu.bodmode.sampdisfast=Sampled/Disabled (1 kHz)",
      "menu.bodmode.sampdisslow=Sampled/Disabled (125 Hz)",
      "menu.bodmode.endisholdwake=Enabled hold wakeup/Disabled",
      "menu.bodmode.disabled.bootloader.BODCFG=0x00",
      "menu.bodmode.enabled.bootloader.bodmodebits=00101",
      "menu.bodmode.ensampfast.bootloader.bodmodebits=00110",
      "menu.bodmode.ensampslow.bootloader.bodmodebits=10110",
      "menu.bodmode.samplefast.bootloader.bodmodebits=01010",
      "menu.bodmode.sampleslow.bootloader.bodmodebits=11010",
      "menu.bodmode.sampdisfast.bootloader.bodmodebits=01000",
      "menu.bodmode.sampdisslow.bootloader.bodmodebits=11000",
      "menu.bodmode.endisholdwake.bootloader.bodmodebits=01100",
    ],
  },
  "eeprommenu":{
    "heading":"#----------------------------------------#\n# EEPROM RETAIN                          #\n#________________________________________#",
    "entries":[
      "menu.eesave.enable=EEPROM retained",
      "menu.eesave.enable.bootloader.eesavebit=1",
      "menu.eesave.disable=EEPROM not retained",
      "menu.eesave.disable.bootloader.eesavebit=0",
    ],
  },
  "millismenu":{
    "heading":"#----------------------------------------#\n# MILLIS TIMER                           #\n#________________________________________#",
    "entries":[
      "menu.millis.enabled=Enabled (default timer)",
      "menu.millis.disabled=Disabled (saves flash)",
      "menu.millis.timera=TCA0 (default on 0-series)",
      "menu.millis.timerd=TCD0 (1-series only, default there)",
      "menu.millis.timerb0=TCB0 (breaks tone() and Servo)",
      "menu.millis.timerb1=TCB1 (3217,1617, 2-series only)",
      "menu.millis.rtc=RTC (no micros)",
      "menu.millis.rtcxtal=RTC w/w32.768 kHz ext. crystal (1/2-series only, no micros)",
      "menu.millis.rtcxosc=RTC w/w32.768 kHz ext. oscillator (1/2-series only, no micros)",
      "menu.millis.disabled.build.millistimer=NONE",
      "menu.millis.timera.build.millistimer='A0'",
      "menu.millis.timerd.build.millistimer='D0'",
      "menu.millis.timerb0.build.millistimer='B0'",
      "menu.millis.timerb1.build.millistimer='B1'",
      "menu.millis.rtc.build.millistimer='RTC'",
      "menu.millis.rtcxtal.build.millistimer='RTC_XTAL'",
      "menu.millis.rtcxosc.build.millistimer='RTC_XOSC'",
    ],
  },
  "pinconfigupdimenu":{
    "heading":"#----------------------------------------#\n# UPDI/RESET PIN CONFIGURATION           #\n#________________________________________#",
  },
  "pinconfigoptimenu":{
    "heading":"#----------------------------------------#\n# UPDI/RESET PIN CONFIGURATION           #\n#________________________________________#",
  },
  "sutmenu":{
    "heading":"#----------------------------------------#\n# STARTUP TIME (SUT)                     #\n#________________________________________#",
    "entries":[
      "menu.startuptime.8=8ms",
      "menu.startuptime.0=0ms",
      "menu.startuptime.1=1ms",
      "menu.startuptime.2=2ms",
      "menu.startuptime.4=4ms",
      "menu.startuptime.16=16ms",
      "menu.startuptime.32=32ms",
      "menu.startuptime.64=64ms",
      "menu.startuptime.0.bootloader.SYSCFG1=0x00",
      "menu.startuptime.1.bootloader.SYSCFG1=0x01",
      "menu.startuptime.2.bootloader.SYSCFG1=0x02",
      "menu.startuptime.4.bootloader.SYSCFG1=0x03",
      "menu.startuptime.8.bootloader.SYSCFG1=0x04",
      "menu.startuptime.16.bootloader.SYSCFG1=0x05",
      "menu.startuptime.32.bootloader.SYSCFG1=0x06",
      "menu.startuptime.64.bootloader.SYSCFG1=0x07",
    ],
  },
  "twimenu":{
    "heading":"#----------------------------------------#\n# TWI OPTIONS                            #\n#________________________________________#",
    "entries":[
      "menu.wiremode.mors=Master or Slave (saves flash and RAM)",
      "menu.wiremode.mors.build.wire=MORS",
      "menu.wiremode.mands=Master and Slave",
      "menu.wiremode.mands.build.wire=MANDS",
    ],
  },
  "printfmenu":{
    "heading":"#----------------------------------------#\n# printf() version options               #\n#________________________________________#",
    "entries":[
      "menu.printf.default=Default (doesn't print floats, 1.4k flash use)",
      "menu.printf.full=Full, 2.6k, prints floats",
      "menu.printf.full.build.printf=-Wl,-u,vfprintf -lprintf_flt",
      "menu.printf.full.build.printfabr=.pfF",
      "menu.printf.minimal=Minimal, 1.1k flash used",
      "menu.printf.minimal.build.printf=-Wl,-u,vfprintf -lprintf_min",
      "menu.printf.minimal.build.printfabr=.pfM",
    ],
  },
  "intmodemenu":{
    "heading":"#----------------------------------------#\n# attachInterrupt Mode                   #\n#________________________________________#",
    "entries":[
      "menu.attach.allenabled=On all pins, new implementation.",
      "menu.attach.allenabled.build.attachmode=-DCORE_ATTACH_ALL",
      "menu.attach.manual=Only enabled ports, new implementation, see docs.",
      "menu.attach.manual.build.attachmode=-DCORE_ATTACH_NONE",
      "menu.attach.oldversion=Old version, may fix bugs.",
      "menu.attach.oldversion.build.attachmode=-DCORE_ATTACH_OLD",
      "menu.attach.manual.build.attachabr=.aMan",
      "menu.attach.oldversion.build.attachabr=.aOld",
    ]
  },
  "bootloaderuartmenu":{
    "heading":"#----------------------------------------#\n# Bootloader Pins/UART                   #\n#________________________________________#"
  }
}


seriesbuildparams=[
  {
    "build.millistimer":"A0"
  },
  {
    "bootloader.TCD0CFG_avrdude":"\"-Ufuse4:w:0x00:m\"",
    "bootloader.TCD0CFG_serialupdi":"4:0x00"
  },
  {
    "bootloader.altreset":"11",
    "build.millistimer":"B1",
    "bootloader.SYSCFG0":"0b1111{bootloader.resetpinbits}1{bootloader.eesavebit}"
  }
]

devices = {
  "atxy7":{
    "heading":"############################################################################\n#                                                                          #\n#                                 ##    #  #     ####   ###  #   #       ###\n#  ATtiny3227/3217               #  #   #  #     #   #   #   ##  #     #####\n#  ATtiny1627/1617/1607            #    #### === ####    #   # # #   #######\n#  ATtiny827/817/807              #        #     #       #   #  ##     #####\n#  ATtiny427/417/407             ####      #     #      ###  #   #       ###\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_updi","exportparams","uploadparams_updi","chipname","clockmenu","bodmenu","bodmodemenu","eeprommenu","millismenu","pinconfigupdimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3227":{},
        "3217":{},
        "1627":{},
        "1617":{},
        "1607":{},
        "827":{},
        "817":{},
        "807":{},
        "427":{},
        "417":{},
      }
    }
  },
    "atxy6":{
    "heading":"############################################################################\n#                                                                          #\n#                                 ##    ###      ####   ###  #   #       ###\n#  ATtiny3226/3216               #  #  #   #     #   #   #   ##  #     #####\n#  ATtiny1626/1616/1606            #   #   # === ####    #   # # #   #######\n#  ATtiny826/816/806              #    #   #     #       #   #  ##     #####\n#  ATtiny426/416/406             ####   ###      #      ###  #   #       ###\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_updi","exportparams","uploadparams_updi","board_name","clockmenu","bodmenu","bodmodemenu","eeprommenu","millismenu","pinconfigupdimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3226":{},
        "3216":{},
        "1626":{},
        "1616":{},
        "1606":{},
        "826":{},
        "816":{},
        "806":{},
        "426":{},
        "416":{},
        "406":{},
      }
    }
  },
  "atxy4":{
    "heading":"############################################################################\n#                                                                          #\n#                                  #    #  #     ####   ###  #   #       ###\n#  ATtiny3224/1614/1604           ##    #  #     #   #   #   ##  #     #####\n#  ATtiny1624/814/804              #    #### === ####    #   # # #   #######\n#  ATtiny824/414/404               #       #     #       #   #  ##     #####\n#  ATtiny424/214/204              ###      #     #      ###  #   #       ###\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_updi","exportparams","uploadparams_updi","board_name","clockmenu","bodmenu","bodmodemenu","eeprommenu","millismenu","pinconfigupdimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3224":{},
        "1624":{},
        "1614":{},
        "1604":{},
        "824":{},
        "814":{},
        "804":{},
        "424":{},
        "414":{},
        "404":{},
        "214":{},
        "204":{}
      }
    }
  },
  "atxy2":{
    "heading":"############################################################################\n#                                                                          #\n#                                       ###      ####   ###  #   #       ###\n#  ATtiny412                           #   #     #   #   #   ##  #     #####\n#  ATtiny402                            ###  === ####    #   # # #   #######\n#  ATtiny212                           #   #     #       #   #  ##     #####\n#  ATtiny202                            ###      #      ###  #   #       ###\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_updi","exportparams","uploadparams_updi","board_name","clockmenu","bodmenu","bodmodemenu","eeprommenu","millismenu","pinconfigupdimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "412":{},
        "402":{},
        "212":{},
        "202":{},
        "412w":{
          "upload.workaround=w"
        }
      }
    }
  },
  "microchip":{
    "heading":"############################################################################\n#                                                                          #\n#                   #   # ###  ### ####   ###   ### #  # ### ####        ###\n#                   ## ##  #  #    #   # #   # #    #  #  #  #   #     #####\n# Curiosity Nano    # # #  #  #    ####  #   # #    ####  #  ####    #######\n# Xplained Mini     #   #  #  #    # #   #   # #    #  #  #  #         #####\n# Xplained Pro      #   # ###  ### #  #   ###   ### #  # ### #           ###\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_updi","exportparams","uploadparams_updi","board_name","clockmenu","bodmenu","bodmodemenu","eeprommenu","millismenu","pinconfigupdimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "board_name":{
        "cn3217":{
          "name":"Curiosity Nano ATtiny3217",
          "entries":{"build.board":"AVR_CuriosityNano3217","upload.protocol":"curiosity_updi","upload.maximum_size":"32768","upload.maximum_data_size":"2048","build.variant":"txy7","build.extra_flags":"-DARDUINO_attinyxy7 {build.tuned} -DLED_BUILTIN=PIN_PA3 -DPIN_BUTTON_BUILTIN=PIN_PB7","build.attiny":"3217"}
        },
        "cn1627":{
          "name":"Curiosity Nano ATtiny1627",
          "buildparams":{
            "entries":{"build.board":"AVR_CuriosityNano1627","build.millistimer":"B1","build.attiny":"1627","upload.protocol":"curiosity_updi","upload.maximum_size":"16384","upload.maximum_data_size":"2048","build.variant":"txy7","build.extra_flags":"-DARDUINO_attinyxy7 {build.tuned} -DLED_BUILTIN=PIN_PB4 -DPIN_BUTTON_BUILTIN=PIN_PB7", "build.board":"AVR_CuriosityNano3217"}
          }
        },
        "cn1607":{
          "name":"Curiosity Nano ATtiny1607",
          "buildparams":{
            "entries":{"build.board":"AVR_CuriosityNano1607","build.attiny":"1607","build.millistimer":"A0","upload.protocol":"curiosity_updi","upload.extra_params":"-Pusb","upload.maximum_size":"16384","upload.maximum_data_size":"1024","build.variant":"txy6","build.extra_flags":"-DARDUINO_attinyxy7 {build.tuned} -DLED_BUILTIN=PIN_PC4 -DPIN_BUTTON_BUILTIN=PIN_PB7"}
          }
        },
        "xp3217":{
          "name":"Xplained Pro ATtiny3217",
          "buildparams":{
            "entries":{"build.board":"AVR_Xplained_Pro_3217","build.millistimer":"D0","upload.protocol":"xplainedpro_updi","build.variant":"atxy7","upload.maximum_size":"32768","upload.maximum_data_size":"2048","build.variant":"txy7","build.extra_flags":"-DARDUINO_attinyxy7 {build.tuned} -DLED_BUILTIN=PIN_PA4 -DPIN_BUTTON_BUILTIN=PIN_PB5"}
          }
        },
        "xp817":{
          "name":"Xplained Pro ATtiny817",
          "buildparams":{
            "entries":{"build.board":"AVR_Xplained_Pro_817","build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PB4 -DPIN_BUTTON_BUILTIN=PIN_PB5","build.attiny":"817","build.millistimer":"D0","upload.protocol":"xplainedpro_updi","upload.maximum_size":"8192","upload.maximum_data_size":"512","build.variant":"txy7"}
          }
        },
        "xm817":{
          "name":"Xplained Mini ATtiny817",
          "buildparams":{
            "entries":{"build.board":"AVR_XplainedMini817","build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PC0 -DPIN_BUTTON_BUILTIN=PIN_PC5","build.attiny":"817","build.millistimer":"D0","upload.protocol":"xplainedmini_updi","upload.maximum_size":"8192","upload.maximum_data_size":"512","build.variant":"txy7"}
          }
        },
        "xn416":{
          "name":"Xplained Nano ATtiny416",
          "buildparams":{
            "entries":{"build.board":"AVR_XplainedNano416","build.extra_flags": "-DARDUINO_attinyxy6 -DLED_BUILTIN=PIN_PB5 -DPIN_BUTTON_BUILTIN=PIN_PB4 -DUART0_SWAP","build.attiny":"416","build.variant":"txy6","build.millistimer":"A0","upload.protocol":"xplainedmini_updi","upload.maximum_size":"4096","upload.maximum_data_size":"256","build.variant":"txy6",}
          }
        }
      }
    }
  },
  "HEADER_OPTIBOOT":"##########################################################################\n##########################################################################\n#                                                                        #\n#           ###   ####   #####  ###  ####    ###    ###   #####          #\n#          #   #  #   #    #     #   #   #  #   #  #   #    #            #\n#          #   #  ####     #     #   ####   #   #  #   #    #            #\n#          #   #  #        #     #   #   #  #   #  #   #    #            #\n#           ###   #        #    ###  ####    ###    ###     #            #\n#________________________________________________________________________#\n##########################################################################\n",
  "atxy7o":{
    "heading":"############################################################################\n#  WITH OPTIBOOT                                                           #\n#  0/1-series                   ##    #  #     ####   ###  #   #           #\n#                              #  #   #  #     #   #   #   ##  #           #\n#  ATtiny3217/1617               #    #### === ####    #   # # #           #\n#  ATtiny1607/817               #        #     #       #   #  ##           #\n#  ATtiny807/417               ####      #     #      ###  #   #           #\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_opti","exportparams","uploadparams_opti","board_name","clockomenu","bodmenu","bodmodemenu","millismenu","pinconfigoptimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3227o":{},
        "3217o":{},
        "1627o":{},
        "1617o":{},
        "1607o":{},
        "827o":{},
        "817o":{},
        "807o":{},
        "427o":{},
        "417o":{},
      }
    }
  },
  "atx27o":{
    "heading":"############################################################################\n#  WITH OPTIBOOT                                                    ##   ###\n#  2-series                     ##    #  #     ####   ###  #   #    ##   ###\n#                              #  #   #  #     #   #   #   ##  #    ##   ###\n#  ATtiny3227/1627               #    #### === ####    #   # # #    ##   ###\n#  ATtiny827/427                #        #     #       #   #  ##    ##   ###\n#                              ####      #     #      ###  #   #    ##   ###\n#___________________________________________________________________##___###",
    "sections_include":["buildparams","bootparams_opti","exportparams","uploadparams_opti","board_name","clockomenu","bodmenu","bodmodemenu","millismenu","pinconfigoptimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3227o":{},
        "1627o":{},
        "827o":{},
        "427o":{},
      }
    }
  },
    "atxy6o":{
    "heading":"############################################################################\n#  WITH OPTIBOOT                                                           #\n#  0/1-series                   ##    ###      ####   ###  #   #           #\n#                              #  #  #   #     #   #   #   ##  #           #\n#  ATtiny3216/1616/1606          #   #   # === ####    #   # # #           #\n#  ATtiny816/806                #    #   #     #       #   #  ##           #\n#  ATtiny416/406               ####   ###      #      ###  #   #           #\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_opti","exportparams","uploadparams_opti","board_name","clockomenu","bodmenu","bodmodemenu","millismenu","pinconfigoptimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3226o":{},
        "3216o":{},
        "1626o":{},
        "1616o":{},
        "1606o":{},
        "826o":{},
        "816o":{},
        "806o":{},
        "426o":{},
        "416o":{},
        "406o":{},
      }
    }
  },
  "atxy4o":{
    "heading":"############################################################################\n#  WITH OPTIBOOT                                                           #\n#  0/1-series                    #    #  #     ####   ###  #   #           #\n#                               ##    #  #     #   #   #   ##  #           #\n#  ATtiny1614/1604               #    #### === ####    #   # # #           #\n#  ATtiny814/804/414             #       #     #       #   #  ##           #\n#  ATtiny404/214/204            ###      #     #      ###  #   #           #\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_opti","exportparams","uploadparams_opti","board_name","clockomenu","bodmenu","bodmodemenu","millismenu","pinconfigoptimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "3224o":{},
        "1624o":{},
        "1614o":{},
        "1604o":{},
        "824o":{},
        "814o":{},
        "804o":{},
        "424o":{},
        "414o":{},
        "404o":{},
        "214o":{},
        "204o":{}
      }
    }
  },
  "atxy2o":{
    "heading":"############################################################################\n#  WITH OPTIBOOT                                                           #\n#                                     ###      ####   ###  #   #           #\n#  ATtiny412                         #   #     #   #   #   ##  #           #\n#  ATtiny402                          ###  === ####    #   # # #           #\n#  ATtiny212                         #   #     #       #   #  ##           #\n#  ATtiny202                          ###      #      ###  #   #           #\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_opti","exportparams","uploadparams_opti","board_name","clockomenu","bodmenu","bodmodemenu","millismenu","pinconfigoptimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "chip":{
        "412o":{},
        "402o":{},
        "212o":{},
        "202o":{},
        "412w":{
          "upload.workaround=w"
        }
      }
    }
  },
  "microchipo":{
    "heading":"############################################################################\n#  WITH OPTIBOOT                                                           #\n#                     #   # ###  ### ####   ###   ### #  # ### ####        #\n#                     ## ##  #  #    #   # #   # #    #  #  #  #   #       #\n# Curiosity Nano      # # #  #  #    ####  #   # #    ####  #  ####        #\n# Xplained Mini       #   #  #  #    # #   #   # #    #  #  #  #           #\n# Xplained Pro        #   # ###  ### #  #   ###   ### #  # ### #           #\n#__________________________________________________________________________#",
    "sections_include":["buildparams","bootparams_opti","exportparams","uploadparams_opti","board_name","clockomenu","bodmenu","bodmodemenu","millismenu","pinconfigoptimenu","sutmenu","twimenu","printfmenu","intmodemenu"],
    "menus":{
      "board_name":{
        "cn3217":{
          "name":"Curiosity Nano ATtiny3217",
          "sect":"replace",
          "entries":{
            "build.board":"AVR_CuriosityNano3217",
            "build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PA3 -DPIN_BUTTON_BUILTIN=PIN_PB7 {build.tuned}",
            "bootloader.file":"optiboot_Curiosity3217.hex",
            "build.attiny":"3217",
            "build.variant":"txy7",
            "upload.maximum_size":"32256",
            "upload.maximum_data_size":"2048"
          }
        },
        "cn1627o":{
          "name":"Curiosity Nano ATtiny1627",
          "sect":"replace",
          "entries":{
            "build.board":"AVR_CuriosityNano1627",
            "build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PB4 -DPIN_BUTTON_BUILTIN=PIN_PB7 {build.tuned}",
            "bootloader.file":"optiboot_Curiosity1627.hex",
            "build.millistimer":"B1",
            "build.attiny":"1627",
            "build.variant":"txy7",
            "upload.maximum_size":"15872",
            "upload.maximum_data_size":"2048",
            "bootloader.SYSCFG0":"0b1111{bootloader.resetpinbits}10"
          }
        },
        "cn1607o":{
          "name":"Curiosity Nano ATtiny1607",
          "sect":"replace",
          "entries":{
            "build.board":"AVR_CuriosityNano1607",
            "build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PC4 -DPIN_BUTTON_BUILTIN=PIN_PB7 {build.tuned}",
            "bootloader.file":"optiboot_Curiosity1607.hex",
            "build.attiny":"1607",
            "build.variant":"txy7",
            "build.millistimer":"A0",
            "upload.maximum_size":"15872",
            "upload.maximum_data_size":"1024"
          }
        },
        "xp3217o":{
          "name":"Xplained Pro ATtiny3217",
          "sect":"replace",
          "buildparams":{
            "entries":{
              "build.board":"AVR_XplainedPro3217",
              "build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PB4 -DPIN_BUTTON_BUILTIN=PIN_PB5 {build.tuned}",
              "bootloader.file":"optiboot_XplainedPro3217.hex",
              "build.attiny":"3217",
              "build.variant":"txy7",
              "build.millistimer":"D0",
              "upload.maximum_size":"32256",
              "upload.maximum_data_size":"2048"
            }
          }
        },
        "xp817o":{
          "name":"Xplained Pro ATtiny817",
          "sect":"replace",
          "buildparams":{
            "entries":{
              "build.board":"AVR_XplainedPro817",
              "build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PB4 -DPIN_BUTTON_BUILTIN=PIN_PB5 {build.tuned}",
              "bootloader.file":"optiboot_XplainedPro817.hex",
              "build.attiny":"817",
              "build.variant":"txy7",
              "build.millistimer":"D0",
              "upload.maximum_size":"7680",
              "upload.maximum_data_size":"512"
            }
          }
        },
        "xm817o":{
          "name":"Xplained Mini ATtiny817",
          "sect":"replace",
          "buildparams":{
            "entries":{
              "build.board":"AVR_XplainedMini817",
              "build.extra_flags":"-DARDUINO_attinyxy7 -DLED_BUILTIN=PIN_PC0 -DPIN_BUTTON_BUILTIN=PIN_PC5 {build.tuned}",
              "bootloader.file":"optiboot_XplainedMini817.hex",
              "build.attiny":"817",
              "build.variant":"txy7",
              "build.millistimer":"D0",
              "upload.maximum_size":"7680",
              "upload.maximum_data_size":"512"
            }
          }
        },
        "xn416":{
          "name":"Xplained Nano ATtiny416",
          "sect":"replace",
          "buildparams":{
            "entries":{
              "build.board":"AVR_XplainedNano416",
              "build.extra_flags":"-DARDUINO_attinyxy6 -DLED_BUILTIN=PIN_PB5 -DPIN_BUTTON_BUILTIN=PIN_PB4 -DUART0_SWAP {build.tuned}",
              "bootloader.file":"optiboot_XplainedNano416.hex",
              "build.attiny":"416",
              "build.variant":"txy6",
              "build.millistimer":"D0",
              "upload.maximum_size":"3584",
              "upload.maximum_data_size":"256"
            }
          }
        }
      }
    }
  }
}

BADDATA = "An attempt was made to look up the specs for a part that does not exist"

flashtosram={"2":["128","256",BADDATA],"4":["256","256","512"],"8":["512","512","1024"],"16":["1024","2048","2048"],"32":[BADDATA,"2048","3072"]}

def printProp(board,string):
  print(board+"="+string)
  print(board+"="+string,file = f1)
def printLit(string):
  print(string)
  print(string,file = f1)

def printList(lines):
  for x in lines:
    printProp(x,line)

def printMenuItem(board,menuname,menuoption,optionprop):
  printProp(x,"menu."+menuname+"."+menuoption+"."+optionprop)
def printMenuItems(board,menuname,menuoption,optionproplist):
  for z in optionproplist:
    printProp(x,"menu."+menuname+"."+menuoption+"."+z)

def partNumberToSeries(partnum):
  retval=int(partnum[2])
  if retval != 0 and retval !=1 and retval != 2:
    print("An attempt was made to look up the series for a "+partnum)
  return retval
def partNumberToPinCount(partnum):
  pcs=[-1,-1,8,-1,14,-1,20,24]
  retval=pcs[int(partnum[3])]
  if retval==-1:
    print("An attempt was made to look up the pincount for a "+partnum)
  return retvalue

def partNumberToFlash(partnum):
  if len(partnum) < 3:
    print("An attempt was made to look up the flash for nonexistent chip "+partnum)

def nameToNum(name):
  retval = name
  if len(name) == 0 or len (name) >8:
    print("nameToNum passed invalid value "+name)
  retval = retval.lstrip("xcnmpfw")
  retval = retval.rstrip("o")
  return retval

def printProp(board,string):
  print(board+"."+string)
  print(board+"."+string, file = f1)
def printLit(string):
  print(string)
  print(string,file = f1)

def printSection(sectname):
  sbs = subsections[sectname]
  printLit('\n'+sbs["heading"])

  for entry in sbs["entries"]:
    if entry == "chipname":
      for ch in dev[""]
    else:
      printProp(dev,entry)


fhead = open("boards_header.txt")
fheadtext=fhead.read()
fhead.close()
f1 = open("boards.txt","w")
printLit(fheadtext)
for dev in devices:
   if (dev == "HEADER_OPTIBOOT"):
     printLit(devices[dev])
   else:
     td=devices[dev];
     print(td)
     printLit(td["heading"])
     for sect in td["sections_include"]:
      printSection(sect)



