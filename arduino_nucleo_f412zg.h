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
  {PC10,DIO},{PC11,DIO},{PC12,DIO},{PD2,DIO},{PF6,DIOP},{PF7,DIOP},{PA15,DIOP},{PB7,DIOP},{PC13,DIO},         // CN11
  {PA0,AIDIOP},{PA1,AIDIOP},{PA4,AIDIO},{PB0,AIDIOP},{PC2,AIDIO},{PC1,AIDIO},{PC3,AIDIO},{PC0,AIDIO},         // CN11
  {PD4,DIO},{PD3,DIO},{PD5,DIO},{PG2,DIO},{PD6,DIO},{PG3,DIO},{PD7,DIO},{PE2,DIO},{PE3,DIO},{PE4,DIO},        // CN11
  {PE5,DIOP},{PF1,DIO},{PF2,DIO},{PF0,DIO},{PF8,DIOP},{PD1,DIO},{PF9,DIOP},{PD0,DIO},{PG1,DIO},{PG0,DIO},     // CN11
  {PE1,DIO},{PE6,DIOP},{PG9,DIO},{PG15,DIO},{PG12,DIO},{PG10,DIO},{PG13,DIO},{PG11,DIO},                      // CN11
  {PC9,DIOP},{PC8,DIOP},{PC6,DIOP},{PC5,AIDIO},{PA5,AIDIOP},{PA6,AIDIOP},{PA7,AIDIOP},{PB12,DIO},{PB6,DIOP},  // CN12
  {PB11,DIOP},{PC7,DIOP},{PB2,DIO},{PB1,AIDIOP},{PB10,DIOP},{PB15,DIOP},{PB4,DIOP},{PB14,DIOP},{PB5,DIOP},    // CN12
  {PB13,DIOP},{PB3,DIOP},{PC4,AIDIO},{PA2,AIDIOP},{PF5,DIO},{PA3,AIDIOP},{PF4,DIO},{PE8,DIOP},{PD13,DIOP},    // CN12
  {PF10,DIO},{PD12,DIOP},{PE7,DIO},{PD11,DIO},{PD14,DIOP},{PE10,DIOP},{PD15,DIOP},{PE12,DIOP},{PF14,DIO},     // CN12
  {PE14,DIOP},{PE9,DIOP},{PE15,DIO},{PE13,DIOP},{PE11,DIOP},{PF13,DIO},{PF3,DIO},{PF12,DIO},{PF15,DIO},       // CN12
  {PG14,DIO},{PF11,DIO},{PE0,DIO},{PD10,DIO},{PG8,DIO},{PG5,DIO},{PG4,DIO},                                   // CN12
};

#define I2C_SDA PB9
#define I2C_SCL PB8

/*
Pins not available for use:
CN11 (digital) - PB_8|9 (I2C),PD_9|8 (serial),PH_0|1 (clock),PA_13|14,PC_14|15, PH_2 (N/C)
CN12 (digital) - PA_8|9|10|11|12 (USB OTG),PG_6|7 (USB OTG)
CN11 (analogue) - 
CN12 (analogue) - 
Buttons/LEDs:
Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
PB0 is the Green LED
PB7 is the Blue LED
PB14 is the Red LED
*/

#endif