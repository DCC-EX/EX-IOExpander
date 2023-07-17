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

#ifndef EXIO_DIMMER_H
#define EXIO_DIMMER_H

#include <Arduino.h>

#if !defined(ARDUINO_ARCH_AVR)
#error "This library only works with AVR."
#endif

#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_UNO)
#define MAX_DIMMERS 16
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#define MAX_DIMMERS 62
#endif
#define INVALID_DIMMER 255

#define MIN_ON 0
#define MAX_ON 255

typedef struct {
  uint8_t physicalPin;
  bool isActive;
  uint8_t onValue;
} dimmerDefinition;

class EXIODimmer {
  public:
    EXIODimmer();
    uint8_t attach(uint8_t pin);
    bool attached();
    void detach();
    void write(uint8_t value);

  private:
    uint8_t dimmerIndex;
};

#endif