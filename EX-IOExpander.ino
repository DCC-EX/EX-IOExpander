/*
 *  © 2022, Peter Cole. All rights reserved.
 *  © 2023, Peter Cole. All rights reserved.
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

/*
Struct to define the capability of each physical pin
*/
typedef struct {
  uint8_t physicalPin;
  uint8_t capability;
} pinDefinition;

/*
* Include required files and libraries.
*/
#include <Arduino.h>
#include "defines.h"
#include "SupportedDevices.h"
#include "version.h"
#include <Wire.h>
#undef USB_SERIAL           // Teensy has this defined by default (in case we ever support Teensy)
#define USB_SERIAL Serial   // Standard serial port most of the time!
#if defined(ARDUINO_ARCH_AVR)
#include <avr/wdt.h>
#endif
#if defined(ARDUINO_ARCH_SAMD)
#undef USB_SERIAL
#define USB_SERIAL SerialUSB  // Most SAMD21 clones use native USB on the SAMD21G18 variants
#endif
#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif

#ifdef CPU_TYPE_ERROR
#error Unsupported microcontroller architecture detected, you need to use a type listed in SupportedDevices.h
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
Define the structure of the pin config and array to hold them
*/
typedef struct {
  uint8_t mode;             // 1 = digital, 2 = analogue, 3 = PWM
  bool direction;           // 0 = output, 1 = input
  bool pullup;              // 0 = no pullup, 1 = pullup (input only)
  bool enable;              // 0 = disabled (default), 1 = enabled
  uint8_t analogueLSBByte;  // Stores the byte number of the LSB byte in analoguePinStates
} pinConfig;

pinConfig exioPins[TOTAL_PINS];

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
// uint8_t numDigitalPins = NUMBER_OF_DIGITAL_PINS;    // Init with default, will be overridden by config
uint8_t numPins = TOTAL_PINS;
uint8_t numAnaloguePins = 0;  // Init with default, will be overridden by config
uint8_t numDigitalPins = 0;
uint8_t numPWMPins = 0;  // Number of PWM capable pins
int digitalPinBytes = 0;  // Used for configuring and sending/receiving digital pins
int analoguePinBytes = 0; // Used for sending analogue 16 bit values
bool setupComplete = false;   // Flag when initial configuration/setup has been received
uint8_t outboundFlag;   // Used to determine what data to send back to the CommandStation
// byte analogueOutBuffer[2];  // Array to send requested LSB/MSB of the analogue value to the CommandStation
// byte digitalOutBuffer[1];   // Array to send digital value to CommandStation
bool newSerialData = false;   // Flag for new serial data being received
const byte numSerialChars = 10;   // Max number of chars for serial input
char serialInputChars[numSerialChars];  // Char array for serial input
char * version;   // Pointer for getting version
uint8_t versionBuffer[3];   // Array to hold version info to send to device driver
unsigned long lastPinDisplay = 0;   // Last time in millis we displayed DIAG input states
bool diag = false;    // Enable/disable diag outputs
unsigned long displayDelay = DIAG_CONFIG_DELAY * 1000;    // Delay in ms between diag display updates
bool analogueTesting = false;   // Flag that analogue input testing is enabled/disabled
bool inputTesting = false;    // Flag that digital input testing without pullups is enabled/disabled
bool outputTesting = false;   // Flag that digital output testing is enabled/disabled
bool pullupTesting = false;   // Flag that digital input testing with pullups is enabled/disabled
unsigned long lastOutputTest = 0;   // Last time in millis we swapped output test state
bool outputTestState = LOW;   // Flag to set outputs high or low for testing
// byte digitalPinStates[(NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS) / 8];
byte* digitalPinStates;  // Store digital pin states to send to device driver
byte* analoguePinStates;  // Store analogue values to send to device driver
byte commandBuffer[3];    // Command buffer to interact with device driver
uint8_t* analoguePinMap;  // Map which analogue pin's value is in which byte
uint16_t firstVpin = 0;

