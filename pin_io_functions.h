#ifndef PIN_IO_FUNCTIONS_H
#define PIN_IO_FUNCTIONS_H

#include <Arduino.h>
#include "globals.h"

void setupPinDetails();
void initialisePins();
void enableAnalogue(uint8_t pin);
void writeAnalogue(uint8_t pin, uint16_t value);

#endif