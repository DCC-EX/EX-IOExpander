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
byte* digitalPinStates;   // Store digital pin states to send to device driver
byte* analoguePinStates;  // Store analogue pin states to send to device driver
unsigned long lastOutputTest = 0; // Delay for output testing

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
  digitalPinBytes = (numDigitalPins + 7) / 8;
  digitalPinStates = (byte*) calloc(digitalPinBytes, 1);
  analoguePinStates = (byte*) calloc(analoguePinBytes, 1);
  analoguePinMap = (uint8_t*) calloc(numAnaloguePins, 1);
}

/*
* Function to initialise all pins as input and initialise pin struct
*/
void initialisePins() {
  for (uint8_t pin = 0; pin < numPins; pin++) {
#if defined(HAS_SERVO_LIB)
    if (exioPins[pin].servoIndex != 255 && servoMap[exioPins[pin].servoIndex].attached()) {
      servoMap[exioPins[pin].servoIndex].detach();
    }
#endif
    if (bitRead(pinMap[pin].capability, DIGITAL_INPUT) || bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
      pinMode(pinMap[pin].physicalPin, INPUT);
      exioPins[pin].direction = 1;
    } else {
      exioPins[pin].direction = 0;
    }
    exioPins[pin].enable = 0;
    exioPins[pin].mode = 0;
    exioPins[pin].pullup = 0;
    exioPins[pin].servoIndex = 255;
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
#if defined(HAS_SERVO_LIB)
  nextServoObject = 0;
#endif
}

/*
* Function to enable pins as digital input pins to start reading
*/
bool enableDigitalInput(uint8_t pin, bool pullup) {
  if (!bitRead(pinMap[pin].capability, DI)) {
    USB_SERIAL.print(F("ERROR! pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" not capable of digital input"));
    return false;
  }
  if (exioPins[pin].enable && exioPins[pin].mode != MODE_DIGITAL && !exioPins[pin].direction) {
    USB_SERIAL.print(F("ERROR! pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" already in use, cannot use as a digital input pin"));
    return false;
  }
  if (!exioPins[pin].enable || (exioPins[pin].enable && exioPins[pin].direction == 1)) {
    exioPins[pin].direction = 1;   // Must be an input if we got a pullup config
    exioPins[pin].mode = MODE_DIGITAL;        // Must be digital if we got a pullup config
    exioPins[pin].pullup = pullup;
    exioPins[pin].enable = 1;
    if (exioPins[pin].pullup) {
      pinMode(pinMap[pin].physicalPin, INPUT_PULLUP);
    } else {
      pinMode(pinMap[pin].physicalPin, INPUT);
    }
    return true;
  } else {
    USB_SERIAL.print(F("ERROR! pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" already in use, cannot use as a digital input pin"));
    return false;
  }
}

/* 
* Function to write to a digital output pint
*/
bool writeDigitalOutput(uint8_t pin, bool state) {
  uint8_t pinByte = pin / 8;
  uint8_t pinBit = pin - pinByte * 8;
  if (bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
    if (exioPins[pin].enable && (exioPins[pin].direction || exioPins[pin].mode != MODE_DIGITAL)) {
      USB_SERIAL.print(F("ERROR! pin "));
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.println(F(" already in use, cannot use as a digital output pin"));
      return false;
    }
    if (!exioPins[pin].enable || (exioPins[pin].enable && exioPins[pin].direction == 0)) {
      exioPins[pin].enable = 1;
      exioPins[pin].mode = MODE_DIGITAL;
      exioPins[pin].direction = 0;
      pinMode(pinMap[pin].physicalPin, OUTPUT);
      if (state) {
        bitSet(digitalPinStates[pinByte], pinBit);
      } else {
        bitClear(digitalPinStates[pinByte], pinBit);
      }
      digitalWrite(pinMap[pin].physicalPin, state);
      return true;
    } else {
      return false;
    }
  } else {
    USB_SERIAL.print(F("ERROR! Pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" not capable of digital output"));
    return false;
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
  bool useServoLib = false;
#if defined(HAS_SERVO_LIB)
  useServoLib = true;
#endif
  bool useSuperPin = bitRead(profile, 7); // if bit 7 is set, we're using FADE, therefore use SuperPin
  if (((useServoLib || useSuperPin) && bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) ||
      bitRead(pinMap[pin].capability, PWM_OUTPUT)) {
    if (exioPins[pin].enable && (exioPins[pin].direction ||
        (exioPins[pin].mode != MODE_PWM && exioPins[pin].mode != MODE_PWM_LED))) {
      USB_SERIAL.print(F("ERROR! pin "));
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.println(F(" already in use, cannot use as a PWM output pin"));
      return false;
    } else {
      if (useServoLib || useSuperPin) {
        if (!configureServo(pin, useSuperPin)) return false;
      }
      if (!exioPins[pin].enable) {
        exioPins[pin].enable = 1;
        if (useSuperPin) {
          exioPins[pin].mode = MODE_PWM_LED;
        } else {
          exioPins[pin].mode = MODE_PWM;
        }
        exioPins[pin].direction = 0;
      }

      uint8_t pinByte = pin / 8;
      uint8_t pinBit = pin - pinByte * 8;
      bitSet(digitalPinStates[pinByte], pinBit);

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
        s->profile = SERVO_INSTANT | USE_SUPERPIN;  // Use instant profile (but not this time)
      }

      // Animated profile.  Initiate the appropriate action.
      s->currentProfile = profile;
      uint8_t profileValue = profile & ~USE_SUPERPIN;  // Mask off 'don't-power-off' bit.
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
    USB_SERIAL.print(pinNameMap[pin].pinLabel);
    USB_SERIAL.println(F(" not capable of PWM output"));
    return false;
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
