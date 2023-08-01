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

#include <Arduino.h>
#include "SuperPin.h"

SuperPin* volatile SuperPin::firstPin=NULL;


// create a superpin for you to set 
// e.g. SuperPin p=new SuperPin(15);
// then set the pattern when required with p->setPattern(....)
 
SuperPin::SuperPin(byte _pinId) {
  pinId=_pinId;
  onCount=0;
  offCount=255;
  runningCount=255;
  pinMode(_pinId, OUTPUT);
  pinState=LOW;
  digitalWrite(pinId,pinState);
  
  // chain in the new pin
  noInterrupts();
  next=firstPin;
  firstPin=this;
  interrupts(); 
}

// Call this to set a pattern of on/off
// setPattern(25,100) low frequency PWM 20% (25 on, 100 off)
// setPattern(1,4) high frequency PWM 20% (1 on, 4 off)
// and so on... 
  
void SuperPin::setPattern(byte _onCount, byte _offCount) {
  noInterrupts();
  onCount=_onCount;
  offCount=_offCount;
  runningCount=0;
  // pinState=LOW;
  pinState=_offCount?LOW:HIGH;
  fastDigitalWrite(pinId, pinState);
  interrupts();  
}

// Set a pin to be HIGH or LOW
void SuperPin::set(byte _pinId, bool _high) {
  if (_high) setPattern(_pinId,255,0);
  else setPattern(_pinId,0,255);
}

// Set a pin to be a PWM pattern 
void SuperPin::setPattern(byte _pinId, byte _onCount, byte _offCount) {
  for ( SuperPin* volatile p=firstPin; p; p=p->next){
    if (p->pinId==_pinId) {
      p->setPattern(_onCount, _offCount);
      return;
    }
  }
  (new SuperPin(_pinId))->setPattern(_onCount,_offCount);     
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
  fastDigitalWrite(pinId, pinState);
  runningCount--; 
}


// EITHER - call this loop() function at a suitable frequency from your
//  sketch loop() 
// OR use a timer of your choice call loop() at the frequency
// of your choice.  
// e.g. Timer1.attachInterrupt(SuperPin::loop,freq); 

void SuperPin::loop() {
  for (SuperPin* volatile p=firstPin; p; p=p->next) p->tick();
}

// Fast digital writes stolen from EX-CommandStation
void SuperPin::fastDigitalWrite(uint8_t pin, bool state) {
#ifdef USE_FAST_WRITES
  if (pin >= NUM_DIGITAL_PINS) return;
  uint8_t mask = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *outPortAdr = portOutputRegister(port);
  noInterrupts();
  if (state) 
    *outPortAdr |= mask;
  else
    *outPortAdr &= ~mask;
  interrupts();
#else
  digitalWrite(pin, state);
#endif
}
