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

const unsigned int refreshInterval = 50;
unsigned long lastRefresh = 0;

void processServos() {
  for (uint8_t pin = 0; pin < numPins; pin++) {
    if (servoDataArray[pin] != NULL) {
      updatePosition(pin);
    }
  }
}

void updatePosition(uint8_t pin) {
  struct ServoData *s = servoDataArray[pin];
  if (s == NULL) return; // No pin configuration/state data

  if (s->numSteps == 0) return; // No animation in progress

  if (s->stepNumber == 0 && s->fromPosition == s->toPosition) {
    // Go straight to end of sequence, output final position.
    s->stepNumber = s->numSteps-1;
  }

  if (s->stepNumber < s->numSteps) {
    // Animation in progress, reposition servo
    s->stepNumber++;
    if ((s->currentProfile & ~SERVO_NOPOWEROFF) == SERVO_BOUNCE) {
      // Retrieve step positions from array in flash
      uint8_t profileValue = bounceProfile[s->stepNumber];
      s->currentPosition = map(profileValue, 0, 100, s->fromPosition, s->toPosition);
    } else {
      // All other profiles - calculate step by linear interpolation between from and to positions.
      s->currentPosition = map(s->stepNumber, 0, s->numSteps, s->fromPosition, s->toPosition);
    }
    // Send servo command
    writePWM(pin, s->currentPosition);
  } else if (s->stepNumber < s->numSteps + _catchupSteps) {
    // We've finished animation, wait a little to allow servo to catch up
    s->stepNumber++;
  } else if (s->stepNumber == s->numSteps + _catchupSteps 
            && s->currentPosition != 0) {
    s->numSteps = 0;  // Done now.
  }
}