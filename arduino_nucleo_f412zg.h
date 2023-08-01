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

pinDefinition pinMap[TOTAL_PINS] = {
  {PC10,DIO},{PC12,DIO},{PF6,DIOP},{PF7,DIOP},{PA15,DIOP},{PB7,DIOP},{PC13,DIO},{PC2,AIDIO},{PC3,AIDIO},{PD4,DIO},{PD5,DIO},{PD6,DIO},{PD7,DIO},{PE3,DIO},
  {PF1,DIO},{PF0,DIO},{PD1,DIO},{PD0,DIO},{PG0,DIO},{PE1,DIO},{PG9,DIO},{PG12,DIO},                                 // CN11 outer pins
  {PC11,DIO},{PD2,DIO},{PA0,AIDIOP},{PA1,AIDIOP},{PA4,AIDIO},{PB0,AIDIOP},{PC1,AIDIO},{PC0,AIDIO},{PD3,DIO},{PG2,DIO},{PG3,DIO},{PE2,DIO},{PE4,DIO},
  {PE5,DIOP},{PF2,DIO},{PF8,DIOP},{PF9,DIOP},{PG1,DIO},{PE6,DIOP},{PG15,DIO},{PG10,DIO},{PG13,DIO},{PG11,DIO},      //CN11 inner pins
  {PC9,DIOP},{PA5,AIDIOP},{PA6,AIDIOP},{PA7,AIDIOP},{PB6,DIOP},{PC7,DIOP},{PB10,DIOP},{PB4,DIOP},{PB5,DIOP},{PB3,DIOP},{PA2,AIDIOP},{PA3,AIDIOP},{PD13,DIOP},
  {PD12,DIOP},{PD11,DIO},{PE10,DIOP},{PE12,DIOP},{PE14,DIOP},{PE15,DIO},{PE13,DIOP},{PF13,DIO},{PF12,DIO},{PG14,DIO},{PD10,DIO},{PG4,DIO},  // CN12 inner pins
  {PC8,DIOP},{PC6,DIOP},{PC5,AIDIO},{PB12,DIO},{PB11,DIOP},{PB2,DIO},{PB1,AIDIOP},{PB15,DIOP},{PB14,DIOP},{PB13,DIOP},{PC4,AIDIO},{PF5,DIO},{PF4,DIO},{PE8,DIOP},
  {PF10,DIO},{PE7,DIO},{PD14,DIOP},{PD15,DIOP},{PF14,DIO},{PE9,DIOP},{PE11,DIOP},{PF3,DIO},{PF15,DIO},{PF11,DIO},{PE0,DIO},{PG8,DIO},{PG5,DIO}, // CN12 outer pins
};

#define I2C_SDA PB9
#define I2C_SCL PB8

pinName pinNameMap[TOTAL_PINS] = {
  {PC10,"PC10"},{PC12,"PC12"},{PF6,"PF6"},{PF7,"PF7"},{PA15,"PA15"},{PB7,"PB7"},{PC13,"PC13"},{PC2,"PC2"},{PC3,"PC3"},{PD4,"PD4"},{PD5,"PD5"},{PD6,"PD6"},{PD7,"PD7"},{PE3,"PE3"},
  {PF1,"PF1"},{PF0,"PF0"},{PD1,"PD1"},{PD0,"PD0"},{PG0,"PG0"},{PE1,"PE1"},{PG9,"PG9"},{PG12,"PG12"},              // CN11 outer pins
  {PC11,"PC11"},{PD2,"PD2"},{PA0,"PA0"},{PA1,"PA1"},{PA4,"PA4"},{PB0,"PB0"},{PC1,"PC1"},{PC0,"PC0"},{PD3,"PD3"},{PG2,"PG2"},{PG3,"PG3"},{PE2,"PE2"},{PE4,"PE4"},
  {PE5,"PE5"},{PF2,"PF2"},{PF8,"PF8"},{PF9,"PF9"},{PG1,"PG1"},{PE6,"PE6"},{PG15,"PG15"},{PG10,"PG10"},{PG13,"PG13"},{PG11,"PG11"},  // CN11 inner pins
  {PC9,"PC9"},{PA5,"PA5"},{PA6,"PA6"},{PA7,"PA7"},{PB6,"PB6"},{PC7,"PC7"},{PB10,"PB10"},{PB4,"PB4"},{PB5,"PB5"},{PB3,"PB3"},{PA2,"PA2"},{PA3,"PA3"},{PD13,"PD13"},
  {PD12,"PD12"},{PD11,"PD11"},{PE10,"PE10"},{PE12,"PE12"},{PE14,"PE14"},{PE15,"PE15"},{PE13,"PE13"},{PF13,"PF13"},{PF12,"PF12"},{PG14,"PG14"},{PD10,"PD10"},{PG4,"PG4"},  // CN12 inner pins
  {PC8,"PC8"},{PC6,"PC6"},{PC5,"PC5"},{PB12,"PB12"},{PB11,"PB11"},{PB2,"PB2"},{PB1,"PB1"},{PB15,"PB15"},{PB14,"PB14"},{PB13,"PB13"},{PC4,"PC4"},{PF5,"PF5"},{PF4,"PF4"},{PE8,"PE8"},
  {PF10,"PF10"},{PE7,"PE7"},{PD14,"PD14"},{PD15,"PD15"},{PF14,"PF14"},{PE9,"PE9"},{PE11,"PE11"},{PF3,"PF3"},{PF15,"PF15"},{PF11,"PF11"},{PE0,"PE0"},{PG8,"PG8"},{PG5,"PG5"},  // CN12 outer pins
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