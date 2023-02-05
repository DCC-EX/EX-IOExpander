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
* Include required files and libraries.
*/
#include <Arduino.h>
#include "globals.h"
#include "version.h"
#include <Wire.h>
#include "pin_io_functions.h"
#include "display_functions.h"
#include "i2c_functions.h"
#include "serial_functions.h"
#include "test_functions.h"
#if defined(ARDUINO_ARCH_AVR)
#include <avr/wdt.h>
#endif
#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif

#ifdef CPU_TYPE_ERROR
#error Unsupported microcontroller architecture detected, you need to use a type listed in SupportedDevices.h
#endif

/*
* Include our CPU specific file
*/
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_PRO)
#include "arduino_avr_nano.h"
#elif defined(ARDUINO_AVR_UNO)
#include "arduino_avr_uno.h"
#endif

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
uint8_t numPins = TOTAL_PINS;
uint8_t numAnaloguePins = 0;  // Init with default, will be overridden by config
uint8_t numDigitalPins = 0;
uint8_t numPWMPins = 0;  // Number of PWM capable pins
int digitalPinBytes = 0;  // Used for configuring and sending/receiving digital pins
int analoguePinBytes = 0; // Used for sending analogue 16 bit values
bool newSerialData = false;   // Flag for new serial data being received
const byte numSerialChars = 10;   // Max number of chars for serial input
char serialInputChars[numSerialChars];  // Char array for serial input
bool diag = false;    // Enable/disable diag outputs
bool analogueTesting = false;   // Flag that analogue input testing is enabled/disabled
bool inputTesting = false;    // Flag that digital input testing without pullups is enabled/disabled
bool outputTesting = false;   // Flag that digital output testing is enabled/disabled
bool pullupTesting = false;   // Flag that digital input testing with pullups is enabled/disabled
unsigned long lastOutputTest = 0;   // Last time in millis we swapped output test state
bool outputTestState = LOW;   // Flag to set outputs high or low for testing
uint8_t* analoguePinMap;  // Map which analogue pin's value is in which byte

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
        serialCaseA();
        break; 
      case 'D': // Enable/disable diagnostic output
        serialCaseD(parameter);
        break;
      case 'E': // Erase EEPROM
        eraseI2CAddress();
        break;
      case 'I': // Enable/disable digital input testing
        serialCaseI();
        break;
      case 'O': // Enable/disable digital output testing
        serialCaseO();
        break;
      case 'P': // Enable/disable digital input testing with pullups
        serialCaseP();
        break;
      case 'R': // Read address from EEPROM
        serialCaseR();
        break;
      case 'T': // Display current state of test modes
        serialCaseT();
        break;
      /*case 'V': // Display Vpin map
        displayVpinMap();
        break;*/
      case 'W': // Write address to EEPROM
        serialCaseW(parameter);
        break;
      case 'Z': // Software reboot
        reset();
        break;
      default:
        break;
    }
  }
}

void serialCaseA() {
  if (analogueTesting) {
    testAnalogue(false);
    USB_SERIAL.println(F("Analogue testing disabled"));
  } else {
    testAnalogue(true);
  }
}

void serialCaseD(unsigned long parameter) {
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
}

void serialCaseI() {
  if (inputTesting) {
    testInput(false);
    USB_SERIAL.println(F("Input testing (no pullups) disabled"));
  } else {
    testInput(true);
  }
}

void serialCaseO() {
  if (outputTesting) {
    testOutput(false);
    USB_SERIAL.println(F("Output testing disabled"));
  } else {
    testOutput(true);
  }
}

void serialCaseP() {
  if (pullupTesting) {
    testPullup(false);
    USB_SERIAL.println(F("Pullup input testing disabled"));
  } else {
    testPullup(true);
  }
}

void serialCaseR() {
  if (getI2CAddress() == 0) {
    USB_SERIAL.println(F("I2C address not stored, using myConfig.h"));
  } else {
    USB_SERIAL.print(F("I2C address stored is 0x"));
    USB_SERIAL.println(getI2CAddress(), HEX);
  }
}

void serialCaseT() {
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
}

void serialCaseW(unsigned long parameter) {
  if (parameter > 0x07 && parameter < 0x78) {
    writeI2CAddress(parameter);
  } else {
    USB_SERIAL.println(F("Invalid I2C address, must be between 0x08 and 0x77"));
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