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

pinDefinition pinMap[TOTAL_PINS] = {
  {PC10,DIO},{PC12,DIO},{PA15,DIOP},{PB7,DIOP},{PC15,DIO},{PC2,AIDIO},{PC3,AIDIO},                  // CN7 outer pins
  {PC11,DIO},{PD2,DIO}, {PA0,AIDIOP},{PA1,AIDIOP},{PA4,AIDIO},{PB0,AIDIOP},{PC1,AIDIO},{PC0,AIDIO}, // CN7 inner pins
  {PC9,DIOP},{PA5,AIDIOP},{PA6,AIDIOP},{PA7,DIOP},{PB6,DIOP},{PC7,DIOP},{PA9,DIOP},{PA8,DIOP},{PB10,DIOP},{PB4,DIOP},{PB5,DIOP},{PB3,DIOP},{PA10,DIOP}, // CN10 inner pins
  {PC8,DIOP},{PC6,DIOP},{PC5,AIDIO},{PA12,DIO},{PA11,DIOP},{PB12,DIO},{PB2,DIO},{PB1,AIDIOP},{PB15,DIOP},{PB14,DIOP},{PB13,DIOP},{PC4,AIDIO}, // CN10 outer pins
};

#define I2C_SDA PB9
#define I2C_SCL PB8

pinName pinNameMap[TOTAL_PINS] = {
  {PC10,"PC10"},{PC12,"PC12"},{PA15,"PA15"},{PB7,"PB7"},{PC15,"PC15"},{PC2,"PC2"},{PC3,"PC3"},        // CN7 outer pins
  {PC11,"PC11"},{PD2,"PD2"},{PA0,"PA0"},{PA1,"PA1"},{PA4,"PA4"},{PB0,"PB0"},{PC1,"PC1"},{PC0,"PC0"},  // CN7 inner pins
  {PC9,"PC9"},{PA5,"PA5"},{PA6,"PA6"},{PA7,"PA7"},{PB6,"PB6"},{PC7,"PC7"},{PA9,"PA9"},{PA8,"PA8"},{PB10,"PB10"},{PB4,"PB4"},{PB5,"PB5"},{PB3,"PB3"},{PA10,"PA10"},  // CN10 inner pins
  {PC8,"PC8"},{PC6,"PC6"},{PC5,"PC5"},{PA12,"PA12"},{PA11,"PA11"},{PB12,"PB12"},{PB2,"PB2"},{PB1,"PB1"},{PB15,"PB15"},{PB14,"PB14"},{PB13,"PB13"},{PC4,"PC4"},  // CN10 outer pins
};

/*
Pins not available for use:
CN7 (digital) - PA13|14 (debugger),PH0|1 (clock), PC13|14 (used by oscillator)
CN10 (digital) - PB8|9 - (I2C)
CN7 (analogue) - 
Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
*/

#endif