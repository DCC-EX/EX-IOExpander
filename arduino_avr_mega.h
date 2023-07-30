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

pinDefinition pinMap[TOTAL_PINS] = {
  {2,DIOP},{3,DIOP},{4,DIOP},{5,DIOP},{6,DIOP},{7,DIOP},{8,DIOP},{9,DIOP},{10,DIOP},{11,DIOP},
  {12,DIOP},{13,DIOP},{14,DIO},{15,DIO},{16,DIO},{17,DIO},{18,DIO},{19,DIO},{22,DIO},{23,DIO},
  {24,DIO},{25,DIO},{26,DIO},{27,DIO},{28,DIO},{29,DIO},{30,DIO},{31,DIO},{32,DIO},{33,DIO},
  {34,DIO},{35,DIO},{36,DIO},{37,DIO},{38,DIO},{39,DIO},{40,DIO},{41,DIO},{42,DIO},{43,DIO},
  {44,DIO},{45,DIO},{46,DIO},{47,DIO},{48,DIO},{49,DIO},{A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},
  {A4,AIDIO},{A5,AIDIO},{A6,AIDIO},{A7,AIDIO},{A8,AIDIO},{A9,AIDIO},{A10,AIDIO},{A11,AIDIO},
  {A12,AIDIO},{A13,AIDIO},{A14,AIDIO},{A15,AIDIO},
};

pinName pinNameMap[TOTAL_PINS] = {
  {2,"D2"},{3,"D3"},{4,"D4"},{5,"D5"},{6,"D6"},{7,"D7"},{8,"D8"},{9,"D9"},{10,"D10"},{11,"D11"},
  {12,"D12"},{13,"D13"},{14,"D14"},{15,"D15"},{16,"D16"},{17,"D17"},{18,"D18"},{19,"D19"},{22,"D22"},{23,"D23"},
  {24,"D24"},{25,"D25"},{26,"D26"},{27,"D27"},{28,"D28"},{29,"D29"},{30,"D30"},{31,"D31"},{32,"D32"},{33,"D33"},
  {34,"D34"},{35,"D35"},{36,"D36"},{37,"D37"},{38,"D38"},{39,"D39"},{40,"D40"},{41,"D41"},{42,"D42"},{43,"D43"},
  {44,"D44"},{45,"D45"},{46,"D46"},{47,"D47"},{48,"D48"},{49,"D49"},{A0,"A0"},{A1,"A1"},{A2,"A2"},{A3,"A3"},
  {A4,"A4"},{A5,"A5"},{A6,"A6"},{A7,"A7"},{A8,"A8"},{A9,"A9"},{A10,"A10"},{A11,"A11"},
  {A12,"A12"},{A13,"A13"},{A14,"A14"},{A15,"A15"},
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
Servo servo13;
Servo servo14;
Servo servo15;
Servo servo16;
Servo servo17;
Servo servo18;
Servo servo19;
Servo servo20;
Servo servo21;
Servo servo22;
Servo servo23;
Servo servo24;
Servo servo25;
Servo servo26;
Servo servo27;
Servo servo28;
Servo servo29;
Servo servo30;
Servo servo31;
Servo servo32;
Servo servo33;
Servo servo34;
Servo servo35;
Servo servo36;
Servo servo37;
Servo servo38;
Servo servo39;
Servo servo40;
Servo servo41;
Servo servo42;
Servo servo43;
Servo servo44;
Servo servo45;
Servo servo46;
Servo servo47;
Servo servo48;

Servo servoMap[MAX_SERVOS] = {
  servo1,servo2,servo3,servo4,servo5,servo6,servo7,servo8,servo9,servo10,servo11,servo12,
  servo13,servo14,servo15,servo16,servo17,servo18,servo19,servo20,servo21,servo22,servo23,servo24,
  servo25,servo26,servo27,servo28,servo29,servo30,servo31,servo32,servo33,servo34,servo35,servo36,
  servo37,servo38,servo39,servo40,servo41,servo42,servo43,servo44,servo45,servo46,servo47,servo48,
};

#endif
