/*
 *  © 2023, Chris Harlow.
 *  © 2023, Peter Cole.
 *
 *  All rights reserved.
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

#ifndef SuperPin_h
#define SuperPin_h

#include <Arduino.h>

// The variables all need to be marked volatile because they may be accessed by loop and interrupt code.
// The constructor and setpattern routines should really use nointerrupt() ... or there is a small risk of lost values. 
// It would be worth creating a static setPattern(pinid, oncount, offcount)  so the caller didn't have to remember the pointers.

class SuperPin  {
    public:
      static void start();
      SuperPin(byte _pinid);
      void setOn(byte _onCount);
      static void interrupt();

    private:
      // static void interrupt();
      void tick();
      static SuperPin* firstPin;
      SuperPin* next;
      volatile byte onCount, offCount, runningCount;
      volatile bool pinState;
      volatile byte pinId;
};
#endif
