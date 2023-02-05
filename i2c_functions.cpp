#include <Arduino.h>
#include <Wire.h>
#include "globals.h"
#include "i2c_functions.h"
#include "display_functions.h"
#include "pin_io_functions.h"

bool setupComplete = false;   // Flag when initial configuration/setup has been received
uint8_t outboundFlag;   // Used to determine what data to send back to the CommandStation
byte commandBuffer[3];    // Command buffer to interact with device driver

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
        enableAnalogue(pin);
      }
      break;
    case EXIOWRAN:
      if (numBytes == 4) {
        uint8_t pin = buffer[1];
        uint16_t value = (buffer[3] << 8) + buffer[2];
        writeAnalogue(pin, value);
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