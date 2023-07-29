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

#ifndef SUPERPIN_H
#define SUPERPIN_H

#include <Arduino.h>
#include "defines.h"

#define INVALID_SUPERPIN 255

#define MIN_ON 0
#define MAX_ON 255

typedef struct {
  uint8_t physicalPin;
  bool isActive;
  uint8_t onValue;
} superPinDefinition;

class SuperPin {
  public:
    SuperPin();
    uint8_t attach(uint8_t pin);
    bool attached();
    void detach();
    void write(uint8_t value);

  private:
    uint8_t superPinIndex;
};

#endif
