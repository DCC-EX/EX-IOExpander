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
  {PC10,DIOP},{PC11,DIOP},{PC12,DIOP},{PD2,DIOP},{PA15,DIOP},{PB7,DIOP},{PC13,DIOP},{PC14,DIOP},  // CN7
  {PC15,DIOP},{PA0,DIOP},{PA1,DIOP},{PA4,DIOP},{PB0,DIO},{PC2,DIO},{PC1,DIO},{PC3,DIO},{PC0,DIO}, // CN7
  {PC9,DIO},{PC8,DIO},{PC6,DIO},{PC5,DIO},{PA5,DIO},{PA12,DIO},{PA6,DIO},{PA11,DIO},{PA7,DIO},    // CN10
  {PB12,DIO},{PB6,DIO},{PC7,DIO},{PA9,DIO},{PB2,DIO},{PA8,DIO},{PB1,DIO},{PB10,DIO},{PB15,DIO},   // CN10
  {PB4,DIO},{PB14,DIO},{PB5,DIO},{PB13,DIO},{PB3,DIO},{PA10,DIO},{PC4,DIO},                       // CN10
};

#define I2C_SDA PB9
#define I2C_SCL PB8

/*
Pins not available for use:
CN7 (digital) - PA13|14 (debugger),PH0|1 (clock)
CN10 (digital) - PB8|9 - (I2C)
CN7 (analogue) - 
Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
*/

// Why no PC13|PC14? adding these, total pins + 2?

#endif