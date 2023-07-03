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
#include <Wire.h>
#include "globals.h"
#include "i2c_functions.h"
#include "display_functions.h"
#include "pin_io_functions.h"

uint8_t numAnaloguePins = 0;  // Init with 0, will be overridden by config
uint8_t numDigitalPins = 0;   // Init with 0, will be overridden by config
uint8_t numPWMPins = 0;  // Number of PWM capable pins
bool setupComplete = false;   // Flag when initial configuration/setup has been received
uint8_t outboundFlag;   // Used to determine what data to send back to the CommandStation
byte commandBuffer[3];    // Command buffer to interact with device driver
byte responseBuffer[1];   // Buffer to send single response back to device driver
uint8_t numReceivedPins = 0;

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
        numReceivedPins = buffer[1];
        firstVpin = (buffer[3] << 8) + buffer[2];
        if (numReceivedPins == numPins) {
          displayEventFlag = 0;
          setupComplete = true;
        } else {
          displayEventFlag = 1;
          setupComplete = false;
        }
        outboundFlag = EXIOINIT;
        displayEvent = EXIOINIT;
      } else {
        displayEventFlag = 2;
      }
      break;
    case EXIOINITA:
      if (numBytes == 1) {
        outboundFlag = EXIOINITA;
      } else {
        displayEvent = EXIOINITA;
      }
      break;
    // Flag to set digital pin pullups, 0 disabled, 1 enabled
    case EXIODPUP:
      outboundFlag = EXIODPUP;
      if (numBytes == 3) {
        uint8_t pin = buffer[1];
        bool pullup = buffer[2];
        bool response = enableDigitalInput(pin, pullup);
        if (response) {
          responseBuffer[0] = EXIORDY;
        } else {
          responseBuffer[0] = EXIOERR;
        }
      } else {
        displayEvent = EXIODPUP;
        responseBuffer[0] = EXIOERR;
      }
      break;
    case EXIORDAN:
      if (numBytes == 1) {
        outboundFlag = EXIORDAN;
      }
      break;
    case EXIOWRD:
      outboundFlag = EXIOWRD;
      if (numBytes == 3) {
        uint8_t pin = buffer[1];
        bool state = buffer[2];
        bool response = writeDigitalOutput(pin, state);
        if (response) {
          responseBuffer[0] = EXIORDY;
        } else {
          responseBuffer[0] = EXIOERR;
        }
      } else {
        displayEvent = EXIOWRD;
        responseBuffer[0] = EXIOERR;        
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
      outboundFlag = EXIOENAN;
      if (numBytes == 2) {
        uint8_t pin = buffer[1];
        bool response = enableAnalogue(pin);
        if (response) {
          responseBuffer[0] = EXIORDY;
        } else {
          responseBuffer[0] = EXIOERR;
        }
      } else {
        responseBuffer[0] = EXIOERR;
      }
      break;
    case EXIOWRAN:
      outboundFlag = EXIOWRAN;
      if (numBytes == 7) {
        uint8_t pin = buffer[1];
        uint16_t value = (buffer[3] << 8) + buffer[2];
        uint8_t profile = buffer[4];
        uint16_t duration = (buffer[6] << 8) + buffer[5];
        bool response = writeAnalogue(pin, value, profile, duration);
        if (response) {
          responseBuffer[0] = EXIORDY;
        } else {
          responseBuffer[0] = EXIOERR;
        }
      } else {
        displayEvent = EXIOWRAN;
        responseBuffer[0] = EXIOERR;
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
    case EXIODPUP:
      Wire.write(responseBuffer, 1);
      break;
    case EXIOENAN:
      Wire.write(responseBuffer, 1);
      break;
    case EXIOWRAN:
      Wire.write(responseBuffer, 1);
      break;
    case EXIOWRD:
      Wire.write(responseBuffer, 1);
      break;
    default:
      break;
  }
}

void disableWire() {
#ifdef WIRE_HAS_END
  Wire.end();
#else
  USB_SERIAL.println(F("WARNING! The Wire.h library has no end() function, ensure EX-IOExpander is disconnected from your CommandStation"));
#endif
}