/*
* Main setup function here.
*/
void setup() {
  Serial.begin(115200);
  USB_SERIAL.print(F("DCC-EX EX-IOExpander v"));
  USB_SERIAL.println(VERSION);
  USB_SERIAL.print(F("Detected device: "));
  USB_SERIAL.println(BOARD_TYPE);
  if (getI2CAddress() != 0) {
    i2cAddress = getI2CAddress();
  }
  if (i2cAddress < 0x08 || i2cAddress > 0x77) {
    USB_SERIAL.print(F("ERROR: Invalid I2C address configured: 0x"));
    USB_SERIAL.print(i2cAddress, HEX);
    USB_SERIAL.println(F(", using myConfig.h instead"));
    i2cAddress = I2C_ADDRESS;
  }
  USB_SERIAL.print(F("Available at I2C address 0x"));
  USB_SERIAL.println(i2cAddress, HEX);
  setVersion();
  setupPinDetails();
#ifdef DIAG
  diag = true;
#endif
  Wire.begin(i2cAddress);
// If desired and pins defined, disable I2C pullups
#if defined(DISABLE_I2C_PULLUPS) && defined(I2C_SDA) && defined(I2C_SCL)
  USB_SERIAL.print(F("Disabling I2C pullups on pins SDA|SCL: "));
  USB_SERIAL.print(I2C_SDA);
  USB_SERIAL.print(F("|"));
  USB_SERIAL.println(I2C_SCL);
  digitalWrite(I2C_SDA, LOW);
  digitalWrite(I2C_SCL, LOW);
#endif
// Need to intialise every pin in INPUT mode (no pull ups) for safe start
  initialisePins();
  USB_SERIAL.println(F("Initialised all pins as input only"));
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
#if (TEST_MODE == ANALOGUE_TEST)
  testAnalogue(true);
#elif (TEST_MODE == INPUT_TEST)
  testInput(true);
#elif (TEST_MODE == OUTPUT_TEST)
  testOutput(true);
#elif (TEST_MODE == PULLUP_TEST)
  testPullup(true);
#endif
  uint8_t analoguePin = 0;
  for (uint8_t pin = 0; pin < numPins; pin++) {
    if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
      exioPins[pin].analogueLSBByte = analoguePin * 2;
      analoguePinMap[analoguePin] = pin;
      analoguePin++;
    }            
  }
}

