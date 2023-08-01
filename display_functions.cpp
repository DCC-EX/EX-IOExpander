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
#include "i2c_functions.h"

char * version;   // Pointer for getting version
uint8_t versionBuffer[3];   // Array to hold version info to send to device driver
unsigned long lastPinDisplay = 0;   // Last time in millis we displayed DIAG input states
unsigned long displayDelay = DIAG_CONFIG_DELAY * 1000;    // Delay in ms between diag display updates
uint16_t firstVpin = 0;
uint8_t displayEvent = 0x00;
uint8_t displayEventFlag = 0;

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
    USB_SERIAL.println("Current pin states:");
    for (uint8_t pin = 0; pin < numPins; pin++) {
      String pinLabel = pinNameMap[pin].pinLabel;
      if (pinLabel.length() == 1) {
        pinLabel += "   ";
      } else if (pinLabel.length() == 2) {
        pinLabel += "  ";
      } else if (pinLabel.length() == 3) {
        pinLabel += " ";
      }
      switch(exioPins[pin].mode) {
        case MODE_UNUSED: {
          USB_SERIAL.print(F("Pin "));
          USB_SERIAL.print(pinLabel);
          USB_SERIAL.println(F(" not in use"));
          break;
        }
        case MODE_DIGITAL: {
          uint8_t dPinByte = pin / 8;
          uint8_t dPinBit = pin - dPinByte * 8;
          USB_SERIAL.print(F("Digital Pin|Direction|Pullup|State:"));
          USB_SERIAL.print(pinLabel);
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
          USB_SERIAL.print(pinLabel);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print((analoguePinStates[msbByte] << 8) + analoguePinStates[lsbByte]);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print(analoguePinStates[lsbByte]);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.println(analoguePinStates[msbByte]);
          break;
        }
        case MODE_PWM: {
          uint8_t dPinByte = pin / 8;
          uint8_t dPinBit = pin - dPinByte * 8;
          USB_SERIAL.print(F("PWM Output Pin|Servo|State:"));
          USB_SERIAL.print(pinLabel);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print(exioPins[pin].servoIndex);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.println(bitRead(digitalPinStates[dPinByte], dPinBit));
          break;
        }
        case MODE_PWM_LED: {
          uint8_t dPinByte = pin / 8;
          uint8_t dPinBit = pin - dPinByte * 8;
          USB_SERIAL.print(F("LED Output Pin|Servo|State:"));
          USB_SERIAL.print(pinLabel);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.print(exioPins[pin].servoIndex);
          USB_SERIAL.print(F("|"));
          USB_SERIAL.println(bitRead(digitalPinStates[dPinByte], dPinBit));
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
    if (String(vpin).length() == 1) {
      USB_SERIAL.print(F("    => "));
    } else if (String(vpin).length() == 2) {
      USB_SERIAL.print(F("   => "));
    } else if (String(vpin).length() == 3) {
      USB_SERIAL.print(F("  => "));
    } else {
      USB_SERIAL.print(F(" => "));
    }
    String pinLabel = pinNameMap[pin].pinLabel;
    if (pinLabel.length() < 3) {
      USB_SERIAL.print(F("  "));
    } else if (pinLabel.length() < 4) {
      USB_SERIAL.print(F(" "));
    }
    USB_SERIAL.print(pinLabel);
    if (pin == numPins - 1 || (pin + 1) % 8 == 0) {
      USB_SERIAL.println(F("|"));
    }
    vpin++;
  }
}

void processDisplayOutput() {
  switch (displayEvent) {
    case EXIOINIT:
      if (displayEventFlag == 0) {
        USB_SERIAL.print(F("Received correct pin count: "));
        USB_SERIAL.print(numReceivedPins);
        USB_SERIAL.print(F(", starting at Vpin: "));
        USB_SERIAL.println(firstVpin);
        displayVpinMap();
      } else if (displayEventFlag == 1) {
        USB_SERIAL.print(F("ERROR: Invalid pin count sent by device driver!: "));
        USB_SERIAL.println(numReceivedPins);
      } else if (displayEventFlag == 2) {
        if (diag) {
          USB_SERIAL.println(F("EXIOINIT received with incorrect data"));
        }
      }
      displayEvent = 0x00;
      displayEventFlag = 0;
      break;
    case EXIOINITA:
      if (diag) {
        USB_SERIAL.println(F("EXIOINITA received with incorrect data"));
      }
      displayEvent = 0x00;
      displayEventFlag = 0;
      break;
    case EXIODPUP:
      if(diag) {
        USB_SERIAL.println(F("EXIODPUP received with incorrect number of bytes"));
      }
      displayEvent = 0x00;
      displayEventFlag = 0;
      break;
    case EXIOWRD:
      if(diag) {
        USB_SERIAL.println(F("EXIOWRD received with incorrect number of bytes"));
      }
      displayEvent = 0x00;
      displayEventFlag = 0;
      break;
    case EXIOWRAN:
      if(diag) {
        USB_SERIAL.println(F("EXIOWRD received with incorrect number of bytes"));
      }
      displayEvent = 0x00;
      displayEventFlag = 0;
      break;
    default:
      break;
  }
}

/*
* Function to display startup info
*/
void startupDisplay() {
  USB_SERIAL.print(F("DCC-EX EX-IOExpander v"));
  USB_SERIAL.println(VERSION);
  USB_SERIAL.print(F("Detected device: "));
  USB_SERIAL.println(BOARD_TYPE);
  if (i2cAddress < 0x08 || i2cAddress > 0x77) {
    USB_SERIAL.print(F("ERROR: Invalid I2C address configured: 0x"));
    USB_SERIAL.print(i2cAddress, HEX);
    USB_SERIAL.println(F(", using myConfig.h instead"));
    i2cAddress = I2C_ADDRESS;
  }
  USB_SERIAL.print(F("Available at I2C address 0x"));
  USB_SERIAL.println(i2cAddress, HEX);
#if defined(HAS_SERVO_LIB)
  USB_SERIAL.print(F("Servo library support for up to "));
  USB_SERIAL.print(MAX_SERVOS);
  USB_SERIAL.println(F(" servos"));
#else
  USB_SERIAL.print(F("Use hardware PWM pins for up to "));
  USB_SERIAL.print(numPWMPins);
  USB_SERIAL.println(F(" servos/LEDs"));
#endif
  USB_SERIAL.print(F("SuperPin support to dim up to "));
  USB_SERIAL.print(MAX_SUPERPINS);
  USB_SERIAL.println(F(" LEDs"));
#if defined(DISABLE_I2C_PULLUPS) && defined(I2C_SDA) && defined(I2C_SCL)
  USB_SERIAL.print(F("Disabling I2C pullups on pins SDA|SCL: "));
  USB_SERIAL.print(I2C_SDA);
  USB_SERIAL.print(F("|"));
  USB_SERIAL.println(I2C_SCL);
#endif
}
