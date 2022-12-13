/*
 *  © 2022, Peter Cole. All rights reserved.
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
#include "defines.h"

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
  bool direction;       // 0 = output, 1 = input
  bool pullup;          // 0 = no pullup, 1 = pullup (input only)
  bool state;           // stores current state, 0 = LOW, 1 = HIGH
} gpioConfig;

/*
* Create GPIO array using struct.
*/
gpioConfig portStates[NUMBER_OF_PINS];

/*
* Global variables here
*/
byte gpioA = 0x00;   // Bytes to store banks A and B states to send to the CommandStation
byte gpioB = 0x00;
#ifdef DIAG
unsigned long lastInputDisplay = 0;   // Last time in millis we displayed DIAG input states
#endif

/*
* If for some reason the I2C address isn't defined, define our default here.
*/
#ifndef I2C_ADDRESS
#define I2C_ADDRESS 0x65
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
  // Start with each port in a known state, safest is input
  for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
    portStates[port].direction = 1;
    portStates[port].pullup = 0;
    portStates[port].state = 0;
    pinMode(pinMap[port], INPUT);
  }
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

/*
* Main loop here, just processes our inputs and updates the writeBuffer.
*/
void loop() {
  for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
    if (portStates[port].direction == 1) {
      if (portStates[port].pullup == 0) {
        pinMode(pinMap[port], INPUT);
      } else {
        pinMode(pinMap[port], INPUT_PULLUP);
      }
      bool currentState = digitalRead(pinMap[port]);
      portStates[port].state = currentState;
    }
    if (portStates[port].state == 0) {
      if (port < 8) {
        bitClear(gpioA, port);
      } else {
        bitClear(gpioB, port);
      }
    } else {
      if (port < 8) {
        bitSet(gpioA, port);
      } else {
        bitSet(gpioB, port);
      }
    }
  }
#ifdef DIAG
  displayConfig();
#endif
}

/*
* Function triggered when CommandStation is sending data to this device.
*/
void receiveEvent(int numBytes) {
  byte buffer[numBytes];
  uint16_t portBits;
  for (uint8_t byte = 0; byte < numBytes; byte++) {
    buffer[byte] = Wire.read();   // Read all received bytes into our buffer array
  }
  if (numBytes > 1) {
    if (numBytes == 3) {
      portBits = (buffer[2] << 8) + buffer[1];  // If 3 bytes, we know we got port info, bit shift them
    }
    switch(buffer[0]) {
      case REG_IODIRA:
        // Register to set port direction, 0 = output, 1 = input
#ifdef DIAG
        Serial.print(F("REG_IODIRA (port|dir): "));
#endif
        for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
          bool direction = portBits >> port & 1;
          portStates[port].direction = direction;
#ifdef DIAG
          Serial.print(pinMap[port]);
          Serial.print(F("|"));
          if (port == NUMBER_OF_PINS - 1) {
            Serial.println(portStates[port].direction);
          } else {
            Serial.print(portStates[port].direction);
            Serial.print(F(","));
          }
#endif
        }
        break;
      case REG_GPINTENA:
        // Register to enable interrupts per port
        // Not enabling interrupts at the moment, disregard this
        break;
      case REG_INTCONA:
        // Register to set interrupt detection method per port
        // Not enabling interrupts at the moment, disregard this
        break;
      case REG_IOCON:
        // Register to configure the I/O expander
        // We don't need to do anything with this, only relevant for actual MCP23017
        break;
      case REG_GPPUA:
        // Register to set pullups per port
#ifdef DIAG
        Serial.print(F("REG_GPPUA (port|pullup): "));
#endif
        for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
          bool pullup = portBits >> port & 1;
          portStates[port].pullup = pullup;
#ifdef DIAG
          Serial.print(pinMap[port]);
          Serial.print(F("|"));
          if (port == NUMBER_OF_PINS - 1) {
            Serial.println(portStates[port].pullup);
          } else {
            Serial.print(portStates[port].pullup);
            Serial.print(F(","));
          }
#endif
        }
        break;
      case REG_GPIOA:
        // Register to reflect the logic level of each port
        // We only deal with outputs here
#ifdef DIAG
        Serial.print(F("REG_GPIO (port|state): "));
#endif
        for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
          if (portStates[port].direction == 0) {
            bool portState = portBits >> port & 1;
            pinMode(pinMap[port], OUTPUT);
            portStates[port].state = portState;
            digitalWrite(pinMap[port], portState);
#ifdef DIAG
            Serial.print(pinMap[port]);
            Serial.print(F("|"));
            Serial.print(portState);
            Serial.print(F(","));
#endif
          }
        }
#ifdef DIAG
        Serial.println(F(""));
#endif
        break;
      default:
        Serial.println(F("Reached default, no case matched"));
        break;
    }
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.
* Simply sends our current GPIO states back to the CommandStation.
*/
void requestEvent() {
  Wire.write(gpioA);
  Wire.write(gpioB);
}

/*
* Function to display current input port states when DIAG enabled
*/
#ifdef DIAG
void displayConfig() {
  if (millis() > lastInputDisplay + DIAG_CONFIG_DELAY) {
    lastInputDisplay = millis();
    Serial.print(F("Pin|D|P|S: "));
    for (uint8_t port = 0; port < NUMBER_OF_PINS; port++) {
      Serial.print(pinMap[port]);
      Serial.print(F("|"));
      Serial.print(portStates[port].direction);
      Serial.print(F("|"));
      Serial.print(portStates[port].pullup);
      Serial.print(F("|"));
      if (port == NUMBER_OF_PINS - 1) {
        Serial.println(portStates[port].state);
      } else {
        Serial.print(portStates[port].state);
        Serial.print(F(","));
      }
    }
  }
}
#endif