/*
* Main loop here, just processes our inputs and updates the writeBuffer.
*/
void loop() {
  if (setupComplete) {
    for (uint8_t pin = 0; pin < numPins; pin++) {
      uint8_t pinByte = pin / 8;
      uint8_t pinBit = pin - pinByte * 8;
      if (exioPins[pin].enable && exioPins[pin].direction) {
        switch(exioPins[pin].mode) {
          case MODE_DIGITAL: {
            bool pullup = exioPins[pin].pullup;
            if (pullup) {
              pinMode(pinMap[pin].physicalPin, INPUT_PULLUP);
            } else {
              pinMode(pinMap[pin].physicalPin, INPUT);
            }
            bool currentState = digitalRead(pinMap[pin].physicalPin);
            if (pullup) currentState = !currentState;
            if (currentState) {
              bitSet(digitalPinStates[pinByte], pinBit);
            } else {
              bitClear(digitalPinStates[pinByte], pinBit);
            }
            break;
          }
          case MODE_ANALOGUE: {
            uint8_t pinLSBByte = exioPins[pin].analogueLSBByte;
            uint8_t pinMSBByte = pinLSBByte + 1;
            pinMode(pinMap[pin].physicalPin, INPUT);
            uint16_t value = analogRead(pinMap[pin].physicalPin);
            analoguePinStates[pinLSBByte] = value & 0xFF;
            analoguePinStates[pinMSBByte] = value >> 8;
            break;
          }
          default:
            break;
        }
      }
    }
    if (outputTesting) {
      if (millis() - lastOutputTest > 1000) {
        outputTestState = !outputTestState;
        lastOutputTest = millis();
        for (uint8_t pin = 0; pin < numPins; pin++) {
          uint8_t pinByte = pin / 8;
          uint8_t pinBit = pin - pinByte * 8;
          if (bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
            pinMode(pinMap[pin].physicalPin, OUTPUT);
            digitalWrite(pinMap[pin].physicalPin, outputTestState);
            if (outputTestState) {
              bitSet(digitalPinStates[pinByte], pinBit);
            } else {
              bitClear(digitalPinStates[pinByte], pinBit);
            }
          }
        }
      }
    }
  }
  if (diag) {
    displayPins();
  }
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
    // Initial configuration start, must be 2 bytes
    case EXIOINIT:
      if (numBytes == 4) {
        initialisePins();
        uint8_t numReceivedPins = buffer[1];
        firstVpin = (buffer[3] << 8) + buffer[2];
        if (numReceivedPins == numPins) {
          USB_SERIAL.print(F("Received correct pin count: "));
          USB_SERIAL.print(numReceivedPins);
          USB_SERIAL.print(F(", starting at Vpin: "));
          USB_SERIAL.println(firstVpin);
          displayVpinMap();
          setupComplete = true;
        } else {
          USB_SERIAL.print(F("ERROR: Invalid pin count sent by device driver!: "));
          USB_SERIAL.println(numReceivedPins);
          setupComplete = false;
        }
        outboundFlag = EXIOINIT;
      } else {
        if (diag) {
          USB_SERIAL.println(F("EXIOINIT received with incorrect data"));
        }
      }
      break;
    case EXIOINITA:
      if (numBytes == 1) {
        outboundFlag = EXIOINITA;
      } else {
        if (diag) {
          USB_SERIAL.println(F("EXIOINITA received with incorrect data"));
        }
      }
      break;
    // Flag to set digital pin pullups, 0 disabled, 1 enabled
    case EXIODPUP:
      if (numBytes == 3) {
        uint8_t pin = buffer[1];
        bool pullup = buffer[2];
        if (exioPins[pin].enable && exioPins[pin].mode != MODE_DIGITAL && !exioPins[pin].direction) {
          USB_SERIAL.print(F("ERROR! pin "));
          USB_SERIAL.print(pinMap[pin].physicalPin);
          USB_SERIAL.println(F(" already in use, cannot use as a digital input pin"));
          break;
        }
        if (!exioPins[pin].enable) {
          exioPins[pin].direction = 1;   // Must be an input if we got a pullup config
          exioPins[pin].mode = MODE_DIGITAL;        // Must be digital if we got a pullup config
          exioPins[pin].pullup = pullup;
          exioPins[pin].enable = 1;
          if (exioPins[pin].pullup) {
            pinMode(pinMap[pin].physicalPin, INPUT_PULLUP);
          } else {
            pinMode(pinMap[pin].physicalPin, INPUT);
          }
        }
      } else {
        if(diag) {
          USB_SERIAL.println(F("EXIODPUP received with incorrect number of bytes"));
        }
      }
      break;
    case EXIORDAN:
      if (numBytes == 1) {
        outboundFlag = EXIORDAN;
      }
      break;
    case EXIOWRD:
      if (numBytes == 3) {
        uint8_t pin = buffer[1];
        bool state = buffer[2];
        uint8_t pinByte = pin / 8;
        uint8_t pinBit = pin - pinByte * 8;
        if (bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
          if (exioPins[pin].enable && (exioPins[pin].direction || exioPins[pin].mode != MODE_DIGITAL)) {
            USB_SERIAL.print(F("ERROR! pin "));
            USB_SERIAL.print(pinMap[pin].physicalPin);
            USB_SERIAL.println(F(" already in use, cannot use as a digital output pin"));
            break;
          }
          if (!exioPins[pin].enable) {
            exioPins[pin].enable = 1;
            exioPins[pin].mode = MODE_DIGITAL;
            exioPins[pin].direction = 0;
            pinMode(pinMap[pin].physicalPin, OUTPUT);
          }
          if (state) {
            bitSet(digitalPinStates[pinByte], pinBit);
          } else {
            bitClear(digitalPinStates[pinByte], pinBit);
          }
          digitalWrite(pinMap[pin].physicalPin, state);
        } else {
          USB_SERIAL.print(F("ERROR! Pin "));
          USB_SERIAL.print(pinMap[pin].physicalPin);
          USB_SERIAL.println(F(" not capable of digital output"));
        }
      }
      break;
    case EXIORDD:
      if (numBytes == 1) {
        outboundFlag = EXIORDD;
      }
      break;
    case EXIOVER:
      if (numBytes == 1) {
        outboundFlag = EXIOVER;
      }
      break;
    case EXIOENAN:
      if (numBytes == 2) {
        uint8_t pin = buffer[1];
        if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
          if (exioPins[pin].enable && exioPins[pin].mode != MODE_ANALOGUE && !exioPins[pin].direction) {
            USB_SERIAL.print(F("ERROR! pin "));
            USB_SERIAL.print(pinMap[pin].physicalPin);
            USB_SERIAL.println(F(" already in use, cannot use as an analogue input pin"));
            break;
          }
          exioPins[pin].enable = 1;
          exioPins[pin].mode = MODE_ANALOGUE;
          exioPins[pin].direction = 1;
          pinMode(pinMap[pin].physicalPin, INPUT);
        } else {
          USB_SERIAL.print(F("ERROR! Pin "));
          USB_SERIAL.print(pinMap[pin].physicalPin);
          USB_SERIAL.println(F(" not capable of analogue input"));
        }
      }
      break;
    case EXIOWRAN:
      if (numBytes == 4) {
        uint8_t pin = buffer[1];
        uint16_t value = (buffer[3] << 8) + buffer[2];
        if (bitRead(pinMap[pin].capability, PWM_OUTPUT)) {
          if (exioPins[pin].enable && (exioPins[pin].direction || exioPins[pin].mode != MODE_PWM)) {
            USB_SERIAL.print(F("ERROR! pin "));
            USB_SERIAL.print(pinMap[pin].physicalPin);
            USB_SERIAL.println(F(" already in use, cannot use as a PWM output pin"));
            break;
          } else {
            exioPins[pin].enable = 1;
            exioPins[pin].mode = MODE_PWM;
            exioPins[pin].direction = 0;
            USB_SERIAL.print(F("analogWrite pin|value:"));
            USB_SERIAL.print(pinMap[pin].physicalPin);
            USB_SERIAL.print(F("|"));
            USB_SERIAL.println(value);
            analogWrite(pinMap[pin].physicalPin, value);
          }
        } else {
          USB_SERIAL.print(F("ERROR! Pin "));
          USB_SERIAL.print(pinMap[pin].physicalPin);
          USB_SERIAL.println(F(" not capable of PWM output"));
        }
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
        commandBuffer[0] = EXIOPINS;
        commandBuffer[1] = numDigitalPins;
        commandBuffer[2] = numAnaloguePins;
      } else {
        commandBuffer[0] = 0;
        commandBuffer[1] = 0;
        commandBuffer[2] = 0;
      }
      Wire.write(commandBuffer, 3);
      break;
    case EXIOINITA:
      Wire.write(analoguePinMap, numAnaloguePins);
      break;
    case EXIORDAN:
      Wire.write(analoguePinStates, analoguePinBytes);
      break;
    case EXIORDD:
      Wire.write(digitalPinStates, digitalPinBytes);
      break;
    case EXIOVER:
      Wire.write(versionBuffer, 3);
      break;
    default:
      break;
  }
}

