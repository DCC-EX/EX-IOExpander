#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "defines.h"

extern pinDefinition pinMap[TOTAL_PINS];
extern pinConfig exioPins[TOTAL_PINS];
extern uint8_t numPins;
extern uint8_t numDigitalPins;
extern uint8_t numAnaloguePins;
extern uint8_t numPWMPins;
extern int analoguePinBytes;
extern int digitalPinBytes;
extern byte* digitalPinStates;  // Store digital pin states to send to device driver
extern byte* analoguePinStates;
extern uint8_t versionBuffer[3];
extern unsigned long displayDelay;
extern uint16_t firstVpin;
extern bool diag;
extern bool setupComplete;
extern uint8_t* analoguePinMap;

#endif