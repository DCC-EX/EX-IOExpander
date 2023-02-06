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

#ifndef ARDUINO_NUCLEO_F412ZG_H
#define ARDUINO_NUCLEO_F412ZG_H

#include <Arduino.h>
#include "globals.h"

#define BOARD_TYPE F("NUCLEO-F412ZG")

pinDefinition pinMap[TOTAL_PINS] = {
  {2,DIOP},{3,DIOP},{4,DIOP},{5,DIOP},{6,DIOP},{7,DIOP},{8,DIOP},{9,DIOP},{10,DIOP},{11,DIOP},
  {12,DIOP},{13,DIOP},{14,DIO},{15,DIO},{16,DIO},{17,DIO},{18,DIO},{19,DIO},{22,DIO},{23,DIO},
  {24,DIO},{25,DIO},{26,DIO},{27,DIO},{28,DIO},{28,DIO},{30,DIO},{31,DIO},{32,DIO},{33,DIO},
  {34,DIO},{35,DIO},{36,DIO},{37,DIO},{38,DIO},{39,DIO},{40,DIO},{41,DIO},{42,DIO},{43,DIO},
  {44,DIO},{45,DIO},{46,DIO},{47,DIO},{48,DIO},{49,DIO},{A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},
  {A4,AIDIO},{A5,AIDIO},{A6,AIDIO},{A7,AIDIO},{A8,AIDIO},{A9,AIDIO},{A10,AIDIO},{A11,AIDIO},
  {A12,AIDIO},{A13,AIDIO},{A14,AIDIO},{A15,AIDIO},
};

#define I2C_SDA PB9
#define I2C_SCL PB8

/*
Pins not available for use:
CN11 (digital) - PB_8|9 (I2C),PD_9|8 (serial),PH_0|1 (clock),PA_13|14,PC_14|15
CN12 (digital) - PA_8|9|10|11|12 (USB OTG),PG_6|7 (USB OTG)
CN11 (analogue) - 
CN12 (analogue) - 
Buttons/LEDs:
Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
PB0 is the Green LED
PB7 is the Blue LED
PB14 is the Red LED
*/

// static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
//   PC10,PC11,PC12,PD2,PF6,PF7,PA15,PB7,PC13,
//   PD4,PD3,PD5,PG2,PD6,PG3,PD7,PE2,PE3,PE4,PE5,PF1,PF2,PF0,
//   PF8,PD1,PF9,PD0,PG1,PG0,PE1,PE6,PG9,PG15,PG12,PG10,PG13,PG11,    // CN11 digital
//   PC9,PC8,PC6,PB12,PB6,PB11,PC7,
//   PB2,PB10,PB15,PB4,PB14,PB5,PB13,PB3,PF5,PF4,PE8,
//   PD13,PF10,PD12,PE7,PD11,PD14,PE10,PD15,PE12,PF14,PE14,PE9,PE15,PE13,PE11,
//   PF13,PF3,PF12,PF15,PG14,PF11,PE0,PD10,PG8,PG5,PG4, // CN12 digital
//   PA0,PA1,PA4,PB0,PC2,PC1,PC3,PC0,   // CN11 analogue
//   PC5,PA5,PA6,PA7,PB1,PC4,PA2,PA3   // CN12 analogue
// };
// static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
//   PA0,PA1,PA4,PB0,PC2,PC1,PC3,PC0,   // CN11 analogue
//   PC5,PA5,PA6,PA7,PB1,PC4,PA2,PA3   // CN12 analogue
// };
// #define I2C_SDA PB9
// #define I2C_SCL PB8

#endif