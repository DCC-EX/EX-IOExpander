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
#include "device_functions.h"

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
int digitalPinBytes = 0;  // Used for configuring and sending/receiving digital pins
int analoguePinBytes = 0; // Used for sending analogue 16 bit values
unsigned long lastOutputTest = 0;   // Last time in millis we swapped output test state
bool outputTestState = LOW;   // Flag to set outputs high or low for testing
uint8_t* analoguePinMap;  // Map which analogue pin's value is in which byte

#ifdef DIAG
  diag = true;
#else
  bool diag = false;
#endif

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