/*
* Function to display pin configuration and states when DIAG enabled
*/
void displayPins() {
  if (millis() - lastPinDisplay > displayDelay) {
    lastPinDisplay = millis();
    for (uint8_t pin = 0; pin < numPins; pin++) {
      uint8_t physicalPin = pinMap[pin].physicalPin;
      switch(exioPins[pin].mode) {
        case MODE_UNUSED: {
          USB_SERIAL.print(F("Pin "));
          USB_SERIAL.print(physicalPin);
          USB_SERIAL.println(F(" not in use"));
          break;
        }
        case MODE_DIGITAL: {
          uint8_t dPinByte = pin / 8;
          uint8_t dPinBit = pin - dPinByte * 8;
          USB_SERIAL.print(F("Digital Pin|Direction|Pullup|State:"));
          USB_SERIAL.print(physicalPin);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print(exioPins[pin].direction);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print(exioPins[pin].pullup);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.println(bitRead(digitalPinStates[dPinByte], dPinBit));
          break;
        }
        case MODE_ANALOGUE: {
          uint8_t lsbByte = exioPins[pin].analogueLSBByte;
          uint8_t msbByte = lsbByte + 1;
          USB_SERIAL.print(F("Analogue Pin|Value|LSB|MSB:"));
          USB_SERIAL.print(physicalPin);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print((analoguePinStates[msbByte] << 8) + analoguePinStates[lsbByte]);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print(analoguePinStates[lsbByte]);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.println(analoguePinStates[msbByte]);
          break;
        }
        case MODE_PWM: {
          break;
        }
        default:
          break;
      }
    }
  }
}

