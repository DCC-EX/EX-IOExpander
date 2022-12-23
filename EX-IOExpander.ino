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
#include "SupportedDevices.h"
#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif

#ifdef CPU_TYPE_ERROR
#error Unsupported microcontroller architecture detected, you need to use a type listed in defines.h
#endif

/*
If we haven't got a custom config.h, use the example.
*/
#if __has_include ("myConfig.h")
  #include "myConfig.h"
#else
  #warning myConfig.h not found. Using defaults from myConfig.example.h
  #include "myConfig.example.h"
#endif

/*
* Struct to define the digital pin parameters and keep state.
*/
typedef struct {
  bool direction;       // 0 = output, 1 = input
  bool pullup;          // 0 = no pullup, 1 = pullup (input only)
  bool state;           // stores current state, 0 = LOW, 1 = HIGH
} digitalConfig;

/*
* Struct to define the analogue pin assignment and keep state
*/
typedef struct {
  byte valueLSB;        // Least significant byte of analogue value
  byte valueMSB;        // Most significant byte of analogue value
  bool enable;          // Flag if it's enabled (1) or not (0)
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
/*
* If for some reason the I2C address isn't defined, define our default here.
*/
#ifndef I2C_ADDRESS
#define I2C_ADDRESS 0x65
#endif
uint8_t i2cAddress = I2C_ADDRESS;   // Assign address to a variable for validation and serial input
uint8_t numDigitalPins = NUMBER_OF_DIGITAL_PINS;    // Init with default, will be overridden by config
uint8_t numAnaloguePins = NUMBER_OF_ANALOGUE_PINS;  // Init with default, will be overridden by config
int digitalPinBytes;  // Used for configuring and sending/receiving digital pins
int analoguePinBytes; // Used for enabling/disabling analogue pins
bool setupComplete = false;   // Flag when initial configuration/setup has been received
uint8_t outboundFlag;   // Used to determine what data to send back to the CommandStation
byte analogueOutBuffer[2];  // Array to send requested LSB/MSB of the analogue value to the CommandStation
byte digitalOutBuffer[1];   // Array to send digital value to CommandStation
bool newSerialData = false;   // Flag for new serial data being received
const byte numSerialChars = 10;   // Max number of chars for serial input
char serialInputChars[numSerialChars];  // Char array for serial input
#ifdef DIAG
unsigned long lastPinDisplay = 0;   // Last time in millis we displayed DIAG input states
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
  Serial.print(F("Detected device: "));
  Serial.println(BOARD_TYPE);
#ifdef HAS_EEPROM
  if (getI2CAddress() != 0) {
    i2cAddress = getI2CAddress();
  }
#endif
  if (i2cAddress < 0x08 || i2cAddress > 0x77) {
    Serial.print(F("ERROR: Invalid I2C address configured: 0x"));
    Serial.println(i2cAddress, HEX);
    i2cAddress = 0x65;
  }
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(i2cAddress, HEX);
  Wire.begin(i2cAddress);
  // Need to intialise every pin in INPUT mode (no pull ups) for safe start
  for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS; pin++) {
    pinMode(digitalPinMap[pin], INPUT);
  }
  for (uint8_t pin = 0; pin < NUMBER_OF_ANALOGUE_PINS; pin++) {
    pinMode(analoguePinMap[pin], INPUT);
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
          pinMode(digitalPinMap[dPin], INPUT_PULLUP);
        } else {
          pinMode(digitalPinMap[dPin], INPUT);
        }
        bool currentState = digitalRead(digitalPinMap[dPin]);
        if (digitalPins[dPin].pullup) currentState = !currentState;
        digitalPins[dPin].state = currentState;
      }
    }
    for (uint8_t aPin = 0; aPin < numAnaloguePins; aPin++) {
      if (analoguePins[aPin].enable == 1) {
        uint16_t value = analogRead(analoguePinMap[aPin]);
        analoguePins[aPin].valueLSB = value & 0xFF;
        analoguePins[aPin].valueMSB = value >> 8;
      }
    }
  }
#ifdef DIAG
  displayPins();
#endif
  processSerialInput();
}

