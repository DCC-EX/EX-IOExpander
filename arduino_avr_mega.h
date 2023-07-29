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

// SuperPin support here
SuperPin sPin1;
SuperPin sPin2;
SuperPin sPin3;
SuperPin sPin4;
SuperPin sPin5;
SuperPin sPin6;
SuperPin sPin7;
SuperPin sPin8;
SuperPin sPin9;
SuperPin sPin10;
SuperPin sPin11;
SuperPin sPin12;
SuperPin sPin13;
SuperPin sPin14;
SuperPin sPin15;
SuperPin sPin16;
SuperPin sPin17;
SuperPin sPin18;
SuperPin sPin19;
SuperPin sPin20;
SuperPin sPin21;
SuperPin sPin22;
SuperPin sPin23;
SuperPin sPin24;
SuperPin sPin25;
SuperPin sPin26;
SuperPin sPin27;
SuperPin sPin28;
SuperPin sPin29;
SuperPin sPin30;
SuperPin sPin31;
SuperPin sPin32;
SuperPin sPin33;
SuperPin sPin34;
SuperPin sPin35;
SuperPin sPin36;
SuperPin sPin37;
SuperPin sPin38;
SuperPin sPin39;
SuperPin sPin40;
SuperPin sPin41;
SuperPin sPin42;
SuperPin sPin43;
SuperPin sPin44;
SuperPin sPin45;
SuperPin sPin46;
SuperPin sPin47;
SuperPin sPin48;
SuperPin sPin49;
SuperPin sPin50;
SuperPin sPin51;
SuperPin sPin52;
SuperPin sPin53;
SuperPin sPin54;
SuperPin sPin55;
SuperPin sPin56;
SuperPin sPin57;
SuperPin sPin58;
SuperPin sPin59;
SuperPin sPin60;
SuperPin sPin61;
SuperPin sPin62;

SuperPin superPinMap[MAX_SUPERPINS] = {
  sPin1,sPin2,sPin3,sPin4,sPin5,sPin6,sPin7,sPin8,sPin9,sPin10,sPin11,sPin12,
  sPin13,sPin14,sPin15,sPin16,sPin17,sPin18,sPin19,sPin20,sPin21,sPin22,sPin23,sPin24,
  sPin25,sPin26,sPin27,sPin28,sPin29,sPin30,sPin31,sPin32,sPin33,sPin34,sPin35,sPin36,
  sPin37,sPin38,sPin39,sPin40,sPin41,sPin42,sPin43,sPin44,sPin45,sPin46,sPin47,sPin48,
  sPin49,sPin50,sPin51,sPin52,sPin53,sPin54,sPin55,sPin56,sPin57,sPin58,sPin59,sPin60,
  sPin61,sPin62,
};

#endif