/*
* Function to get the version from version.h char array to bytes nicely
*/
void setVersion() {
  const String versionString = VERSION;
  char versionArray[versionString.length() + 1];
  versionString.toCharArray(versionArray, versionString.length() + 1);
  version = strtok(versionArray, "."); // Split version on .
  versionBuffer[0] = atoi(version);  // Major first
  version = strtok(NULL, ".");
  versionBuffer[1] = atoi(version);  // Minor next
  version = strtok(NULL, ".");
  versionBuffer[2] = atoi(version);  // Patch last
}

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
    unsigned long parameter;
    if (activity == 'W') {
      parameter = strtol(strtokIndex, NULL, 16); // last parameter is the address in hex
    } else {
      parameter = strtol(strtokIndex, NULL, 10);
    }
    switch (activity) {
      case 'A': // Enable/disable analogue input testing
        if (analogueTesting) {
          testAnalogue(false);
          USB_SERIAL.println(F("Analogue testing disabled"));
        } else {
          testAnalogue(true);
        }
        break;
      case 'D': // Enable/disable diagnostic output
        if (diag && parameter) {
          displayDelay = parameter * 1000;
          USB_SERIAL.print(F("Diagnostics enabled, delay set to "));
          USB_SERIAL.println(displayDelay);
          diag = true;
        } else if (diag && !parameter) {
          USB_SERIAL.println(F("Diagnostics disabled"));
          diag = false;
        } else {
          USB_SERIAL.print(F("Diagnostics enabled, delay set to "));
          USB_SERIAL.println(displayDelay);
          diag = true;
        }
        break;
      case 'E': // Erase EEPROM
        eraseI2CAddress();
        break;
      case 'I': // Enable/disable digital input testing
        if (inputTesting) {
          testInput(false);
          USB_SERIAL.println(F("Input testing (no pullups) disabled"));
        } else {
          testInput(true);
        }
        break;
      case 'O': // Enable/disable digital output testing
        if (outputTesting) {
          testOutput(false);
          USB_SERIAL.println(F("Output testing disabled"));
        } else {
          testOutput(true);
        }
        break;
      case 'P': // Enable/disable digital input testing with pullups
        if (pullupTesting) {
          testPullup(false);
          USB_SERIAL.println(F("Pullup input testing disabled"));
        } else {
          testPullup(true);
        }
        break;
      case 'R': // Read address from EEPROM
        if (getI2CAddress() == 0) {
          USB_SERIAL.println(F("I2C address not stored, using myConfig.h"));
        } else {
          USB_SERIAL.print(F("I2C address stored is 0x"));
          USB_SERIAL.println(getI2CAddress(), HEX);
        }
        break;
      case 'T': // Display current state of test modes
        if (analogueTesting) {
          USB_SERIAL.println(F("Analogue testing <A> enabled"));
        } else if (inputTesting) {
          USB_SERIAL.println(F("Input testing <I> (no pullups) enabled"));
        } else if (outputTesting) {
          USB_SERIAL.println(F("Output testing <O> enabled"));
        } else if (pullupTesting) {
          USB_SERIAL.println(F("Pullup input <P> testing enabled"));
        } else {
          USB_SERIAL.println(F("No testing in progress"));
        }
        break;
      case 'V': // Display Vpin map
        displayVpinMap();
        break;
      case 'W': // Write address to EEPROM
        if (parameter > 0x07 && parameter < 0x78) {
          writeI2CAddress(parameter);
        } else {
          USB_SERIAL.println(F("Invalid I2C address, must be between 0x08 and 0x77"));
        }
        break;
      case 'Z': // Software reboot
        reset();
        break;
      
      default:
        break;
    }
  }
}

