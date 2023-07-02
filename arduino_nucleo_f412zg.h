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

pinName pinNameMap[TOTAL_PINS] = {
  {PC10,"PC10"},{PC11,"PC11"},{PC12,"PC12"},{PD2,"PD2"},{PF6,"PF6"},{PF7,"PF7"},{PA15,"PA15"},{PB7,"PB7"},                        // CN11
  {PC13,"PC13"},{PA0,"PA0"},{PA1,"PA1"},{PA4,"PA4"},{PB0,"PB0"},{PC2,"PC2"},{PC1,"PC1"},{PC3,"PC3"},{PC0,"PC0"},                  // CN11
  {PD4,"PD4"},{PD3,"PD3"},{PD5,"PD5"},{PG2,"PG2"},{PD6,"PD6"},{PG3,"PG3"},{PD7,"PD7"},{PE2,"PE2"},{PE3,"PE3"},{PE4,"PE4"},        // CN11
  {PE5,"PE5"},{PF1,"PF1"},{PF2,"PF2"},{PF0,"PF0"},{PF8,"PF8"},{PD1,"PD1"},{PF9,"PF9"},{PD0,"PD0"},{PG1,"PG1"},{PG0,"PG0"},        // CN11
  {PE1,"PE1"},{PE6,"PE6"},{PG9,"PG9"},{PG15,"PG15"},{PG12,"PG12"},{PG10,"PG10"},{PG13,"PG13"},{PG11,"PG11"},                      // CN11
  {PC9,"PC9"},{PC8,"PC8"},{PC6,"PC6"},{PC5,"PC5"},{PA5,"PA5"},{PA6,"PA6"},{PA7,"PA7"},{PB12,"PB12"},{PB6,"PB6"},                  // CN12
  {PB11,"PB11"},{PC7,"PC7"},{PB2,"PB2"},{PB1,"PB1"},{PB10,"PB10"},{PB15,"PB15"},{PB4,"PB4"},{PB14,"PB14"},{PB5,"PB5"},            // CN12
  {PB13,"PB13"},{PB3,"PB3"},{PC4,"PC4"},{PA2,"PA2"},{PF5,"PF5"},{PA3,"PA3"},{PF4,"PF4"},{PE8,"PE8"},{PD13,"PD13"},                // CN12
  {PF10,"PF10"},{PD12,"PD12"},{PE7,"PE7"},{PD11,"PD11"},{PD14,"PD14"},{PE10,"PE10"},{PD15,"PD15"},{PE12,"PE12"},{PF14,"PF14"},    // CN12
  {PE14,"PE14"},{PE9,"PE9"},{PE15,"PE15"},{PE13,"PE13"},{PE11,"PE11"},{PF13,"PF13"},{PF3,"PF3"},{PF12,"PF12"},{PF15,"PF15"},      // CN12
  {PG14,"PG14"},{PF11,"PF11"},{PE0,"PE0"},{PD10,"PD10"},{PG8,"PG8"},{PG5,"PG5"},{PG4,"PG4"},                                      // CN12
};

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