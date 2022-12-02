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

Expectation that the bytes coming in will specify which ports are being set/reset.

Function to set/reset should be something like:

pinMode(port, OUTPUT);
digitalWrite(port, HIGH/LOW);

How to get port list from bytes received?

Expectation this function also needs to perform the initial device setup for which
ports are inputs vs. outputs?

When MCP23017 device is created, there is nothing to define which ports are input vs. output.
How to deal with this so input ports are polled?
*/
void receiveEvent(int numBytes) {
  // Serial.println(F("receiveEvent triggered"));
  byte buffer[numBytes];
  for (uint8_t byte = 0; byte < numBytes; byte++) {
    buffer[byte] = Wire.read();
    if (numBytes != 1) {
      Serial.print(F("Multi byte "));
      Serial.print(byte);
      Serial.print(F(": "));
      Serial.println(buffer[byte], HEX);
    } else if (numBytes == 1 && buffer[byte] != 0x12) {
      Serial.print(F("Single byte "));
      Serial.print(byte);
      Serial.print(F(": "));
      Serial.println(buffer[byte], HEX);
    }
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.

Expectation that the input ports are known already when this device initialises.

Is this set during initilisation of registers etc. when device is first connected to?

Will need to have some sort of global buffer setup to be updated during the main loop().

This function will need to convert that buffer into bytes to be sent via Wire.write().
*/
void requestEvent() {
  // Serial.println(F("requestEvent triggered"));
}


/*
18 to start with (continuous, only byte).

Only single byte seems to be 18.

When CS initialises, 3 events received:
Byte 0: 10
Byte 1: 68
Byte 0: 8
Byte 1: 0
Byte 2: 0
Byte 0: 12
Byte 1: 0
Byte 2: 0

<Z 800 800 0>:
Multi byte 0: 0
Multi byte 1: 254
Multi byte 2: 255
Multi byte 0: 4
Multi byte 1: 0
Multi byte 2: 0

<Z 800 0>:
Multi byte 0: 18
Multi byte 1: 34
Multi byte 2: 243 <- this seems to change 243/247

<Z 800 1>:
Multi byte 0: 18
Multi byte 1: 35
Multi byte 2: 243 <- this seems to change 243/247

<Z 815 815 0>:
Multi byte 0: 0
Multi byte 1: 254
Multi byte 2: 127
Multi byte 0: 4
Multi byte 1: 0
Multi byte 2: 0
Multi byte 0: 18
Multi byte 1: 35
Multi byte 2: 115

<Z 815 0>
Multi byte 0: 18
Multi byte 1: 35
Multi byte 2: 115

<Z 815 1>
Multi byte 0: 18
Multi byte 1: 35
Multi byte 2: 243

<Z 801 801 0>
Multi byte 0: 0
Multi byte 1: 252
Multi byte 2: 127
Multi byte 0: 4
Multi byte 1: 0
Multi byte 2: 0
Multi byte 0: 18
Multi byte 1: 33
Multi byte 2: 243

<Z 801 0>
Multi byte 0: 18
Multi byte 1: 33
Multi byte 2: 243

<Z 801 1>
Multi byte 0: 18
Multi byte 1: 35
Multi byte 2: 243

<S 802 802 1>
Multi byte 0: 0
Multi byte 1: 252
Multi byte 2: 127
Multi byte 0: 8
Multi byte 1: 0
Multi byte 2: 0
Multi byte 0: 12
Multi byte 1: 7
Multi byte 2: 128

<S 803 803 0>
Multi byte 0: 0
Multi byte 1: 252
Multi byte 2: 127
Multi byte 0: 8
Multi byte 1: 0
Multi byte 2: 0
Multi byte 0: 12
Multi byte 1: 7
Multi byte 2: 128
*/