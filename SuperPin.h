/*
 *  © 2023, Chris Harlow and Peter Cole. All rights reserved.
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

class SuperPin  {
  public:
    static void setPattern(byte pinId, byte _onCount, byte _offCount);
    static void set(byte pinId, bool high);
    static void loop();
    
  private:
    SuperPin(byte _pinid);
    void setPattern(byte _onCount, byte _offCount);
    void tick();
    static SuperPin* volatile firstPin;
    SuperPin* volatile next;
    volatile byte pinId, onCount, offCount, runningCount;
    volatile bool pinState;
    static void fastDigitalWrite(uint8_t pin, bool state);
};

#endif
