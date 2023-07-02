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

#ifndef ARDUINO_NUCLEO_F411RE_H
#define ARDUINO_NUCLEO_F411RE_H

#include <Arduino.h>
#include "globals.h"

#define BOARD_TYPE F("NUCLEO-F411RE")

pinDefinition pinMap[TOTAL_PINS] = {
  {PC10,DIO},{PC11,DIO},{PC12,DIO},{PD2,DIO},{PA15,DIOP},{PB7,DIOP},{PC15,DIO},  // CN7
  {PA0,AIDIOP},{PA1,AIDIOP},{PA4,AIDIO},{PB0,AIDIOP},{PC2,AIDIO},{PC1,AIDIO},{PC3,AIDIO},{PC0,AIDIO}, // CN7
  {PC9,DIOP},{PC8,DIOP},{PC6,DIOP},{PC5,AIDIO},{PA5,AIDIOP},{PA12,DIO},{PA6,AIDIOP},{PA11,DIOP},{PA7,DIOP},    // CN10
  {PB12,DIO},{PB6,DIOP},{PC7,DIOP},{PA9,DIOP},{PB2,DIO},{PA8,DIOP},{PB1,AIDIOP},{PB10,DIOP},{PB15,DIOP},   // CN10
  {PB4,DIOP},{PB14,DIOP},{PB5,DIOP},{PB13,DIOP},{PB3,DIOP},{PA10,DIOP},{PC4,AIDIO},                       // CN10
};

#define I2C_SDA PB9
#define I2C_SCL PB8

pinName pinNameMap[TOTAL_PINS] = {
  {PC10,"PC10"},{PC11,"PC11"},{PC12,"PC12"},{PD2,"PD2"},{PA15,"PA15"},{PB7,"PB7"},{PC15,"PC15"},  // CN7
  {PA0,"PA0"},{PA1,"PA1"},{PA4,"PA4"},{PB0,"PB0"},{PC2,"PC2"},{PC1,"PC1"},{PC3,"PC3"},{PC0,"PC0"}, // CN7
  {PC9,"PC9"},{PC8,"PC8"},{PC6,"PC6"},{PC5,"PC5"},{PA5,"PA5"},{PA12,"PA12"},{PA6,"PA6"},{PA11,"PA11"},{PA7,"PA7"},    // CN10
  {PB12,"PB12"},{PB6,"PB6"},{PC7,"PC7"},{PA9,"PA9"},{PB2,"PB2"},{PA8,"PA8"},{PB1,"PB1"},{PB10,"PB10"},{PB15,"PB15"},   // CN10
  {PB4,"PB4"},{PB14,"PB14"},{PB5,"PB5"},{PB13,"PB13"},{PB3,"PB3"},{PA10,"PA10"},{PC4,"PC4"},                       // CN10
};

/*
Pins not available for use:
CN7 (digital) - PA13|14 (debugger),PH0|1 (clock), PC13|14 (used by oscillator)
CN10 (digital) - PB8|9 - (I2C)
CN7 (analogue) - 
Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
*/

#endif