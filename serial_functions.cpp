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
#include "serial_functions.h"
#include "test_functions.h"
#include "device_functions.h"
#include "display_functions.h"

bool newSerialData = false;   // Flag for new serial data being received
const byte numSerialChars = 20;   // Max number of chars for serial input
char serialInputChars[numSerialChars];  // Char array for serial input

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
    uint8_t vpin, profile;
    uint16_t value;
    if (activity == 'W') {
      parameter = strtol(strtokIndex, NULL, 16); // last parameter is the address in hex
    } else if (activity == 'T') {
      parameter = strtokIndex[0];
      if (parameter == 'S') {
        strtokIndex = strtok(NULL," ");
        vpin = strtoul(strtokIndex, NULL, 10); // get Vpin, this needs to be disconnected from CS
        strtokIndex = strtok(NULL, " ");
        value = strtoul(strtokIndex, NULL, 10); // get value of the angle or dimming
        strtokIndex = strtok(NULL, " ");
        profile = strtoul(strtokIndex, NULL, 10);  // get the profile
      }
    } else {
      parameter = strtol(strtokIndex, NULL, 10);
    }
    switch (activity) {
      case 'D': // Enable/disable diagnostic output
        serialCaseD(parameter);
        break;
      case 'E': // Erase EEPROM
        eraseI2CAddress();
        break;
      case 'R': // Read address from EEPROM
        serialCaseR();
        break;
      case 'T': // Display current state of test modes
        if (parameter == 'A') {
          setAnalogueTesting();
        } else if (parameter == 'I') {
          setInputTesting();
        } else if (parameter == 'O') {
          setOutputTesting();
        } else if (parameter == 'P') {
          setPullupTesting();
        } else if (parameter == 'S') {
          testServo(vpin, value, profile);
        } else {
          serialCaseT();
        }
        break;
      case 'V': // Display Vpin map
        startupDisplay();
        displayVpinMap();
        break;
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

void setAnalogueTesting() {
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
    if (parameter) {
      displayDelay = parameter * 1000;
    }
    USB_SERIAL.print(F("Diagnostics enabled, delay set to "));
    USB_SERIAL.println(displayDelay);
    diag = true;
  }
}

void setInputTesting() {
  if (inputTesting) {
    testInput(false);
    USB_SERIAL.println(F("Input testing (no pullups) disabled"));
  } else {
    testInput(true);
  }
}

void setOutputTesting() {
  if (outputTesting) {
    testOutput(false);
    USB_SERIAL.println(F("Output testing disabled"));
  } else {
    testOutput(true);
  }
}

void setPullupTesting() {
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