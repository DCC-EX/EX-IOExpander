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

#ifndef ARDUINO_AVR_UNO_H
#define ARDUINO_AVR_UNO_H

#include <Arduino.h>
#include "globals.h"

pinDefinition pinMap[TOTAL_PINS] = {
  {2,DIO},{3,DIOP},{4,DIO},{5,DIOP},{6,DIOP},{7,DIO},
  {8,DIO},{9,DIOP},{10,DIOP},{11,DIOP},{12,DIO},{13,DIO},
  {A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},
};
#define I2C_SDA A4
#define I2C_SCL A5

pinName pinNameMap[TOTAL_PINS] = {
  {2,"D2"},{3,"D3"},{4,"D4"},{5,"D5"},{6,"D6"},{7,"D7"},
  {8,"D8"},{9,"D9"},{10,"D10"},{11,"D11"},{12,"D12"},{13,"D13"},
  {A0,"A0"},{A1,"A1"},{A2,"A2"},{A3,"A3"},
};

// Servo support here
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;
Servo servo8;
Servo servo9;
Servo servo10;
Servo servo11;
Servo servo12;

Servo servoMap[MAX_SERVOS] = {
  servo1, servo2, servo3, servo4, servo5, servo6, servo7, servo8, servo9, servo10, servo11, servo12,
};

#endif
