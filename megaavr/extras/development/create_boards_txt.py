import re

subsections={
  "buildparams":{
    "heading":"#                                        #\n# BUILD PARAMETERS                       #\n#________________________________________#",
    "entries":[
      "build.text_section_start=.text=0x0",
      "build.millistimer=D0",
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
  "chipmenu":{
    "heading":"                 SUBMENUS                #\n#_____________________________________________#\n# CHIP                                   #\n#________________________________________#",
  },
  "clockmenu":{
    "heading":"#----------------------------------------#\n# CLOCK SOURCE                           #\n#________________________________________#",
    "menuname":("optiboot","clocko","clock"),
    "entries":[
      "20internal":{
        "name":"20 MHz internal",
        "params":[
          "build.clocksource=0",
          "build.speed=20",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "20internal.build.clocksource=0",
      "20internal.build.speed=20",
      "20internal.bootloader.OSCCFG=0x02",
      "16internal":{
        "name":"16 MHz internal",
        "params":[
          "build.speed=16",
          "build.clocksource=0",
        ]
      },
      "16internal.build.speed=16",
      "16internal.build.clocksource=0",
      "10internal":{
        "name":"10 MHz internal",
        "params":[
          "build.clocksource=0",
          "build.speed=10",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "10internal.build.clocksource=0",
      "10internal.build.speed=10",
      "10internal.bootloader.OSCCFG=0x02",
      "8internal":{
        "name":"8 MHz internal",
        "params":[
          "build.speed=8",
          "build.clocksource=0",
        ]
      },
      "8internal.build.speed=8",
      "8internal.build.clocksource=0",
      "5internal":{
        "name":"5 MHz internal",
        "params":[
          "build.clocksource=0",
          "build.speed=5",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "5internal.build.clocksource=0",
      "5internal.build.speed=5",
      "5internal.bootloader.OSCCFG=0x02",
      "4internal":{
        "name":"4 MHz internal",
        "params":[
          "build.speed=4",
          "build.clocksource=0",
        ]
      },
      "4internal.build.speed=4",
      "4internal.build.clocksource=0",
      "2internal":{
        "name":"2 MHz internal",
        "params":[
          "build.clocksource=0",
          "build.speed=2",
        ]
      },
      "2internal.build.clocksource=0",
      "2internal.build.speed=2",
      "1internal":{
        "name":"1 MHz internal",
        "params":[
          "build.speed=1",
          "build.clocksource=0",
        ]
      },
      "1internal.build.speed=1",
      "1internal.build.clocksource=0",
      "20extclock":{
        "name":"20 MHz external clock",
        "params":[
          "build.speed=20",
          "build.clocksource=2",
        ]
      },
      "16extclock":{
        "name":"16 MHz external clock",
        "params":[
          "build.speed=16",
          "build.clocksource=2",
        ]
      },
      "12extclock":{
        "name":"12 MHz external clock",
        "params":[
          "build.speed=12",
          "build.clocksource=2",
        ]
      },
      "10extclock":{
        "name":"10 MHz external clock",
        "params":[
          "build.speed=10",
          "build.clocksource=2",
        ]
      },
      "8extclock":{
        "name":"8 MHz external clock",
        "params":[
          "build.speed=8"
          "build.clocksource=2",
        ]
      },
      "24extclock":{
        "name":"24 MHz external clock - overclocked",
        "params":[
          "build.speed=24",
          "build.clocksource=2",
        ]
      },
      "25extclock":{
        "name":"25 MHz external clock - overclocked",
        "params":[
          "build.speed=25",
          "build.clocksource=2",
        ]
      },
      "30extclock":{
        "name":"30 MHz external clock - overclocked, may be unstable",
        "params":[
          "build.speed=30",
          "build.clocksource=2",
        ]
      },,
      "32extclock":{
        "name":"32 MHz external clock - overclocked, may be unstable",
        "params":[
          "build.speed=32",
          "build.clocksource=2",
        ]
      },
      "20internaltuned":{
        "name":"20 MHz internal - tuned",
        "params":[
          "build.clocksource=0",
          "build.speed=20",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "20internaltuned.build.clocksource=0",
      "20internaltuned.build.speed=20",
      "20internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "20internaltuned.bootloader.OSCCFG=0x02",
      "16internaltuned":{
        "name":"16 MHz internal - tuned",
        "params":[
          "build.speed=16",
          "build.clocksource=0",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
        ]
      },
      "16internaltuned.build.speed=16",
      "16internaltuned.build.clocksource=0",
      "16internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "24internaltuned":{
        "name":"24 MHz internal - tuned, overclocked",
        "params":[
          "build.clocksource=0",
          "build.speed=24",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "24internaltuned.build.clocksource=0",
      "24internaltuned.build.speed=24",
      "24internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "24internaltuned.bootloader.OSCCFG=0x02",
      "25internaltuned":{
        "name":"25 MHz internal - tuned, overclocked",
        "params":[
          "build.clocksource=0",
          "build.speed=25",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "30internaltuned":{
        "name":"30 MHz internal - tuned, overclocked, may be unstable, req. 20 MHz fuse",
        "params":[
          "build.clocksource=0",
          "build.speed=30",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "30internaltuned.build.clocksource=0",
      "30internaltuned.build.speed=30",
      "30internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "30internaltuned.bootloader.OSCCFG=0x02",
      "32internaltuned":{
        "name":"32 MHz internal - tuned, 2-series only, overclocked, may be unstable, req. 20 MHz fuse",
        "params":[
          "build.clocksource=0",
          "build.speed=32",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "32internaltuned.build.clocksource=0",
      "32internaltuned.build.speed=32",
      "32internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "32internaltuned.bootloader.OSCCFG=0x02",
      "12internaltuned":{
        "name":"12 MHz internal - tuned",
        "params":[
          "build.clocksource=0",
          "build.speed=12",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "12internaltuned.build.clocksource=0",
      "12internaltuned.build.speed=12",
      "12internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "12internaltuned.bootloader.OSCCFG=0x02",
      "10internaltuned":{
        "name":"10 MHz internal - tuned",
        "params":[
          "build.clocksource=0",
          "build.speed=10",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "10internaltuned.build.clocksource=0",
      "10internaltuned.build.speed=10",
      "10internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "10internaltuned.bootloader.OSCCFG=0x02",
      "8internaltuned":{
        "name":"8 MHz internal - tuned",
        "params":[
          "build.speed=8",
          "build.clocksource=0",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
        ]
      },
      "8internaltuned.build.speed=8",
      "8internaltuned.build.clocksource=0",
      "8internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "6internaltuned":{
        "name":"6 MHz internal - tuned",
        "params":[
          "build.speed=6",
          "build.clocksource=0",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
        ]
      },
      "6internaltuned.build.speed=6",
      "6internaltuned.build.clocksource=0",
      "6internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "5internaltuned":{
        "name":"5 MHz internal - tuned",
        "params":[
          "build.clocksource=0",
          "build.speed=5",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "bootloader.OSCCFG=0x02",
        ]
      },
      "5internaltuned.build.clocksource=0",
      "5internaltuned.build.speed=5",
      "5internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "5internaltuned.bootloader.OSCCFG=0x02",
      "4internaltuned":{
        "name":"4 MHz internal - tuned",
        "params":[
          "build.speed=4",
          "build.clocksource=0",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
        ]
      },
      "4internaltuned.build.speed=4",
      "4internaltuned.build.clocksource=0",
      "4internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "2internaltuned":{
        "name":"2 MHz internal - tuned",
        "params":[
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
          "build.speed=2",
          "build.clocksource=0",
        ]
      },
      "2internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
      "2internaltuned.build.speed=2",
      "2internaltuned.build.clocksource=0",
      "1internaltuned":{
        "name":"1 MHz internal - tuned",
        "params":[
          "build.speed=1",
          "build.clocksource=0",
          "build.tuned=-DCLOCK_TUNE_INTERNAL",
        ]
      },
      "1internaltuned.build.speed=1",
      "1internaltuned.build.clocksource=0",
      "1internaltuned.build.tuned=-DCLOCK_TUNE_INTERNAL",
    ]
  },
  "bodmenu":{
    "heading":"#----------------------------------------#\n# BOD VOLTAGE                            #\n#________________________________________#",
    "menuname":"bodvoltage",
    "entries":[
      "1v8":{
        "name":"1.8V (5 MHz or less)",
        "params":[
          "bootloader.bodlevbits=000"
        ]
      },
      "2v6":{
        "name":"2.6V (10 MHz or less)",
        "params":[
          "bootloader.bodlevbits=001"
        ]
      },
      "4v2":{
        "name":"4.2V (20 MHZ or less)",
        "params":[
          "bootloader.bodlevbits=111"
        ]
      },
      "2v1":{
        "name":"2.1V (Unofficial)",
        "params":[
          "bootloader.bodlevbits=010"
        ]
      },
      "2v9":{
        "name":"2.9V (Unofficial)",
        "params":[
          "bootloader.bodlevbits=011"
        ]
      },
      "3v3":{
        "name":"3.3V (Unofficial)",
        "params":[
          "bootloader.bodlevbits=100"
        ]
      },
      "3v7":{
        "name":"3.7V (Unofficial)",
        "params":[
          "bootloader.bodlevbits=101"
        ]
      },
      "4v0":{
        "name":"4.0V (Unofficial)",
        "params":[
          "bootloader.bodlevbits=110"
        ]
      },
    ],
  },
  "bodmodemenu":{
    "heading":"#----------------------------------------#\n# BOD MODE                               #\n#________________________________________#",
    "menuname":"bodmode",
    "entries":[
      "disabled":{
        "name":"Disabled/Disabled",
        "params":[
          "bootloader.BODCFG=0x00"
        ]
      },
      "enabled":{
        "name":"Enabled/Enabled",
        "params":[
          "bootloader.bodmodebits=00101"
        ]
      },
      "ensampfast":{
        "name":"Enabled/Sampled (1 kHz)",
        "params":[
          "bootloader.bodmodebits=00110"
        ]
      },
      "ensampslow":{
        "name":"Enabled/Sampled (125 Hz)",
        "params":[
          "bootloader.bodmodebits=10110"
        ]
      },
      "sampledfast":{
        "name":"Sampled/Sampled (1 kHz)",
        "params":[
          "bootloader.bodmodebits=01010"
        ]
      },
      "sampledslow":{
        "name":"Sampled/Sampled (125 Hz)",
        "params":[
          "bootloader.bodmodebits=11010"
        ]
      },
      "sampdisfast":{
        "name":"Sampled/Disabled (1 kHz)",
        "params":[
          "bootloader.bodmodebits=01000"
        ]
      },
      "sampdisslow":{
        "name":"Sampled/Disabled (125 Hz)",
        "params":[
          "bootloader.bodmodebits=11000"
        ]
      },
      "endisholdwake":{
        "name":"Enabled hold wakeup/Disabled",
        "params":[
          "bootloader.bodmodebits=01100"
        ]
      },
    ],
  },
  "eeprommenu":{
    "heading":"#----------------------------------------#\n# EEPROM RETAIN                          #\n#________________________________________#",
    "menuname":"eesave",
    "entries":[
      "enable":{
        "name":"EEPROM retained",
        "params":[
          "bootloader.eesavebit=1"
        ]
      },
      "disable":{
        "name":"EEPROM not retained",
        "params":[
          "bootloader.eesavebit=0"
        ]
      },
    ],
  },
  "millismenu":{
    "heading":"#----------------------------------------#\n# MILLIS TIMER                           #\n#________________________________________#",
    "menuname":"millis",
    "entries":[
      "enabled":{
        "name":"Default - TCA0 on 0-series, TCD0 on 1-series, TCB1 on 2-series)",
        "params":[
          ...
        ]
      },
      "disabled":{
        "name":"Disabled (saves flash)",
        "params":[
          "build.millistimer=NONE"
        ]
      },
      "timera":{
        "name":"TCA0 (default on 0-series)",
        "params":[
          "build.millistimer=A0"
        ]
      },
      "timerd":{
        "name":"TCD0 (1-series only, default there)",
        "params":[
          "build.millistimer=D0"
        ]
      },
      "timerb0":{
        "name":"TCB0 (breaks tone() and Servo)",
        "params":[
          "build.millistimer=B0"
        ]
      },
      "timerb1":{
        "name":"TCB1 (2-series, 16/32k 1-series only. Smallest overhead)",
        "params":[
          "build.millistimer=B1"
        ]
      },
      "rtc":{
        "name":"RTC (no micros, not very accurate)",
        "params":[
          "build.millistimer=RTC",
        ]
      },
      "rtcxtal":{
        "name":"RTC w/w32.768 kHz ext. crystal (1/2-series only, no micros)",
        "params":[
          "build.millistimer=RTC_XTAL",
        ]
      },
      "rtcxosc":{
        "name":"RTC w/w32.768 kHz ext. oscillator (1/2-series only, no micros)",
        "params":[
          "build.millistimer=RTC_XOSC"
        ]
      },
    ],
  },
  "pinconfigupdimenu":{
    "heading":"#----------------------------------------#\n# UPDI/RESET PIN CONFIGURATION           #\n#________________________________________#",
    "menuname":"resetpin",
    "entries":[
      "UPDI":{
        "name":"UPDI (no reset pin)",
        "params":[
          "UPDI.bootloader.resetpinbits=01"
        ]
      },
      "UPDIaltrst":{
        "name":"UPDI w/reset on PB4 (2-series only)",
        "params":[
          "bootloader.resetpinbits={bootloader.altreset}",
        ]
      }
    ]
  },
  "pinconfigoptimenu"{
    "heading":"#----------------------------------------#\n# UPDI/RESET PIN CONFIGURATION           #\n#________________________________________#",
    "menuname":"resetpinopti",
    "entries":[
      "UPDIaltrst":{
        "name":"UPDI on PA0, RST on PB4 (Optiboot for 1 sec after reset)",
        "params":[
          "bootloader.file=optiboot_tx2z{bootloader.uartswap}_rst.hex",
          "bootloader.resetpinbits=11",
        ]
        "series":"2",
        "pincount":("20","24")
      },

      "UPDI":{
        "name":"UPDI, Optiboot for 8 sec. after power-on & reset",
        "params":[
          "bootloader.file=optiboot_tx2z{bootloader.uartswap}.hex",
          "bootloader.resetpinbits=01",
        ]
      },
      "UPDIFastBoot":{
        "name":"UPDI, Optiboot for 1 sec. after power-on & reset",
        "params":[
          "bootloader.file=optiboot_tx2z{bootloader.uartswap}_1sec.hex",
          "bootloader.resetpinbits=01",
        ]
      },

      "reset":{
        "name":"Reset, Optiboot for 1 sec. after reset *DANGER* HV UPDI programmer req'd for UPDI reprogramming!",
        "params":[
          "bootloader.file=optiboot_tx2z{bootloader.uartswap}_rst.hex",
          "bootloader.resetpinbits=10",
        ]
      },
      "gpio":{
        "name":"GPIO, Optiboot for 8 sec. after power-on & SW reset *DANGER* HV programmer req'd for UPDI reprogramming! (PCHV)",
        "params":[
          "bootloader.file=optiboot_tx2z{bootloader.uartswap}.hex",
          "bootloader.resetpinbits=00",
        ]
      },
    ]
  }
  "sutmenu":{
    "heading":"#----------------------------------------#\n# STARTUP TIME (SUT)                     #\n#________________________________________#",
    "menuname":"startuptime",
    "entries":[
      "8":{
        "name":"8ms",
        "params":[
          "bootloader.SYSCFG1=0x04",
        ]
      },
      "0":{
        "name":"0ms",
        "params":[
          "bootloader.SYSCFG1=0x00",
        ]
      },
      "1":{
        "name":"1ms",
        "params":[
          "bootloader.SYSCFG1=0x01",
        ]
      },
      "2":{
        "name":"2ms",
        "params":[
          "bootloader.SYSCFG1=0x02",
        ]
      },
      "4":{
        "name":"4ms",
        "params":[
          "bootloader.SYSCFG1=0x03",
        ]
      },
      "16":{
        "name":"16ms",
        "params":[
          "bootloader.SYSCFG1=0x05",
        ]
      },
      "32":{
        "name":"32ms",
        "params":[
          "bootloader.SYSCFG1=0x06",
        ]
      },
      "64":{
        "name":"64ms",
        "params":[
          "bootloader.SYSCFG1=0x07",
        ]
      },
    ],
  },
  "twimenu":{
    "heading":"#----------------------------------------#\n# TWI OPTIONS                            #\n#________________________________________#",
    "menuname":"wiremode",
    "entries":[
      "mors":{
        "name":"Master or Slave (saves flash and RAM)",
        "params":[
          "build.wire=MORS",
        ]
      },

      "mands":{
        "name":"Master and Slave",
        "params":[
          "build.wire=MANDS",
        ]
      },
    ],
  },
  "printfmenu":{
    "heading":"#----------------------------------------#\n# printf() version options               #\n#________________________________________#",
    "menuname":"printf",
    "entries":[
      "default":{
        "name":"Default (doesn't print floats, 1.4k flash use)",
      },
      "full":{
        "name":"Full, 2.6k, prints floats",
        "params":[
          "build.printf=-Wl,-u,vfprintf -lprintf_flt",
          "build.printfabr=.pfF",
        ]
      },
      "minimal":{
        "name":"Minimal, 1.1k flash used",
        "params":[
          "build.printf=-Wl,-u,vfprintf -lprintf_min",
          "build.printfabr=.pfM",
        ]
      },
    ],
  },
  "intmodemenu":{
    "heading":"#----------------------------------------#\n# attachInterrupt Mode                   #\n#________________________________________#",
    "menuname":"attach",
    "entries":[
      "allenabled":{
        "name":"On all pins, new implementation.",
        "params":[
          "build.attachmode=-DCORE_ATTACH_ALL"
        ]
      },
      "manual":{
        "name":"Only enabled ports, new implementation, see docs.",
        "params":[
          "manual.build.attachmode=-DCORE_ATTACH_NONE",
          "build.attachabr=.aMan"
        ]
      },
      "oldversion":{
        "name":"Old version, may fix bugs.",
        "params":[
          "build.attachmode=-DCORE_ATTACH_OLD",
          "build.attachabr=.aOld"
        ]
      }
    ]
  },
  "bootloaderuartmenu":{
    "heading":"#----------------------------------------#\n# Bootloader Pins/UART                   #\n#________________________________________#",
    "menuname":"bootloaderuart"
    "entries":[
      "default":{
        "name":"TX: PB2, RX: PB3 (UART0 default pins)",
        "params":[
          "bootloader.uartswap="
        ]
      },
      "alternate":{
        "name":"TX: PA1, RX: PA2 (UART0 alt pins)",
        "params":[
          "bootloader.uartswap=_alt"
        ]
      },
      "alternate1":{
        "name":"TX: PC2, RX: PC1 (UART1 alt pins)",
        "params":[
          "bootloader.uartswap=_u1alt"
        ]
        "series":"2",
        "pincount":("20","24")
      },
    ]
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



