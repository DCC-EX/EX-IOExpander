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

#ifndef DEVICE_FUNCTIONS_H
#define DEVICE_FUNCTIONS_H

#include <Arduino.h>
#include "globals.h"

uint8_t getI2CAddress();
void writeI2CAddress(int16_t eepromAddress);
void eraseI2CAddress();
void reset();
#if defined(ARDUINO_BLUEPILL_F103C8)
void disableJTAG();
#endif

#endif