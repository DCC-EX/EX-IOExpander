#ifndef I2C_FUNCTIONS_H
#define I2C_FUNCTIONS_H

#include <Arduino.h>
#include "globals.h"

void receiveEvent(int numBytes);
void requestEvent();

#endif