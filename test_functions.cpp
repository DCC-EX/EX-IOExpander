/*
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

#include <Arduino.h>
#include "globals.h"
#include "test_functions.h"
#include "i2c_functions.h"
#include "pin_io_functions.h"

bool analogueTesting = false; // Flag that analogue input testing is enabled/disabled
bool inputTesting = false;    // Flag that digital input testing without pullups is enabled/disabled
bool outputTesting = false;   // Flag that digital output testing is enabled/disabled
bool pullupTesting = false;   // Flag that digital input testing with pullups is enabled/disabled

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

void testServo(uint8_t vpin, uint16_t value, uint8_t profile) {
  if (firstVpin > 0) {
    USB_SERIAL.println(F("EX-IOExpander has been connected and configured, please disconnect from EX-CommandStation and reboot"));
  } else if (analogueTesting || inputTesting || outputTesting || pullupTesting) {
    USB_SERIAL.println(F("Please disable all other testing first"));
  } else {
    String pinLabel = pinNameMap[vpin].pinLabel;
    USB_SERIAL.print(F("Test move servo or dim LED - vpin|physicalPin|value|profile:"));
    USB_SERIAL.print(vpin);
    USB_SERIAL.print(F("|"));
    USB_SERIAL.print(pinLabel);
    USB_SERIAL.print(F("|"));
    USB_SERIAL.print(value);
    USB_SERIAL.print(F("|"));
    USB_SERIAL.println(profile);
    if (!setupComplete) {
      setupComplete = true;
    }
    disableWire();
    writeAnalogue(vpin, value, profile);
  }
}
