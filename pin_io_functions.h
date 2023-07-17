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

#ifndef PIN_IO_FUNCTIONS_H
#define PIN_IO_FUNCTIONS_H

#include <Arduino.h>
#include "globals.h"

void setupPinDetails();
void initialisePins();
bool enableDigitalInput(uint8_t pin, bool pullup);
bool writeDigitalOutput(uint8_t pin, bool state);
bool enableAnalogue(uint8_t pin);
bool writeAnalogue(uint8_t pin, uint16_t value, uint8_t profile, uint16_t duration);
// void writePWM(uint8_t pin, uint16_t value);
void processInputs();
bool processOutputTest(bool testState);

#endif