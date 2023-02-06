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

#ifndef ARDUINO_AVR_MEGA_H
#define ARDUINO_AVR_MEGA_H

#include <Arduino.h>
#include "globals.h"

#define BOARD_TYPE F("Mega")
pinDefinition pinMap[TOTAL_PINS] = {
  {2,DIOP},{3,DIOP},{4,DIOP},{5,DIOP},{6,DIOP},{7,DIOP},{8,DIOP},{9,DIOP},{10,DIOP},{11,DIOP},
  {12,DIOP},{13,DIOP},{14,DIO},{15,DIO},{16,DIO},{17,DIO},{18,DIO},{19,DIO},{22,DIO},{23,DIO},
  {24,DIO},{25,DIO},{26,DIO},{27,DIO},{28,DIO},{28,DIO},{30,DIO},{31,DIO},{32,DIO},{33,DIO},
  {34,DIO},{35,DIO},{36,DIO},{37,DIO},{38,DIO},{39,DIO},{40,DIO},{41,DIO},{42,DIO},{43,DIO},
  {44,DIO},{45,DIO},{46,DIO},{47,DIO},{48,DIO},{49,DIO},{A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},
  {A4,AIDIO},{A5,AIDIO},{A6,AIDIO},{A7,AIDIO},{A8,AIDIO},{A9,AIDIO},{A10,AIDIO},{A11,AIDIO},
  {A12,AIDIO},{A13,AIDIO},{A14,AIDIO},{A15,AIDIO},
};

#endif