/*
* Function triggered when CommandStation is sending data to this device.
*/
void receiveEvent(int numBytes) {
  if (numBytes == 0) {
    return;
  }
  byte buffer[numBytes];
  for (uint8_t byte = 0; byte < numBytes; byte++) {
    buffer[byte] = Wire.read();   // Read all received bytes into our buffer array
  }
  switch(buffer[0]) {
    // Initial configuration start, must be 3 bytes
    case EXIOINIT:
      if (numBytes == 3) {
        numDigitalPins = buffer[1];
        numAnaloguePins = buffer[2];
        if (numDigitalPins + numAnaloguePins == NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS) {
          // Calculate number of bytes required to cover pins
          digitalPinBytes = (numDigitalPins + 7) / 8;
          analoguePinBytes = (numAnaloguePins + 7) / 8;
          for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
            digitalPins[pin].direction = 0;
            digitalPins[pin].pullup = 0;
            digitalPins[pin].state = 0;
          }
          for (uint8_t pin = 0; pin < NUMBER_OF_ANALOGUE_PINS; pin++) {
            analoguePins[pin].enable = 0;
            analoguePins[pin].valueLSB = 0;
            analoguePins[pin].valueMSB = 0;
          }
#ifdef DIAG
          Serial.print(F("Configured pins (digital|analogue): "));
          Serial.print(numDigitalPins);
          Serial.print(F("|"));
          Serial.println(numAnaloguePins);
#endif
          setupComplete = true;
        } else {
          Serial.print(F("ERROR: Invalid pins sent by device driver! (Digital|Analogue): "));
          Serial.print(numDigitalPins);
          Serial.print(F("|"));
          Serial.println(numAnaloguePins);
          setupComplete = false;
        }
        outboundFlag = EXIOINIT;
      } else {
#ifdef DIAG
        Serial.println(F("EXIOINIT received with incorrect data"));
#endif
      }
      break;
    // Flag to set digital pin pullups, 0 disabled, 1 enabled
    case EXIODPUP:
      if (numBytes == 3) {
        uint8_t pin = buffer[1];
        digitalPins[pin].direction = 1;   // Must be an input if we got a pullup config
        digitalPins[pin].pullup = buffer[2];
        if (digitalPins[pin].pullup == 1) {
          pinMode(digitalPinMap[pin], INPUT_PULLUP);
        } else {
          pinMode(digitalPinMap[pin], INPUT);
        }
        digitalPins[pin].state = digitalRead(digitalPinMap[pin]);
        // }
      } else {
#ifdef DIAG
      Serial.println(F("EXIODPUP received with incorrect number of bytes"));
#endif
      }
      break;
    case EXIORDAN:
      if (numBytes == 2) {
        outboundFlag = EXIORDAN;
        uint8_t aPin = buffer[1] - NUMBER_OF_DIGITAL_PINS;
        if (analoguePins[aPin].enable == 0) {
          analoguePins[aPin].enable = 1;
          uint16_t value = analogRead(analoguePinMap[aPin]);
          analoguePins[aPin].valueLSB = value & 0xFF;
          analoguePins[aPin].valueMSB = value >> 8;
        }
        analogueOutBuffer[0] = analoguePins[aPin].valueLSB;
        analogueOutBuffer[1] = analoguePins[aPin].valueMSB;
      }
      break;
    case EXIOWRD:
      if (numBytes == 3) {
        uint8_t dPin = buffer[1];
        bool state = buffer[2];
        digitalPins[dPin].direction = 0;
        digitalPins[dPin].state = state;
        digitalWrite(digitalPinMap[dPin], state);
      }
      break;
    case EXIORDD:
      if (numBytes == 3) {
        uint8_t dPin = buffer[1];
        outboundFlag = EXIORDD;
        digitalOutBuffer[0] = digitalPins[dPin].state;
      }
      break;
    default:
      break;
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.
*/
void requestEvent() {
  switch(outboundFlag) {
    case EXIOINIT:
      if (setupComplete) {
        Wire.write(EXIORDY);
      } else {
        Wire.write(0);
      }
      break;
    case EXIORDAN:
      Wire.write(analogueOutBuffer, 2);
      break;
    case EXIORDD:
      Wire.write(digitalOutBuffer, 1);
      break;
    default:
      break;
  }
}

/*
* Function to display pin configuration and states when DIAG enabled
*/
#ifdef DIAG
void displayPins() {
  if (millis() > lastPinDisplay + DIAG_CONFIG_DELAY) {
    lastPinDisplay = millis();
    Serial.println(F("Digital Pin|Direction|Pullup|State:"));
    for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
      Serial.print(digitalPinMap[pin]);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].direction);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].pullup);
      Serial.print(F("|"));
      if (pin == NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS - 1 || (pin % 15 == 0 && pin > 0)) {
        Serial.println(digitalPins[pin].state);
      } else {
        Serial.print(digitalPins[pin].state);
        Serial.print(F(","));
      }
    }
    Serial.println(F("Analogue Pin|Enable|Value|LSB|MSB:"));
    for (uint8_t pin = 0; pin < NUMBER_OF_ANALOGUE_PINS; pin++) {
      Serial.print(analoguePinMap[pin]);
      Serial.print(F("|"));
      Serial.print(analoguePins[pin].enable);
      Serial.print(F("|"));
      Serial.print((analoguePins[pin].valueMSB << 8) + analoguePins[pin].valueLSB);
      Serial.print(F("|"));
      Serial.print(analoguePins[pin].valueLSB);
      Serial.print(F("|"));
      if (pin == NUMBER_OF_ANALOGUE_PINS - 1) {
        Serial.println(analoguePins[pin].valueMSB);
      } else {
        Serial.print(analoguePins[pin].valueMSB);
        Serial.print(F(","));
      }
    }
  }
}
#endif

