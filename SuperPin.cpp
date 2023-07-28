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

#include "SuperPin.h"

SuperPin* SuperPin::firstPin=NULL;


// create a superpin for you to set 
// e.g. SuperPin p=new SuperPin(15);
// then set the pattern when required with p->setPattern(....)
 
SuperPin::SuperPin(byte _pinId) {
  next=firstPin;
  firstPin=this;
  pinId=_pinId;
  onCount=0;
  runningCount=255;
  pinMode(_pinId, OUTPUT);
  pinState=LOW;
  digitalWrite(pinId,pinState); 
}

void SuperPin::setOn(byte _onCount) {
  onCount=_onCount;
  offCount = 255 - onCount;
  runningCount=0;
  pinState=LOW;  
}

void SuperPin::tick() {
  if (runningCount) {
    runningCount--;
    return;
  }
  if (pinState) {
    // pin is HIGH... switch to LOW unless locked
    if (offCount==0) {
      // pin is locked on
      runningCount=onCount;
      return; 
    }
    runningCount=offCount;
    pinState=LOW;
  }
  else {
    // pin is LOW switch to HIGH unless locked 
    if (onCount==0) {
      // pin is locked off
      runningCount=offCount;
      return; 
    }
    runningCount=onCount;
    pinState=HIGH;
  }
  digitalWrite(pinId,pinState);
  runningCount--; 
}

void SuperPin::interrupt() {
    for (SuperPin* p=firstPin; p; p=p->next) p->tick();
}

void SuperPin::start() {

  // OK... have to put some HW specific code here, taken from the CS
  // to make the timer of your choice call interrupt() at the frequency
  // of your choice.  
  // Timer1.attachInterrupt(interrupt,1); 
}
