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

#ifndef ARDUINO_BLUEPILL_F103C8_H
#define ARDUINO_BLUEPILL_F103C8_H

#include <Arduino.h>
#include "globals.h"

pinDefinition pinMap[TOTAL_PINS] = {
  {PC13,DIO},{PC14,DIO},{PC15,DIO},{PA0,AIDIO},{PA1,AIDIOP},{PA2,AIDIOP},{PA3,AIDIOP},{PA4,AIDIO},
  {PA5,AIDIO},{PA6,AIDIOP},{PA7,AIDIOP},{PB0,AIDIOP},{PB1,AIDIOP},{PB10,DIOP},{PB11,DIOP},
  {PB9,DIO},{PB8,DIO},{PB5,DIOP},{PB4,DIOP},{PB3,DIOP},{PA15,DIOP},
  {PA10,DIOP},{PA9,DIOP},{PA8,DIOP},{PB15,DIOP},{PB14,DIOP},{PB13,DIOP},{PB12,DIO},
};
#define I2C_SDA PB7
#define I2C_SCL PB6

pinName pinNameMap[TOTAL_PINS] = {
  {PC13,"PC13"},{PC14,"PC14"},{PC15,"PC15"},{PA0,"PA0"},{PA1,"PA1"},{PA2,"PA2"},{PA3,"PA3"},{PA4,"PA4"},
  {PA5,"PA5"},{PA6,"PA6"},{PA7,"PA7"},{PB0,"PB0"},{PB1,"PB1"},{PB10,"PB10"},{PB11,"PB11"},
  {PB9,"PB9"},{PB8,"PB8"},{PB5,"PB5"},{PB4,"PB4"},{PB3,"PB3"},{PA15,"PA15"},
  {PA10,"PA10"},{PA9,"PA9"},{PA8,"PA8"},{PB15,"PB15"},{PB14,"PB14"},{PB13,"PB13"},{PB12,"PB12"},
};

/*
Pins not available for use:
PB7|PB6 (I2C)
PA11|PA12 (USB)

Buttons/LEDs:
Note that PC13 has an LED
*/

#endif