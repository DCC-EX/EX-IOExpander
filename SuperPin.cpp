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

#include <Arduino.h>
#include "SuperPin.h"

/* 
  Variables
*/
static superPinDefinition superPins[MAX_SUPERPINS];
uint8_t superPinCount = 0;
static volatile uint8_t counter = 0;

/*
  Static functions
*/
static inline void handle_interrupts() {
  for (uint8_t i = 0; i < MAX_SUPERPINS; i++) {
    if (superPins[i].isActive) {
      if (counter < superPins[i].onValue) {
        digitalWrite(superPins[i].physicalPin, HIGH);
      } else {
        digitalWrite(superPins[i].physicalPin, LOW);
      }
    }
  }
  counter++;
}

SIGNAL (TIMER2_COMPA_vect) {
  handle_interrupts();
}

static void initISR() {
  TCCR2A = 0x00;
  TCCR2B = (1<<CS21);
  TCNT2 = 0;
  TIFR2 = (1<<OCF2A);   // Clear pending interrupts
  TIMSK2 = (1<<OCIE2A); // Interrupt when hitting OCR2A
  OCR2A = 1;
}

static bool isTimerActive(uint8_t channel) {
  if (superPins[channel].isActive == true) {
    return true;
  } else {
    return false;
  }
}

/*
  Constructor and functions
*/
SuperPin::SuperPin() {
  if (superPinCount < MAX_SUPERPINS) {
    this->superPinIndex = superPinCount++;
  } else {
    this->superPinIndex = INVALID_SUPERPIN;
  }
}

uint8_t SuperPin::attach(uint8_t pin) {
  if (this->superPinIndex < MAX_SUPERPINS) {
    if (isTimerActive(this->superPinIndex) == false) {
      initISR();
      superPins[this->superPinIndex].isActive = true;
      superPins[this->superPinIndex].physicalPin = pin;
      pinMode(superPins[this->superPinIndex].physicalPin, OUTPUT);
    }
  }
  return this->superPinIndex;
}

bool SuperPin::attached() {
  if (superPins[this->superPinIndex].isActive) {
    return true;
  } else {
    return false;
  }
}

void SuperPin::detach() {
  superPins[this->superPinIndex].isActive = false;
}

void SuperPin::write(uint8_t value) {
  uint8_t channel = this->superPinIndex;
  if (channel < MAX_SUPERPINS) {
    if (value < MIN_ON) {
      value = MIN_ON;
    } else if (value > MAX_ON) {
      value = MAX_ON;
    }
    uint8_t oldSREG = SREG;
    cli();
    superPins[channel].onValue = value;
    SREG = oldSREG;
  }
}
