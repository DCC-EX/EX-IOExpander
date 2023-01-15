/*
 *  © 2022 Peter Cole. All rights reserved.
 *
 *  This file is part of EX-IOExpander.
 *  
 *  This file contains the definitions required for supported devices and should not
 *  need to be edited.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 * 
 */
#ifndef SUPPORTEDDEVICES_H
#define SUPPORTEDDEVICES_H

#include <Arduino.h>

/////////////////////////////////////////////////////////////////////////////////////
//  Defines for the supported hardware architectures
//
//  When adding new boards/architectures, the digital pin map must contain every pin
//  capable of being a digital input/output device, including all relevant analogue pins.
//  When the CommandStation configures EX-IOExpander, it will set whether the pin is in
//  digital or analogue mode based on the number of digital/analogue pins.

// Arduino Uno
#if defined(ARDUINO_AVR_UNO)
#define BOARD_TYPE F("Uno")
#define HAS_EEPROM
#define NUMBER_OF_DIGITAL_PINS 12   // D2 - D13
#define NUMBER_OF_ANALOGUE_PINS 4     // A0 - A3, cannot use A4/A5
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  2,3,4,5,6,7,8,9,10,11,12,13,A0,A1,A2,A3
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  A0,A1,A2,A3
};

// Arduino Nano or Pro Mini
#elif defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_PRO)
#if defined(ARDUINO_AVR_NANO)
#define BOARD_TYPE F("Nano")
#elif defined(ARDUINO_AVR_PRO)
#define BOARD_TYPE F("Pro Mini")
#endif
#define HAS_EEPROM
#define NUMBER_OF_DIGITAL_PINS 12   // D2 - D13
#define NUMBER_OF_ANALOGUE_PINS 6     // A0 - A3, A6/A7, cannot use A4/A5
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  2,3,4,5,6,7,8,9,10,11,12,13,A0,A1,A2,A3
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  A0,A1,A2,A3,A6,A7
};

// Arduino Mega/Mega2560
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#define BOARD_TYPE F("Mega")
#define HAS_EEPROM
#define NUMBER_OF_DIGITAL_PINS 46   // D2 - D19, D22 - D49
#define NUMBER_OF_ANALOGUE_PINS 16     // A0 - A15
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,22,23,24,25,26,27,28,29,
  30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,
  A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15
};

// NUCLEO F411RE - EXPERIMENTAL, in need of testing
// Pins not available for use:
// CN7 (digital) - PA_13|14 used for debugger, PB_7 - used for I2C, PA_15 conflict
// CN10 (digital) - PB_4|3 seem to have conflicts as inputs, PB_8|9|6 - used for I2C
// CN7 (analogue) - PA_0|1 seem to have conflicts as inputs
#elif defined(ARDUINO_NUCLEO_F411RE)
#define BOARD_TYPE F("NUCLEO-F411RE")
#define NUMBER_OF_DIGITAL_PINS 25
#define NUMBER_OF_ANALOGUE_PINS 12
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  PC_10,PC_11,PC_12,PD_2,PC_13,PC_14,PC_15,PH_0,PH_1,    // CN7 digital
  PC_9,PC_8,PC_6,PA_12,PA_11,PB_12,PC_7,PA_9,PB_2,PA_8,PB_10,PB_15,PB_14,PB_5,PB_13,PA_10, // CN10 digital
  PA_4,PB_0,PC_2,PC_1,PC_3,PC_0,   // CN7 analogue
  PC_5,PA_5,PA_6,PA_7,PB_1,PC_4   // CN10 analogue
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  PA_4,PB_0,PC_2,PC_1,PC_3,PC_0,   // CN7 analogue
  PC_5,PA_5,PA_6,PA_7,PB_1,PC_4   // CN10 analogue
};

// NUCLEO F412ZG - EXPERIMENTAL, in need of testing
// Pins not available for use:
// CN11 (digital) - PB_8|PB_9 - used for I2C
// CN12 (digital) - 
// CN11 (analogue) - 
// CN12 (analogue) - 
#elif defined(ARDUINO_NUCLEO_F412ZG)
#define BOARD_TYPE F("NUCLEO-F412ZG")
#define NUMBER_OF_DIGITAL_PINS 94
#define NUMBER_OF_ANALOGUE_PINS 16
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  PC_10,PC_11,PC_12,PD_2,PF_6,PF_7,PA_13,PA_14,PA_15,PB_7,PC_13,PH_1,
  PD_4,PD_3,PD_5,PG_2,PD_6,PG_3,PD_7,PE_2,PE_3,PE_4,PE_5,PF_1,PF_2,PF_0,
  PF_8,PD_1,PF_9,PD_0,PG_1,PG_0,PE_1,PE_6,PG_9,PG_15,PG_12,PG_10,PH_2,PG_13,PD_9,PG_11,    // CN11 digital
  PC_9,PC_8,PC_6,PD_8,PA_12,PA_11,PB_12,PB_6,PB_11,PC_7,PA_9,
  PB_2,PA_8,PB_10,PB_15,PB_4,PB_15,PB_5,PB_13,PB_3,PA_10,PF_5,PF_4,PE_8,
  PD_13,PF_10,PD_12,PE_7,PD_11,PD_14,PE_10,PD_15,PE_12,PF_14,PE_14,PE_9,PE_15,PE_13,PE_11,
  PF_13,PF_3,PF_12,PF_15,PG_14,PF_11,PE_0,PD_10,PG_8,PG_7,PG_5,PG_4,PG_6, // CN12 digital
  PA_0,PA_1,PA_4,PB_0,PC_2,PC_1,PC_3,PC_0,   // CN11 analogue
  PC_5,PA_5,PA_6,PA_7,PB_1,PC_4,PA_2,PA_3,   // CN12 analogue
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  PA_0,PA_1,PA_4,PB_0,PC_2,PC_1,PC_3,PC_0,   // CN11 analogue
  PC_5,PA_5,PA_6,PA_7,PB_1,PC_4,PA_2,   // CN12 analogue
};

#else
#define CPU_TYPE_ERROR
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  Define the register hex values we need to act on or respond with
//
#define EXIOINIT 0xE0     // Flag to start setup procedure
#define EXIORDY 0xE1      // Flag setup procedure done, return to CS to ACK
#define EXIODPUP 0xE2     // Flag we're receiving digital pin pullup configuration
#define EXIOVER 0xE3      // Flag to send version
#define EXIORDAN 0xE4     // Flag an analogue input is being read
#define EXIOWRD 0xE5      // Flag for digital write
#define EXIORDD 0xE6      // Flag a digital input is being read

/////////////////////////////////////////////////////////////////////////////////////
//  Define version to store in EEPROM/FLASH in case this needs to change later
//  This needs to be defined in order to invalidate contents if the structure changes
//  in future releases.
//
#define EEPROM_VERSION 1
#define FLASH_VERSION 1

#endif