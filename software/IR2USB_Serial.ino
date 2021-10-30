// ===================================================================================
// Project:   IR2USB - NEC Decoder with serial interface
// Version:   v1.0
// Year:      2021
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// ATtiny receives and decodes IR signals (NEC protocol) and sends address and command
// of the signal via USB/Serial to the PC. Use the serial monitor to display the info.
//
// Note: After plugging the device into the USB port, it takes a few seconds for the
//       serial connection to work properly.
//
// Wiring:
// -------
//                           +-\/-+
//         --- A0 (D5) PB5  1|°   |8  Vcc
// VUSB D- --- A3 (D3) PB3  2|    |7  PB2 (D2) A1 --- 
// VUSB D+ --- A2 (D4) PB4  3|    |6  PB1 (D1) ------ IR Receiver
//                     GND  4|    |5  PB0 (D0) ------ LED
//                           +----+
//
// Compilation Settings:
// ---------------------
// Controller:  ATtiny45 or ATtiny85
// Core:        VUSB-AVR (https://github.com/wagiminator/VUSB-AVR)
// Clockspeed:  16.5 MHz internal
//
// Don't forget to "Burn bootloader"!


// ===================================================================================
// Libraries and Definitions
// ===================================================================================

// Libraries
#include <VUSB_CDC.h>         // part of USB-AVR core

// Pin definitions
#define LED_PIN       0       // pin for LED
#define IR_PIN        1       // pin for IR receiver

// Global variables
uint8_t cmd;                  // received command
uint16_t addr;                // received address

// ===================================================================================
// IR Receiver Implementation (NEC Protocol)
// ===================================================================================

// IR receiver definitions and macros
#define IR_init()       PORTB |= (1<<IR_PIN)  // pullup on IR pin
#define IR_available()  (~PINB & (1<<IR_PIN)) // return true if IR line is low

// IR wait for signal change and measure duration
uint8_t IR_waitChange(uint8_t timeout) {
  uint8_t pinState = PINB & (1<<IR_PIN);      // get current signal state
  uint8_t dur = 0;                            // variable for measuring duration
  while((PINB & (1<<IR_PIN)) == pinState) {   // measure length of signal
    if(dur++ > timeout) return 0;             // exit if timeout
    _delay_us(100);                           // count every 100us
  }
  return dur;                                 // return time in 100us
}

// IR read data byte
uint8_t IR_readByte(void) {
  uint8_t result;
  uint8_t dur;
  for(uint8_t i=8; i; i--) {                  // 8 bits
    result >>= 1;                             // LSB first
    if(IR_waitChange(11) < 3) return 0;       // exit if wrong burst length
    dur = IR_waitChange(21);                  // measure length of pause
    if(dur <  3) return 0;                    // exit if wrong pause length
    if(dur > 11) result |= 0x80;              // bit "0" or "1" depends on pause duration
  }
  return result;                              // return received byte
}

// IR read data according to NEC protocol
uint8_t IR_read(void) {
  if(!IR_available())        return 0;        // exit if no signal
  if(!IR_waitChange(100))    return 0;        // exit if wrong start burst length
  if(IR_waitChange(55) < 35) return 0;        // exit if wrong start pause length

  uint8_t addr1 = IR_readByte();              // get first  address byte
  uint8_t addr2 = IR_readByte();              // get second address byte
  uint8_t cmd1  = IR_readByte();              // get first  command byte
  uint8_t cmd2  = IR_readByte();              // get second command byte

  if(IR_waitChange(11) < 3)  return 0;        // exit if wrong final burst length
  if((cmd1 + cmd2) < 255)    return 0;        // exit if command bytes are not inverse
  cmd = cmd1;                                 // set received command
  if((addr1 + addr2) == 255) addr = addr1;    // check if it's extended NEC-protocol ...
  else addr = ((uint16_t)addr2 << 8) | addr1; // ... and get the correct address
  return 1;                                   // return TRUE
}

// ===================================================================================
// Byte to Hex String Conversion
// ===================================================================================

// Convert nibble into ASCII character
char ascii(uint8_t nibble) {
  return((nibble <= 9) ? (nibble + '0') : (nibble + ('A' - 10)));
}

// Print hex byte via VUSB serial
void printHex(uint8_t val) {
  static char *str = "00";
  str[0] = ascii(val >> 4);                   // high nibble
  str[1] = ascii(val & 0x0F);                 // low nibble
  Serial_VUSB.print(str);                     // print hex value
}

// ===================================================================================
// Main Function
// ===================================================================================

void setup() {
  Serial_VUSB.begin();                        // prepare and start V-USB CDC
  DDRB |= (1<<LED_PIN);                       // set LED pin as output
  IR_init();                                  // init IR receiver
}

void loop() {
  if(IR_available()) {                        // IR signal coming in?
    PORTB |= (1<<LED_PIN);                    // switch on indicator LED
    if(IR_read()) {                           // received a valid signal?
      // Print address and command via serial
      Serial_VUSB.println(F("NEC telegram received:"));
      Serial_VUSB.print(F("Address: 0x"));
      if(addr > 255) printHex(addr >> 8);
      printHex(addr); Serial_VUSB.println();
      Serial_VUSB.print(F("Command: 0x"));
      printHex(cmd); Serial_VUSB.println();
    }
    PORTB &= ~(1<<LED_PIN);                   // switch off indicator LED
  }
  Serial_VUSB.refresh();                      // to keep the USB connection alive
}
