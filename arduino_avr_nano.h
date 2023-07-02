#ifndef ARDUINO_AVR_NANO_H
#define ARDUINO_AVR_NANO_H

#include <Arduino.h>
#include "globals.h"

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

#if defined(ARDUINO_AVR_NANO)
#define BOARD_TYPE F("Nano")
#elif defined(ARDUINO_AVR_PRO)
#define BOARD_TYPE F("Pro Mini")
#endif
pinDefinition pinMap[TOTAL_PINS] = {
  {2,DIO},{3,DIOP},{4,DIO},{5,DIOP},{6,DIOP},{7,DIO},
  {8,DIO},{9,DIOP},{10,DIOP},{11,DIOP},{12,DIO},{13,DIO},
  {A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},{A6,AI},{A7,AI},
};
#define I2C_SDA A4
#define I2C_SCL A5

pinName pinNameMap[TOTAL_PINS] = {
  {2,"D2"},{3,"D3"},{4,"D4"},{5,"D5"},{6,"D6"},{7,"D7"},
  {8,"D8"},{9,"D9"},{10,"D10"},{11,"D11"},{12,"D12"},{13,"D13"},
  {A0,"A0"},{A1,"A1"},{A2,"A2"},{A3,"A3"},{A6,"A6"},{A7,"A7"},
};

#endif