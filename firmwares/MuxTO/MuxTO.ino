/*
   j2updi.cpp

   Created: 11-11-2017 22:29:58
   Author : JMR_2
   Ported to generic Arduino framework by the Arduino Team - 2019
*/

/*
    Compile with
    fqbn=arduino:samd:muxto:float=default,config=enabled,clock=internal_usb,timer=timer_732Hz,bootloader=4kb,serial=two_uart,usb=cdc
    to obtain the binary for Serial-to-USB converter + UPDI programmer on the Arduino Nano Every (MuxTO for brevity)

    Since the "sketch" is (almost) pure Arduino it can be ported easily to other architectures (on chips with two serial ports)
    The bitbanged UPDI layer is being reworked to make it compatible with chips with just one UART (eg. atmega 32u4)
*/

// Includes
#include "sys.h"
#include "updi_io.h"
#include "JICE_io.h"
#include "JTAG2.h"

volatile bool updi_mode = false;
unsigned long baudrate = 115200;
unsigned long updi_mode_start = 0;
unsigned long updi_mode_end = 0;
uint8_t stopbits = 1;
uint8_t paritytype = 0;
uint8_t numbits = 8;
uint16_t serial_mode = SERIAL_8N1;
bool serialNeedReconfiguration = false;

char support_buffer[64];

void setup() {
  /* Initialize MCU */
  pinMode(LED_BUILTIN, OUTPUT);

  /* Initialize serial links */
  JICE_io::init();
  UPDI_io::init();
  Serial1.begin(baudrate, serial_mode);
}

//#define DEBUG;

//long blink_timer = 0;
//long blink_delay = 1000;

void loop() {

#ifdef DEBUG
  if (millis() - blink_timer > blink_delay) {
    SYS::toggleLED();
    blink_timer = millis();
  }
#endif

  if (updi_mode == false) {

    //blink_delay = 1000;

    if (int c = Serial1.available()) {
      if (c > Serial.availableForWrite()) {
        c = Serial.availableForWrite();
      }
      Serial1.readBytes(support_buffer, c);
      Serial.write(support_buffer, c);
    }

    if (int c = Serial.available()) {
      if (c > Serial1.availableForWrite()) {
        c = Serial1.availableForWrite();
      }
      Serial.readBytes(support_buffer, c);
      Serial1.write(support_buffer, c);
    }

    if (Serial.stopbits() != stopbits) {
      serial_mode &= ~HARDSER_STOP_BIT_MASK;
      stopbits = Serial.stopbits();
      serialNeedReconfiguration = true;
      switch (stopbits) {
        case 1:
          serial_mode |= HARDSER_STOP_BIT_1;
          break;
        case 2:
          serial_mode |= HARDSER_STOP_BIT_2;
          break;
      }
    }
    if (Serial.paritytype() != paritytype) {
      serial_mode &= ~HARDSER_PARITY_MASK;
      paritytype = Serial.paritytype();
      serialNeedReconfiguration = true;
      switch (paritytype) {
        case 0:
          serial_mode |= HARDSER_PARITY_NONE;
          break;
        case 1:
          serial_mode |= HARDSER_PARITY_EVEN;
          break;
        case 2:
          serial_mode |= HARDSER_PARITY_ODD;
          break;
      }
    }
    if (Serial.numbits() != numbits) {
      serial_mode &= ~HARDSER_DATA_MASK;
      numbits = Serial.numbits();
      serialNeedReconfiguration = true;
      switch (numbits) {
        case 5:
          serial_mode |= HARDSER_DATA_5;
          break;
        case 6:
          serial_mode |= HARDSER_DATA_6;
          break;
        case 7:
          serial_mode |= HARDSER_DATA_7;
          break;
        case 8:
          serial_mode |= HARDSER_DATA_8;
          break;
      }
    }

    if (baudrate == 1200 && Serial.dtr() == 0 && (millis() - updi_mode_end > 200)) {
      // don't reenter here if you just finished flashing
      updi_mode = true;
      updi_mode_start = millis();
      updi_mode_end = 0;
    }

    if (Serial.baud() != baudrate || serialNeedReconfiguration) {
      if (Serial.dtr() == 1) {
        baudrate = Serial.baud();
        Serial1.end();
        if (baudrate != 1200) {
          Serial1.begin(baudrate, serial_mode);
          serialNeedReconfiguration = false;
        }
      }
    }
    return;
  }

  if (updi_mode == true) {

    // updi_mode cannot last more than 1 minute; in that case, break forcibly
    if ((updi_mode_end != 0 && (millis() - updi_mode_end) > 500) || ((millis() - updi_mode_start) > 60000)) {
      updi_mode = false;
      baudrate = -1;
      return;
    }

    //blink_delay = 100;

    // Receive command
    if (!JTAG2::receive()) {
      return;
    }
    // Process command
    switch (JTAG2::packet.body[0]) {
      case JTAG2::CMND_GET_SIGN_ON:
        JTAG2::sign_on();
        break;
      case JTAG2::CMND_GET_PARAMETER:
        JTAG2::get_parameter();
        break;
      case JTAG2::CMND_SET_PARAMETER:
        JTAG2::set_parameter();
        break;
      case JTAG2::CMND_RESET:
      case JTAG2::CMND_ENTER_PROGMODE:
        JTAG2::enter_progmode();
        break;
      case JTAG2::CMND_SIGN_OFF:
        // Restore default baud rate before exiting
        JTAG2::PARAM_BAUD_RATE_VAL = JTAG2::baud_19200;
      case JTAG2::CMND_LEAVE_PROGMODE:
        JTAG2::leave_progmode();
        updi_mode_end = millis();
        break;
      case JTAG2::CMND_GET_SYNC:
      case JTAG2::CMND_GO:
        JTAG2::set_status(JTAG2::RSP_OK);
        break;
      case JTAG2::CMND_SET_DEVICE_DESCRIPTOR:
        JTAG2::set_device_descriptor();
        break;
      case JTAG2::CMND_READ_MEMORY:
        JTAG2::read_mem();
        break;
      case JTAG2::CMND_WRITE_MEMORY:
        JTAG2::write_mem();
        break;
      case JTAG2::CMND_XMEGA_ERASE:
        JTAG2::erase();
        break;
      default:
        JTAG2::set_status(JTAG2::RSP_FAILED);
        break;
    }
    // send response
    JTAG2::answer();
    // some commands need to be executed after sending the answer
    JTAG2::delay_exec();
  }
}
