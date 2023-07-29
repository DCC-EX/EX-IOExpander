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
#include "servo_functions.h"
#include "pin_io_functions.h"
#if defined(HAS_SERVO_LIB)
#include "Servo.h"
uint8_t nextServoObject = 0;
bool useServoLib = 1;
#else
bool useServoLib = 0;
#endif
#include "SuperPin.h"
uint8_t nextSuperPinObject = 0;

const unsigned int refreshInterval = 50;
unsigned long lastRefresh = 0;
ServoData** servoDataArray;

void processServos() {
  if (millis() - lastRefresh > refreshInterval) {
    lastRefresh = millis();
    for (uint8_t pin = 0; pin < numPins; pin++) {
      if (servoDataArray[pin] != NULL) {
        updatePosition(pin);
      }
    }
  }
}

void updatePosition(uint8_t pin) {
  struct ServoData *s = servoDataArray[pin];
  if (s == NULL) return; // No pin configuration/state data
  
  uint8_t pinByte = pin / 8;
  uint8_t pinBit = pin - pinByte * 8;
  if (s->numSteps == 0) {
    bitClear(digitalPinStates[pinByte], pinBit);
    return; // No animation in progress
  }

  if (s->stepNumber == 0 && s->fromPosition == s->toPosition) {
    // Go straight to end of sequence, output final position.
    bitSet(digitalPinStates[pinByte], pinBit);
    s->stepNumber = s->numSteps-1;
  }

  if (s->stepNumber < s->numSteps) {
    bitSet(digitalPinStates[pinByte], pinBit);
    // Animation in progress, reposition servo
    s->stepNumber++;
    bool useSuperPin = s->currentProfile & USE_SUPERPIN;
    if ((s->currentProfile & ~USE_SUPERPIN) == SERVO_BOUNCE) {
      // Retrieve step positions from array in flash
      uint8_t profileValue = bounceProfile[s->stepNumber];
      s->currentPosition = map(profileValue, 0, 100, s->fromPosition, s->toPosition);
    } else {
      // All other profiles - calculate step by linear interpolation between from and to positions.
      s->currentPosition = map(s->stepNumber, 0, s->numSteps, s->fromPosition, s->toPosition);
    }
    // Send servo command
    bitSet(digitalPinStates[pinByte], pinBit);
    writeServo(pin, s->currentPosition, useSuperPin);
  } else if (s->stepNumber < s->numSteps + _catchupSteps) {
    bitSet(digitalPinStates[pinByte], pinBit);
    // We've finished animation, wait a little to allow servo to catch up
    s->stepNumber++;
  } else if (s->stepNumber == s->numSteps + _catchupSteps 
            && s->currentPosition != 0) {
    bitClear(digitalPinStates[pinByte], pinBit);
    s->numSteps = 0;  // Done now.
  }
}

bool configureServo(uint8_t pin, bool useSuperPin) {
  if (exioPins[pin].servoIndex == 255) {
    if (((!useSuperPin && nextServoObject < MAX_SERVOS) || (useSuperPin && nextSuperPinObject < MAX_SUPERPINS)) && bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
      if (useSuperPin) {
        exioPins[pin].servoIndex = nextSuperPinObject;
        nextSuperPinObject++;
      } else {
        exioPins[pin].servoIndex = nextServoObject;
        nextServoObject++;
      }
    } else {
      return false;
    }
  }
  if (useSuperPin) {
    if (!superPinMap[exioPins[pin].servoIndex].attached()) {
      superPinMap[exioPins[pin].servoIndex].attach(pinMap[pin].physicalPin);
    }
  } else {
    if (!servoMap[exioPins[pin].servoIndex].attached()) {
      servoMap[exioPins[pin].servoIndex].attach(pinMap[pin].physicalPin);
    }
  }
  return true;
}

void writeServo(uint8_t pin, uint16_t value, bool useSuperPin) {
  if (useServoLib || useSuperPin) {
    if (exioPins[pin].mode == MODE_PWM) {
      servoMap[exioPins[pin].servoIndex].writeMicroseconds(value);
    } else if (exioPins[pin].mode == MODE_PWM_LED) {
      superPinMap[exioPins[pin].servoIndex].write(value);
    }
  } else {
    if (value >= 0 && value <= 255) {
      analogWrite(pinMap[pin].physicalPin, value);
    }
  }
}
