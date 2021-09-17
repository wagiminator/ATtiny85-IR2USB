// IR2USB - IR to USB HID Keyboard Converter Example
//
//                           +-\/-+
//         --- A0 (D5) PB5  1|    |8  Vcc
// VUSB D- --- A3 (D3) PB3  2|    |7  PB2 (D2) A1 --- 
// VUSB D+ --- A2 (D4) PB4  3|    |6  PB1 (D1) ------ IR Receiver
//                     GND  4|    |5  PB0 (D0) ------ LED
//                           +----+
//
// Controller:  ATtiny45/85
// Core:        VUSB-AVR (https://github.com/wagiminator/VUSB-AVR)
// Clockspeed:  16.5 MHz internal
//
// 2021 by Stefan Wagner 
// Project Files (EasyEDA): https://easyeda.com/wagiminator
// Project Files (Github):  https://github.com/wagiminator
// License: http://creativecommons.org/licenses/by-sa/3.0/

// Libraries
#include "VUSB_Keyboard.h"

// Pin definitions
#define LED_PIN       0       // Pin for LED
#define IR_PIN        1       // Pin for IR receiver

// IR codes
#define IR_ADDR       0x1A    // IR device address
#define IR_KEY1       0x01    // IR code for key 1
#define IR_KEY2       0x02    // IR code for key 2
#define IR_KEY3       0x03    // IR code for key 3
#define IR_KEY4       0x04    // IR code for key 4
#define IR_KEY5       0x05    // IR code for key 5
#define IR_FAIL       0xFF    // IR fail code

// Keyboard scan codes
#define KEY_NONE      0x00
#define KEY_ENTER     0x28
#define KEY_ESC       0x29
#define KEY_BACKSPACE 0x2a
#define KEY_SPACE     0x2c
#define KEY_PAGEUP    0x4b
#define KEY_PAGEDOWN  0x4e
#define KEY_RIGHT     0x4f
#define KEY_LEFT      0x50
#define KEY_DOWN      0x51
#define KEY_UP        0x52
#define KEY_MUTE      0xe2
#define KEY_VOLUMEUP  0xe9
#define KEY_VOLUMEDOWN 0xea

// -----------------------------------------------------------------------------
// IR Receiver Implementation (NEC Protocol)
// -----------------------------------------------------------------------------

// IR receiver definitions and macros
#define IR_init()       PORTB |= (1<<IR_PIN)  // pullup on IR pin
#define IR_available()  (~PINB & (1<<IR_PIN)) // return true if IR line is low

// IR wait for signal change and measure duration
uint8_t IR_waitChange(uint8_t timeout) {
  uint8_t pinState = PINB & (1<<IR_PIN);      // get current signal state
  uint8_t dur = 0;                            // variable for measuring duration
  while ((PINB & (1<<IR_PIN)) == pinState) {  // measure length of signal
    if (dur++ > timeout) return 0;            // exit if timeout
    _delay_us(100);                           // count every 100us
  }
  return dur;                                 // return time in 100us
}

// IR read data byte
uint8_t IR_readByte(void) {
  uint8_t result;
  uint8_t dur;
  for (uint8_t i=8; i; i--) {                 // 8 bits
    result >>= 1;                             // LSB first
    if (IR_waitChange(11) < 3) return IR_FAIL;// exit if wrong burst length
    dur = IR_waitChange(21);                  // measure length of pause
    if (dur <  3) return IR_FAIL;             // exit if wrong pause length
    if (dur > 11) result |= 0x80;             // bit "0" or "1" depends on pause duration
  }
  return result;                              // return received byte
}

// IR read data according to NEC protocol
uint8_t IR_read(void) {
  uint16_t addr;                              // variable for received address
  if (!IR_available())        return IR_FAIL; // exit if no signal
  if (!IR_waitChange(100))    return IR_FAIL; // exit if wrong start burst length
  if (IR_waitChange(55) < 35) return IR_FAIL; // exit if wrong start pause length

  uint8_t addr1 = IR_readByte();              // get first  address byte
  uint8_t addr2 = IR_readByte();              // get second address byte
  uint8_t cmd1  = IR_readByte();              // get first  command byte
  uint8_t cmd2  = IR_readByte();              // get second command byte

  if (IR_waitChange(11) < 3)  return IR_FAIL; // exit if wrong final burst length
  if ((cmd1 + cmd2) < 255)    return IR_FAIL; // exit if command bytes are not inverse
  if ((addr1 + addr2) == 255) addr = addr1;   // check if it's extended NEC-protocol ...
  else addr = ((uint16_t)addr2 << 8) | addr1; // ... and get the correct address
  if (addr != IR_ADDR)        return IR_FAIL; // wrong address
  return cmd1;                                // return command code
}

// -----------------------------------------------------------------------------
// Main Function
// -----------------------------------------------------------------------------

void setup() {
  DDRB |= (1<<LED_PIN);                       // set LED pin as output
  IR_init();                                  // init IR receiver
}

void loop() {
  if(IR_available()) {
    PORTB |= (1<<LED_PIN);
    uint8_t command = IR_read();
    switch (command) {
      case IR_KEY1:   VUSB_Keyboard.sendKeyStroke(KEY_SPACE); break;
      case IR_KEY2:   VUSB_Keyboard.sendKeyStroke(KEY_BACKSPACE); break;
      case IR_KEY3:   VUSB_Keyboard.sendKeyStroke(KEY_PAGEUP); break;
      case IR_KEY4:   VUSB_Keyboard.sendKeyStroke(KEY_PAGEDOWN); break;
      case IR_KEY5:   VUSB_Keyboard.sendKeyStroke(KEY_A); break;
      default:        break;
    }
    PORTB &= ~(1<<LED_PIN);
  }
}
