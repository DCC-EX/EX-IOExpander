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
* EX-IOExpander is a native DCC-EX I/O expander utilising a microcontroller's digital and analogue I/O pins
* to expand the I/O capability of an EX-CommandStation.
*
* All digital I/O pins are available as digital inputs or outputs.
* Analogue I/O pins are available as digital inputs or outputs or analogue inputs (depending on architecture).
*/

#include <Arduino.h>
#include "defines.h"
#include "DefaultPinMaps.h"

#ifdef CPU_TYPE_ERROR
#error Unsupported microcontroller architecture detected, you need to use a type listed in defines.h
#endif

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
* Struct to define the digital pin parameters and keep state.
*/
typedef struct {
  uint8_t pin;          // Pin assignment
  bool direction;       // 0 = output, 1 = input
  bool pullup;          // 0 = no pullup, 1 = pullup (input only)
  bool state;           // stores current state, 0 = LOW, 1 = HIGH
} digitalConfig;

/*
* Struct to define the analogue pin assignment and keep state
*/
typedef struct {
  uint8_t pin;          // Pin assignment
  byte valueLSB;        // Least significant byte of analogue value
  byte valueMSB;        // Most significant byte of analogue value
} analogueConfig;

/*
* Create digitalConfig array using struct.
* Size of the array needs to be able to include analogue pins as well.
*/
digitalConfig digitalPins[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS];

/*
* Create array for analogue pin assignments.
*/
analogueConfig analoguePins[NUMBER_OF_ANALOGUE_PINS];

/*
* Global variables here
*/
uint8_t numDigitalPins = NUMBER_OF_DIGITAL_PINS;    // Init with default, will be overridden by config
uint8_t numAnaloguePins = NUMBER_OF_ANALOGUE_PINS;  // Init with default, will be overridden by config
uint8_t digitalPinBytes;  // Used for efficiency to flag how many bytes are needed for digital pins
bool setupComplete = false;   // Flag when initial configuration/setup has been received
#ifdef DIAG
unsigned long lastPinDisplay = 0;   // Last time in millis we displayed DIAG input states
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
  // Need to intialise every pin in INPUT mode (no pull ups) for safe start
  for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
    pinMode(defaultPinMap[pin], INPUT);
  }
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

/*
* Main loop here, just processes our inputs and updates the writeBuffer.
*/
void loop() {
  if (setupComplete) {
    for (uint8_t dPin = 0; dPin < numDigitalPins; dPin++) {
      if (digitalPins[dPin].direction == 1) {
        if (digitalPins[dPin].pullup == 1) {
          pinMode(digitalPins[dPin].pin, INPUT_PULLUP);
        } else {
          pinMode(digitalPins[dPin].pin, INPUT);
        }
        bool currentState = digitalRead(digitalPins[dPin].pin);
        digitalPins[dPin].state = currentState;
      }
    }
    for (uint8_t aPin = 0; aPin < numAnaloguePins; aPin++) {
      uint16_t value = analogRead(analoguePins[aPin].pin);
      analoguePins[aPin].valueLSB = value & 0xFF;
      analoguePins[aPin].valueMSB = value >> 8;
    }
  }
  #ifdef DIAG
  displayPins();
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
  switch(buffer[0]) {
    // Initial configuration start, must be 3 bytes
    case REG_EXIOINIT:
      if (numBytes == 3) {
        numDigitalPins = buffer[1];
        numAnaloguePins = buffer[2];
        // Calculate number of bytes required to cover digital pins
        digitalPinBytes = (numDigitalPins + 7) / 8;
      } else {
#ifdef DIAG
        Serial.println(F("REG_EXIOINIT received with incorrect data"));
#endif
      }
      break;
    // Receive digital pin assignments, should be one byte per pin + flag
    case REG_EXIODPIN:
      if (numBytes == numDigitalPins + 1) {
        for(uint8_t pin = 0; pin < numDigitalPins; pin++) {
          digitalPins[pin].pin = buffer[pin + 1];
        }
    } else {
#ifdef DIAG
        Serial.println(F("REG_EXIODPIN received with incorrect number of pins"));
#endif
      }
      break;
    // Receive analogue pin assignments, should be one byte per pin + flag
    case REG_EXIOAPIN:
      if (numBytes == numAnaloguePins + 1) {
        for(uint8_t pin = 0; pin < numAnaloguePins; pin++) {
          analoguePins[pin].pin = buffer[pin + 1];
        }
    } else {
#ifdef DIAG
        Serial.println(F("REG_EXIOAPIN received with incorrect number of pins"));
#endif
      }
      break;
    // Received flag that setup should be complete
    case REG_EXIORDY:
      setupComplete = true;
      break;
    // Flag to set digital pin direction, 0 output, 1 input
    case REG_EXIODDIR:
      if (numBytes == digitalPinBytes + 1) {
        for (uint8_t pin = 0; pin < numDigitalPins; pin++) {
          digitalPins[pin].direction = buffer[pin];
        }
      } else {
#ifdef DIAG
      Serial.println(F("REG_EXIODDIR received with incorrect number of bytes"));
#endif
      }
      break;
    // Flag to set digital pin pullups, 0 disabled, 1 enabled
    case REG_EXIODPUP:
      if (numBytes == digitalPinBytes + 1) {
        for (uint8_t pin = 0; pin < numDigitalPins; pin++) {
          digitalPins[pin].pullup = buffer[pin];
        }
      } else {
#ifdef DIAG
      Serial.println(F("REG_EXIODPUP received with incorrect number of bytes"));
#endif
      }
      break;
    default:
      Serial.println(F("Reached default, no case matched"));
      break;
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.
*/
void requestEvent() {

}

/*
* Function to display pin configuratin and states when DIAG enabled
*/
#ifdef DIAG
void displayPins() {
  if (millis() > lastPinDisplay + DIAG_CONFIG_DELAY) {
    lastPinDisplay = millis();
    Serial.println(F("Digital Pin|Direction|Pullup|State:"));
    for (uint8_t pin = 0; pin < numDigitalPins; pin++) {
      Serial.print(digitalPins[pin].pin);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].direction);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].pullup);
      Serial.print(F("|"));
      if (pin == numDigitalPins - 1 || (pin % 16 == 0 && pin > 0)) {
        Serial.println(digitalPins[pin].state);
      } else {
        Serial.print(digitalPins[pin].state);
        Serial.print(F(","));
      }
    }
    Serial.println(F("Analogue Pin|LSB|MSB:"));
    for (uint8_t pin = 0; pin < numAnaloguePins; pin++) {
      Serial.print(analoguePins[pin].pin);
      Serial.print(F("|"));
      Serial.print(analoguePins[pin].valueLSB);
      Serial.print(F("|"));
      if (pin == numAnaloguePins - 1) {
        Serial.println(analoguePins[pin].valueMSB);
      } else {
        Serial.print(analoguePins[pin].valueMSB);
        Serial.print(F(","));
      }
    }
  }
}
#endif