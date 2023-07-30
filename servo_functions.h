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

#ifndef SERVO_FUNCTIONS_H
#define SERVO_FUNCTIONS_H

#include <Arduino.h>
#include "globals.h"

static const uint8_t _catchupSteps = 5;
extern const unsigned int refreshInterval;
extern unsigned long lastRefresh;
const uint8_t bounceProfile[30] = 
  {0,2,3,7,13,33,50,83,100,83,75,70,65,60,60,65,74,84,100,83,75,70,70,72,75,80,87,92,97,100};
#if defined(HAS_SERVO_LIB)
extern uint8_t nextServoObject;
#endif
extern uint8_t nextSuperPinObject;

void processServos();
void updatePosition(uint8_t pin);
bool configureServo(uint8_t pin, bool useSuperPin);
void writeServo(uint8_t pin, uint16_t value, bool useSuperPin);
void setSuperPin(uint8_t pin, uint16_t value);

#endif