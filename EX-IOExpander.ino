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
* (Nano can use A6/A7 for 18 pins)
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

}

/*
* Function triggered when CommandStation is sending data to this device.
*/
void receiveEvent(int numBytes) {
  // Serial.println(F("receiveEvent triggered"));
  byte buffer[numBytes];
  for (uint8_t byte = 0; byte < numBytes; byte++) {
    buffer[byte] = Wire.read();
    if (numBytes != 1) {
      // Serial.print(F("Multi byte "));
      // Serial.print(byte);
      // Serial.print(F(": "));
      // Serial.println(buffer[byte], HEX);
    }
    // } else if (numBytes == 1 && buffer[byte] != 0x12) {
    //   Serial.print(F("Single byte "));
    //   Serial.print(byte);
    //   Serial.print(F(": "));
    //   Serial.println(buffer[byte], HEX);
    // }
  }
  if (numBytes > 1) {
    switch(buffer[0]) {
      case REG_IODIRA:
        Serial.print(F("REG_IODIRA with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_IODIRB:
        Serial.print(F("REG_IODIRB with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_GPINTENA:
        Serial.print(F("REG_INTENA with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_GPINTENB:
        Serial.print(F("REG_INTENB with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_INTCONA:
        Serial.print(F("REG_INTCONA with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_INTCONB:
        Serial.print(F("REG_INTCONB with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_IOCON:
        Serial.print(F("REG_IOCON with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_GPPUA:
        Serial.print(F("REG_GPPUA with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_GPPUB:
        Serial.print(F("REG_GPPUB with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      case REG_GPIOA:
        if (numBytes == 3) {
          Serial.print(F("REG_GPIOA received, outputState: "));
          Serial.print(buffer[1]);
          Serial.print(F(", outputState>>8: "));
          Serial.println(buffer[2]);
        } else {
          Serial.print(F("REG_GPIOA with "));
          Serial.print(numBytes);
          Serial.println(F(" received, what to do?"));
        }
        break;
      case REG_GPIOB:
        Serial.print(F("REG_GPIOA with "));
        Serial.print(numBytes);
        Serial.println(F(" received, what to do?"));
        break;
      default:
        Serial.println(F("Reached default, no case matched"));
        break;
    }
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.
*/
void requestEvent() {
  // Serial.println(F("requestEvent triggered"));
}