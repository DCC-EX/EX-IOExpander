/*
 *  © 2021, xxxxxxxxxx. All rights reserved.
 *  
 *  This file is part of EX-IOExpander.
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
* EX-IOExpander is designed to emulate the digital I/O capability of an MCP23017 I/O expander.
*
* All 12 digital I/O pins on Uno/Nano footprints are available as digital inputs or outputs (2 - 13).
* Analogue I/O pins A0 - A3 are also available as digital inputs or outputs for a total of 16 pins.
*/

#include <Arduino.h>

/*
If we haven't got a custom config.h, use the example.
*/
#if __has_include ("config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

/*
* Struct to define the port parameters.
*/
typedef struct {
  bool mode;            // 0 = output, 1 = input
  bool pullup;          // 0 = no pullup, 1 = pullup (input only)
  bool state;           // stores current state, 0 = LOW, 1 = HIGH
} gpioConfig;

/*
* Create GPIO array using struct.
*/
gpioConfig portStates[NUMBER_OF_PINS];

/*
* If for some reason the I2C address isn't defined, define our default here.
*/
#ifndef I2C_ADDRESS
#define I2C_ADDRESS 0x90
#endif

/*
* Include required files and libraries.
*/
#include "version.h"
#include <Wire.h>

/*
* Main setup function here.
*/
void setup() {
  Serial.begin(115200);
  Serial.print(F("DCC-EX EX-IOExpander version "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

/*
* Main loop here.
*/
void loop() {
/*  To read inputs, this should probably look something like:
  for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
    if (portStates[port].mode == 1) {
      if (portStates[port].pullup == 0) {
        pinMode(pinMap[port], INPUT);
      } else {
        pinMode(pinMap[port], INPUT_PULLUP);
      }
      portStates[port].state = digitalRead(pinMap[port]);
    }
  }
*/
}

/*
* Function triggered when CommandStation is sending data to this device.
*
* This function probably needs to:
* - Configure ports as input or output
* - For input ports, configure pullup flag
* - For output ports, perform the digitalWrite
* 
* Step 1: how to know which port is being configured or written to?
* For port registers, MSB is in byte two, LSB in byte 1
*/
void receiveEvent(int numBytes) {
  // Serial.println(F("receiveEvent triggered"));
  byte buffer[numBytes];
  uint16_t portBits;
  for (uint8_t byte = 0; byte < numBytes; byte++) {
    buffer[byte] = Wire.read();
  }
  if (numBytes > 1) {
    if (numBytes == 3) {
      portBits = (buffer[2] << 8) + buffer[1];
    }
    switch(buffer[0]) {
      case REG_IODIRA:
        Serial.println(F("REG_IODIRA:"));
        break;
      case REG_GPINTENA:
        Serial.println(F("REG_INTENA:"));
        break;
      case REG_INTCONA:
        // Serial.println(F("REG_INTCONA:"));
        // Not enabling interrupts at the moment, disregard this
        break;
      case REG_IOCON:
        // Serial.println(F("REG_IOCON:"));
        // We don't need to do anything with this, only relevant for actual MCP23017
        break;
      case REG_GPPUA:
        // Serial.println(F("REG_GPPUA:"));
        for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
          bool pullup = portBits >> port & 1;
          portStates[port].pullup = pullup;
          Serial.print(F("Port "));
          Serial.print(pinMap[port]);
          Serial.print(F(" pullup set to "));
          Serial.println(portStates[port].pullup);
        }
        break;
      case REG_GPIOA:
        // Serial.println(F("REG_GPIOA:"));
        for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
          if (portStates[port].mode == 0) {
            bool portState = portBits >> port & 1;
            pinMode(pinMap[port], OUTPUT);
            Serial.print(F("Port "));
            Serial.print(pinMap[port]);
            Serial.print(F(" in output mode, set to "));
            Serial.println(portState);
            digitalWrite(pinMap[port], portState);
          }
        }
        break;
      default:
        Serial.println(F("Reached default, no case matched"));
        break;
    }
    // for (uint8_t byte = 1; byte < numBytes; byte++) {
    //   Serial.print(F("Byte "));
    //   Serial.print(byte);
    //   Serial.print(F(" contains: 0x"));
    //   Serial.println(buffer[byte], HEX);
    // }
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.
* This function probably needs to:
* - Combine the appropriate input ports from portStates into one (or more?) bytes
* - Perform a Wire.write() of those bytes
*/
void requestEvent() {
  // Serial.println(F("requestEvent triggered"));
}