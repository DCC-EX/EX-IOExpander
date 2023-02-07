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
#include "version.h"
#include "display_functions.h"

char * version;   // Pointer for getting version
uint8_t versionBuffer[3];   // Array to hold version info to send to device driver
unsigned long lastPinDisplay = 0;   // Last time in millis we displayed DIAG input states
unsigned long displayDelay = DIAG_CONFIG_DELAY * 1000;    // Delay in ms between diag display updates
uint16_t firstVpin = 0;

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
          USB_SERIAL.print(F("PWM Output Pin:"));
          USB_SERIAL.println(physicalPin);
          break;
        }
        default:
          break;
      }
    }
  }
}

/*
* Function to display Vpin to physical pin mappings after initialisation
*/
void displayVpinMap() {
  uint16_t vpin = firstVpin;
  USB_SERIAL.println(F("Vpin to physical pin mappings (Vpin => physical pin):"));
  for (uint8_t pin = 0; pin < numPins; pin++) {
    USB_SERIAL.print(F("|"));
    USB_SERIAL.print(vpin);
    USB_SERIAL.print(F(" => "));
    if (pinMap[pin].physicalPin < 10) {
      USB_SERIAL.print(F("  "));
    } else if (pinMap[pin].physicalPin < 100) {
      USB_SERIAL.print(F(" "));
    }
    USB_SERIAL.print(pinMap[pin].physicalPin);
    if (pin == numPins - 1 || pin % 10 == 0  && pin > 0) {
      USB_SERIAL.println(F("|"));
    }
    vpin++;
  }
}