// EEPROM functions here, only for uCs with EEPROM support
#if defined(HAS_EEPROM)
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
    eepromAddress = EEPROM.read(5);
    if(diag) {
      USB_SERIAL.print(F("I2C address defined in EEPROM: 0x"));
      USB_SERIAL.println(eepromAddress, HEX);
    }
    return eepromAddress;
  } else {
    if(diag) {
      USB_SERIAL.println(F("I2C address not defined in EEPROM"));
    }
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
  USB_SERIAL.print(F("Saving address 0x"));
  USB_SERIAL.print(eepromAddress, HEX);
  USB_SERIAL.println(F(" to EEPROM, reboot to activate"));
  EEPROM.write(5, eepromAddress);
}

/*
* Function to erase EEPROM contents
*/
void eraseI2CAddress() {
  for (uint8_t i = 0; i < 6; i++) {
    EEPROM.write(i, 0);
  }
  USB_SERIAL.println(F("Erased EEPROM, reboot to revert to myConfig.h"));
}

#else
// Placeholders for no EEPROM support
uint8_t getI2CAddress() {
  USB_SERIAL.println(F("No EEPROM support, use myConfig.h"));
  return 0;
}

void writeI2CAddress(int16_t notRequired) {
  USB_SERIAL.println(F("No EEPROM support, use myConfig.h"));
}

void eraseI2CAddress() {
  USB_SERIAL.println(F("No EEPROM support, use myConfig.h"));
}

#endif

void reset() {
#if defined(ARDUINO_ARCH_AVR)
  wdt_enable(WDTO_15MS);
  delay(50);
#elif defined(ARDUINO_ARCH_STM32)
  __disable_irq();
  NVIC_SystemReset();
  while(true) {};
#endif
}

