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

// static const uint8_t _catchupSteps = 5;   // number of steps to wait before switching servo off
// const unsigned int refreshInterval = 50;  // refresh every 50ms
// unsigned long lastRefresh = 0;

// Profile for a bouncing signal or turnout
// The profile below is in the range 0-100% and should be combined with the desired limits
// of the servo set by _activePosition and _inactivePosition.  The profile is symmetrical here,
// i.e. the bounce is the same on the down action as on the up action.  First entry isn't used.
// const byte bounceProfile[30] = 
  // {0,2,3,7,13,33,50,83,100,83,75,70,65,60,60,65,74,84,100,83,75,70,70,72,75,80,87,92,97,100};

void updatePosition(uint8_t pin) {
  struct ServoData *s = servoData[pin];
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
      uint8_t profileValue = &bounceProfile[s->stepNumber];
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