/*
* Function to read and process serial input for I2C address config
*/
void processSerialInput() {
  static bool serialInProgress = false;
  static byte serialIndex = 0;
  char startMarker = '<';
  char endMarker = '>';
  char serialChar;
  while (Serial.available() > 0 && newSerialData == false) {
    serialChar = Serial.read();
    if (serialInProgress == true) {
      if (serialChar != endMarker) {
        serialInputChars[serialIndex] = serialChar;
        serialIndex++;
        if (serialIndex >= numSerialChars) {
          serialIndex = numSerialChars - 1;
        }
      } else {
        serialInputChars[serialIndex] = '\0';
        serialInProgress = false;
        serialIndex = 0;
        newSerialData = true;
      }
    } else if (serialChar == startMarker) {
      serialInProgress = true;
    }
  }
  if (newSerialData == true) {
    newSerialData = false;
    char * strtokIndex;
    strtokIndex = strtok(serialInputChars," ");
    char activity = strtokIndex[0];    // activity is our first parameter
    strtokIndex = strtok(NULL," ");       // space is null, separator
    unsigned long newAddress = strtol(strtokIndex, NULL, 16); // last parameter is the address in hex
#ifdef DIAG
    Serial.print(F("Perform activity "));
    Serial.print(activity);
    Serial.print(F(" for I2C address 0x"));
    Serial.println(newAddress, HEX);
#endif
    switch (activity) {
      case 'W':
        if (newAddress > 0x07 && newAddress < 0x78) {
#ifdef HAS_EEPROM
          Serial.print(F("Saving address 0x"));
          Serial.print(newAddress, HEX);
          Serial.println(F(" to EEPROM, reboot to activate"));
          writeI2CAddress(newAddress);
#else
          Serial.println(F("No EEPROM support, ignoring"));
#endif
        } else {
          Serial.println(F("Invalid I2C address, must be between 0x08 and 0x77"));
        }
        break;
      case 'E':
        eraseEEPROM();
        Serial.println(F("Erased EEPROM, reboot to revert to default"));
        break;
      case 'R':
        if (getI2CAddress() == 0) {
          Serial.println(F("I2C address not stored in EEPROM"));
        } else {
          Serial.print(F("I2C address stored in EEPROM is 0x"));
          Serial.println(getI2CAddress(), HEX);
        }
        break;
      default:
        break;
    }
  }
}

// EEPROM functions here, only for uCs with EEPROM support
#ifdef HAS_EEPROM
/*
* Function to read I2C address from EEPROM
* Look for "EXIO" and the version EEPROM_VERSION at 0 to 5, address at 6
*/
uint8_t getI2CAddress() {
  char data[5];
  char eepromData[5] = {'E', 'X', 'I', 'O', EEPROM_VERSION};
  uint8_t eepromAddress;
  bool addressSet = true;
  for (uint8_t i = 0; i < 5; i ++) {
    data[i] = EEPROM.read(i);
    if (data[i] != eepromData[i]) {
      addressSet = false;
      break;
    }
  }
  if (addressSet) {
    eepromAddress = EEPROM.read(6);
#ifdef DIAG
      Serial.print(F("I2C address defined in EEPROM: 0x"));
      Serial.println(eepromAddress, HEX);
#endif
    return eepromAddress;
  } else {
#ifdef DIAG
    Serial.println(F("I2C address not defined in EEPROM"));
#endif
    return 0;
  }
}

/*
* Function to store I2C address in EEPROM
*/
void writeI2CAddress(int16_t eepromAddress) {
  char eepromData[5] = {'E', 'X', 'I', 'O', EEPROM_VERSION};
  for (uint8_t i = 0; i < 5; i++) {
    EEPROM.write(i, eepromData[i]);
  }
  EEPROM.write(6, eepromAddress);
}

/*
* Function to erase EEPROM contents
*/
void eraseEEPROM() {
  for (uint8_t i = 0; i < 7; i++) {
    EEPROM.write(i, 0);
  }
}
#endif