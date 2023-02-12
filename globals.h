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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "defines.h"

extern pinDefinition pinMap[TOTAL_PINS];
extern pinConfig exioPins[TOTAL_PINS];
extern uint8_t numPins;
extern uint8_t numDigitalPins;
extern uint8_t numAnaloguePins;
extern uint8_t numPWMPins;
extern int analoguePinBytes;
extern int digitalPinBytes;
extern byte* digitalPinStates;
extern byte* analoguePinStates;
extern uint8_t versionBuffer[3];
extern unsigned long displayDelay;
extern uint16_t firstVpin;
extern bool diag;
extern bool setupComplete;
extern uint8_t* analoguePinMap;
extern bool analogueTesting;
extern bool inputTesting;
extern bool outputTesting;
extern bool pullupTesting;
extern ServoData** servoDataArray;

#endif