/*
* Testing functions below, just pass true/false to enable/disable the appropriate testing
* Note enabling any of these will disable Wire() (providing the library supports it) so the
* device will need to be rebooted once testing is completed to enable it again.
*/
void testAnalogue(bool enable) {
  if (enable) {
    USB_SERIAL.println(F("Analogue input testing enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testInput(false);
    testOutput(false);
    testPullup(false);
    diag = true;
    analogueTesting = true;
    for (uint8_t pin = 0; pin < numPins; pin++) {
      if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
        exioPins[pin].enable = 1;
        exioPins[pin].mode = MODE_ANALOGUE;
        exioPins[pin].direction = 1;
      }
    }
  } else {
    analogueTesting = false;
    diag = false;
    initialisePins();
  }
}

void testInput(bool enable) {
  if (enable) {
    USB_SERIAL.println(F("Input testing (no pullups) enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testAnalogue(false);
    testOutput(false);
    testPullup(false);
    diag = true;
    inputTesting = true;
    for (uint8_t pin = 0; pin < numPins; pin++) {
      if (bitRead(pinMap[pin].capability, DIGITAL_INPUT)) {
        exioPins[pin].enable = 1;
        exioPins[pin].mode = MODE_DIGITAL;
        exioPins[pin].pullup = 0;
        exioPins[pin].direction = 1;
      }
    }
  } else {
    inputTesting = false;
    diag = false;
    initialisePins();
  }
}

void testOutput(bool enable) {
  if (enable) {
    USB_SERIAL.println(F("Output testing enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testAnalogue(false);
    testInput(false);
    testPullup(false);
    diag = true;
    outputTesting = true;
    for (uint8_t pin = 0; pin < numPins; pin++) {
      if (bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
        exioPins[pin].enable = 1;
        exioPins[pin].mode = DIGITAL_OUTPUT;
        exioPins[pin].direction = 0;
      }
    }
  } else {
    outputTesting = false;
    diag = false;
    initialisePins();
  }
}

void testPullup(bool enable) {
  if (enable) {
    USB_SERIAL.println(F("Pullup input testing enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testAnalogue(false);
    testOutput(false);
    testInput(false);
    diag = true;
    pullupTesting = true;
    for (uint8_t pin = 0; pin < numPins; pin++) {
      if (bitRead(pinMap[pin].capability, DIGITAL_INPUT)) {
        exioPins[pin].enable = 1;
        exioPins[pin].mode = MODE_DIGITAL;
        exioPins[pin].pullup = 1;
        exioPins[pin].direction = 1;
      }
    }
  } else {
    pullupTesting = false;
    diag = false;
    initialisePins();
  }
}

void disableWire() {
#ifdef WIRE_HAS_END
  Wire.end();
#else
  USB_SERIAL.println(F("WARNING! The Wire.h library has no end() function, ensure EX-IOExpander is disconnected from your CommandStation"));
#endif
}

/*
* Function to initialise all pins as input and initialise pin struct
*/
void initialisePins() {
  for (uint8_t pin = 0; pin < TOTAL_PINS; pin++) {
    if (bitRead(pinMap[pin].capability, DIGITAL_INPUT) || bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
      pinMode(pinMap[pin].physicalPin, INPUT);
      exioPins[pin].direction = 1;
    } else {
      exioPins[pin].direction = 0;
    }
    exioPins[pin].enable = 0;
    exioPins[pin].mode = 0;
    exioPins[pin].pullup = 0;
  }
  for (uint8_t dPinByte = 0; dPinByte < digitalPinBytes; dPinByte++) {
    digitalPinStates[dPinByte] = 0;
  }
  for (uint8_t aPinByte = 0; aPinByte < analoguePinBytes; aPinByte++) {
    analoguePinStates[aPinByte] = 0;
  }
}

/*
* Get the count of analogue and PWM capable pins
*/
void setupPinDetails() {
  for (uint8_t pin = 0; pin < numPins; pin++) {
    if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
      numAnaloguePins++;
    }
    if (bitRead(pinMap[pin].capability, DIGITAL_INPUT) || bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
      numDigitalPins++;
    }
    if (bitRead(pinMap[pin].capability, PWM_OUTPUT)) {
      numPWMPins++;
    }
  }
  analoguePinBytes = numAnaloguePins * 2;
  digitalPinBytes = numDigitalPins / 8;
  digitalPinStates = (byte*) calloc(digitalPinBytes, 1);
  analoguePinStates = (byte*) calloc(analoguePinBytes, 1);
}

/*
* Function to display Vpin to physical pin mappings after initialisation
*/
void displayVpinMap() {
  uint16_t vpin = firstVpin;
  USB_SERIAL.println(F("Vpin to physical pin mappings (Vpin => physical pin):"));
  for (uint8_t pin = 0; pin < numPins; pin++) {
    USB_SERIAL.print(vpin);
    USB_SERIAL.print(F(" => "));
    if (pin == numPins - 1 || (pin %9 == 0 && pin > 0)) {
      USB_SERIAL.println(pinMap[pin].physicalPin);
    } else {
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.print(F(","));
    }
    vpin++;
  }
}