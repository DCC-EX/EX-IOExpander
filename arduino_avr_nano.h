#ifndef ARDUINO_AVR_NANO_H
#define ARDUINO_AVR_NANO_H

#include <Arduino.h>
#include "globals.h"

#if defined(ARDUINO_AVR_NANO)
#define BOARD_TYPE F("Nano")
#elif defined(ARDUINO_AVR_PRO)
#define BOARD_TYPE F("Pro Mini")
#endif
pinDefinition pinMap[TOTAL_PINS] = {
  {2,DIO},{3,DIOP},{4,DIO},{5,DIOP},{6,DIOP},{7,DIO},
  {8,DIO},{9,DIOP},{10,DIOP},{11,DIOP},{12,DIO},{13,DIO},
  {A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},{A6,AI},{A7,AI},
};
#define I2C_SDA A4
#define I2C_SCL A5

#endif