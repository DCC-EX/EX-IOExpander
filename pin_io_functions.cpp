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
#include "pin_io_functions.h"
#include "servo_functions.h"

pinConfig exioPins[TOTAL_PINS];
int digitalPinBytes = 0;  // Used for configuring and sending/receiving digital pins
int analoguePinBytes = 0; // Used for sending analogue 16 bit values
byte* digitalPinStates;  // Store digital pin states to send to device driver
byte* analoguePinStates;

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
* Function to initialise all pins as input and initialise pin struct
*/
void initialisePins() {
  for (uint8_t pin = 0; pin < numPins; pin++) {
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
  for (uint8_t pin = 0; pin < numPins; pin++) {
    servoDataArray[pin] = NULL;
  }
}

/*
* Function to enable pins as analogue input pins to start reading
*/
bool enableAnalogue(uint8_t pin) {
  if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
    if (exioPins[pin].enable && exioPins[pin].mode != MODE_ANALOGUE && !exioPins[pin].direction) {
      USB_SERIAL.print(F("ERROR! pin "));
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.println(F(" already in use, cannot use as an analogue input pin"));
      return false;
    }
    exioPins[pin].enable = 1;
    exioPins[pin].mode = MODE_ANALOGUE;
    exioPins[pin].direction = 1;
    pinMode(pinMap[pin].physicalPin, INPUT);
    return true;
  } else {
    USB_SERIAL.print(F("ERROR! Pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" not capable of analogue input"));
    return false;
  }
}

/*
* Function to write PWM output to a pin
*/
bool writeAnalogue(uint8_t pin, uint16_t value, uint8_t profile, uint16_t duration) {
  if (bitRead(pinMap[pin].capability, PWM_OUTPUT)) {
    if (exioPins[pin].enable && (exioPins[pin].direction || exioPins[pin].mode != MODE_PWM)) {
      USB_SERIAL.print(F("ERROR! pin "));
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.println(F(" already in use, cannot use as a PWM output pin"));
      return false;
    } else {
      exioPins[pin].enable = 1;
      exioPins[pin].mode = MODE_PWM;
      exioPins[pin].direction = 0;

      if (value > 4095) value = 4095;
      else if (value < 0) value = 0;

      struct ServoData *s = servoDataArray[pin];
      if (s == NULL) {
        // Servo pin not configured, so configure now using defaults
        s = servoDataArray[pin] = (struct ServoData *) calloc(sizeof(struct ServoData), 1);
        if (s == NULL) return false;  // Check for memory allocation failure
        s->activePosition = 4095;
        s->inactivePosition = 0;
        s->currentPosition = value;
        s->profile = SERVO_INSTANT | SERVO_NOPOWEROFF;  // Use instant profile (but not this time)
      }

      // Animated profile.  Initiate the appropriate action.
      s->currentProfile = profile;
      uint8_t profileValue = profile & ~SERVO_NOPOWEROFF;  // Mask off 'don't-power-off' bit.
      s->numSteps = profileValue==SERVO_FAST ? 10 :   // 0.5 seconds
                    profileValue==SERVO_MEDIUM ? 20 : // 1.0 seconds
                    profileValue==SERVO_SLOW ? 40 :   // 2.0 seconds
                    profileValue==SERVO_BOUNCE ? sizeof(bounceProfile)-1 : // ~ 1.5 seconds
                    duration * 2 + 1; // Convert from deciseconds (100ms) to refresh cycles (50ms)
      s->stepNumber = 0;
      s->toPosition = value;
      s->fromPosition = s->currentPosition;
      return true;
    }
  } else {
    USB_SERIAL.print(F("ERROR! Pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" not capable of PWM output"));
    return false;
  }
}

void writePWM(uint8_t pin, uint16_t value) {
  if (value >= 0 && value <= 255) {
    analogWrite(pinMap[pin].physicalPin, value);
  }
}

void processInputs() {
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
}

bool processOutputTest(bool testState) {
  if (outputTesting) {
    unsigned long lastOutputTest;
    if (millis() - lastOutputTest > 1000) {
      testState = !testState;
      lastOutputTest = millis();
      for (uint8_t pin = 0; pin < numPins; pin++) {
        uint8_t pinByte = pin / 8;
        uint8_t pinBit = pin - pinByte * 8;
        if (bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
          pinMode(pinMap[pin].physicalPin, OUTPUT);
          digitalWrite(pinMap[pin].physicalPin, testState);
          if (testState) {
            bitSet(digitalPinStates[pinByte], pinBit);
          } else {
            bitClear(digitalPinStates[pinByte], pinBit);
          }
        }
      }
    }
  }
  return testState;
}