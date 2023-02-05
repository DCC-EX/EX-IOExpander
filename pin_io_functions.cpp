#include <Arduino.h>
#include "globals.h"
#include "pin_io_functions.h"

pinConfig exioPins[TOTAL_PINS];
byte* digitalPinStates;  // Store digital pin states to send to device driver
byte* analoguePinStates;

/*
* Get the count of analogue and PWM capable pins
*/
void setupPinDetails() {
  for (uint8_t pin = 0; pin < numPins; pin++) {
    if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
      numAnaloguePins++;
    }
    if (bitRead(pinMap[pin].capability, DIGITAL_INPUT) || bitRead(pinMap[pin].capability, DIGITAL_OUTPUT)) {
      numDigitalPins++;
    }
    if (bitRead(pinMap[pin].capability, PWM_OUTPUT)) {
      numPWMPins++;
    }
  }
  analoguePinBytes = numAnaloguePins * 2;
  digitalPinBytes = numDigitalPins / 8;
  digitalPinStates = (byte*) calloc(digitalPinBytes, 1);
  analoguePinStates = (byte*) calloc(analoguePinBytes, 1);
}

/*
* Function to initialise all pins as input and initialise pin struct
*/
void initialisePins() {
  for (uint8_t pin = 0; pin < TOTAL_PINS; pin++) {
    if (bitRead(pinMap[pin].capability, DIGITAL_INPUT) || bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
      pinMode(pinMap[pin].physicalPin, INPUT);
      exioPins[pin].direction = 1;
    } else {
      exioPins[pin].direction = 0;
    }
    exioPins[pin].enable = 0;
    exioPins[pin].mode = 0;
    exioPins[pin].pullup = 0;
  }
  for (uint8_t dPinByte = 0; dPinByte < digitalPinBytes; dPinByte++) {
    digitalPinStates[dPinByte] = 0;
  }
  for (uint8_t aPinByte = 0; aPinByte < analoguePinBytes; aPinByte++) {
    analoguePinStates[aPinByte] = 0;
  }
}








/*
* Function to enable pins as analogue input pins to start reading
*/
void enableAnalogue(uint8_t pin) {
  if (bitRead(pinMap[pin].capability, ANALOGUE_INPUT)) {
    if (exioPins[pin].enable && exioPins[pin].mode != MODE_ANALOGUE && !exioPins[pin].direction) {
      USB_SERIAL.print(F("ERROR! pin "));
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.println(F(" already in use, cannot use as an analogue input pin"));
    }
    exioPins[pin].enable = 1;
    exioPins[pin].mode = MODE_ANALOGUE;
    exioPins[pin].direction = 1;
    pinMode(pinMap[pin].physicalPin, INPUT);
  } else {
    USB_SERIAL.print(F("ERROR! Pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" not capable of analogue input"));
  }
}

/*
* Function to write PWM output to a pin
*/
void writeAnalogue(uint8_t pin, uint16_t value) {
  if (bitRead(pinMap[pin].capability, PWM_OUTPUT)) {
    if (exioPins[pin].enable && (exioPins[pin].direction || exioPins[pin].mode != MODE_PWM)) {
      USB_SERIAL.print(F("ERROR! pin "));
      USB_SERIAL.print(pinMap[pin].physicalPin);
      USB_SERIAL.println(F(" already in use, cannot use as a PWM output pin"));
    } else {
      exioPins[pin].enable = 1;
      exioPins[pin].mode = MODE_PWM;
      exioPins[pin].direction = 0;
      analogWrite(pinMap[pin].physicalPin, value);
    }
  } else {
    USB_SERIAL.print(F("ERROR! Pin "));
    USB_SERIAL.print(pinMap[pin].physicalPin);
    USB_SERIAL.println(F(" not capable of PWM output"));
  }
}