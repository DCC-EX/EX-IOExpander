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

// Arduino Nano or Pro Mini
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_PRO)
#if defined(ARDUINO_AVR_NANO)
#define BOARD_TYPE F("Nano")
#elif defined(ARDUINO_AVR_PRO)
#define BOARD_TYPE F("Pro Mini")
#endif
#define TOTAL_PINS 18
#define NUMBER_OF_ANALOGUE_PINS = 6
pinDefinition pinMap[TOTAL_PINS] = {
  {D2,DIO},{D3,DIOPO},{D4,DIO},{D5,DIOPO},{D6,DIOPO},{D7,DIO},
  {D8,DIO},{D9,DIOPO},{D10,DIOPO},{D11,DIOPO},{D12,DIO},{D13,DIO},
  {A0,AIDIO},{A1,AIDIO},{A2,AIDIO},{A3,AIDIO},{A6,AI},{A7,AI},
};
#define I2C_SDA A4
#define I2C_SCL A5

/*
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
#define I2C_SDA A4
#define I2C_SCL A5

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
#define I2C_SDA A4
#define I2C_SCL A5

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

// NUCLEO F411RE
// Pins not available for use:
// CN7 (digital) - PA13|14 (debugger),PH0|1 (clock)
// CN10 (digital) - PB8|9 - (I2C)
// CN7 (analogue) - 
// Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
#elif defined(ARDUINO_NUCLEO_F411RE)
#define BOARD_TYPE F("NUCLEO-F411RE")
#define NUMBER_OF_DIGITAL_PINS 26
#define NUMBER_OF_ANALOGUE_PINS 14
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  PC10,PC11,PC12,PD2,PA15,PB7,PC13,    // CN7 digital
  PC9,PC8,PC6,PA12,PA11,PB12,PB6,PC7,PA9,PB2,PA8,PB10,PB15,PB4,PB14,PB5,PB13,PB3,PA10, // CN10 digital
  PA0,PA1,PA4,PB0,PC2,PC1,PC3,PC0,   // CN7 analogue
  PC5,PA5,PA6,PA7,PB1,PC4   // CN10 analogue
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  PA0,PA1,PA4,PB0,PC2,PC1,PC3,PC0,   // CN7 analogue
  PC5,PA5,PA6,PA7,PB1,PC4   // CN10 analogue
};
#define I2C_SDA PB9
#define I2C_SCL PB8

// NUCLEO F412ZG - EXPERIMENTAL, in need of testing
// Pins not available for use:
// CN11 (digital) - PB_8|9 (I2C),PD_9|8 (serial),PH_0|1 (clock),PA_13|14,PC_14|15
// CN12 (digital) - PA_8|9|10|11|12 (USB OTG),PG_6|7 (USB OTG)
// CN11 (analogue) - 
// CN12 (analogue) - 
// Buttons/LEDs:
// Note that PC13 has a switch (blue button) with pullup, so input-only unless you disconnect SB17
// PB0 is the Green LED
// PB7 is the Blue LED
// PB14 is the Red LED
#elif defined(ARDUINO_NUCLEO_F412ZG)
#define BOARD_TYPE F("NUCLEO-F412ZG")
#define NUMBER_OF_DIGITAL_PINS 81
#define NUMBER_OF_ANALOGUE_PINS 16
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  PC10,PC11,PC12,PD2,PF6,PF7,PA15,PB7,PC13,
  PD4,PD3,PD5,PG2,PD6,PG3,PD7,PE2,PE3,PE4,PE5,PF1,PF2,PF0,
  PF8,PD1,PF9,PD0,PG1,PG0,PE1,PE6,PG9,PG15,PG12,PG10,PG13,PG11,    // CN11 digital
  PC9,PC8,PC6,PB12,PB6,PB11,PC7,
  PB2,PB10,PB15,PB4,PB14,PB5,PB13,PB3,PF5,PF4,PE8,
  PD13,PF10,PD12,PE7,PD11,PD14,PE10,PD15,PE12,PF14,PE14,PE9,PE15,PE13,PE11,
  PF13,PF3,PF12,PF15,PG14,PF11,PE0,PD10,PG8,PG5,PG4, // CN12 digital
  PA0,PA1,PA4,PB0,PC2,PC1,PC3,PC0,   // CN11 analogue
  PC5,PA5,PA6,PA7,PB1,PC4,PA2,PA3   // CN12 analogue
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  PA0,PA1,PA4,PB0,PC2,PC1,PC3,PC0,   // CN11 analogue
  PC5,PA5,PA6,PA7,PB1,PC4,PA2,PA3   // CN12 analogue
};
#define I2C_SDA PB9
#define I2C_SCL PB8

// Arduino Zero (SAMD21) using native USB - EXPERIMENTAL, in need of testing
// Pins not available for use:
// XTAL pins - PA00, PA01
// USB pins - USB_HOST_ENABLE (D27), USB_DM (D28), USB_DP (D29)
// I2C pins - SDA (PA22/D20), SCL (PA23,D21)
// LED pins - BuiltinLED (D13), RxLED (D25), TxLED (D26)
// AREF pin - AREF (42)
#elif defined(ARDUINO_ARCH_SAMD)
#define BOARD_TYPE F("Arduino Zero or Clone")
#define NUMBER_OF_DIGITAL_PINS 27
#define NUMBER_OF_ANALOGUE_PINS 6
static const uint8_t digitalPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,A0,A1,A2,A3,A4,A5,22,23,24,38,39,40,41
};
static const uint8_t analoguePinMap[NUMBER_OF_ANALOGUE_PINS] = {
  A0, A1, A2, A3, A4, A5
};
#define I2C_SDA PA22
#define I2C_SCL PA23
*/

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
#define EXIOENAN 0xE7     // Flag to enable an analogue input pin

/////////////////////////////////////////////////////////////////////////////////////
//  Define version to store in EEPROM/FLASH in case this needs to change later
//  This needs to be defined in order to invalidate contents if the structure changes
//  in future releases.
//
#define EEPROM_VERSION 1

/////////////////////////////////////////////////////////////////////////////////////
//  Define the capability hex values for pins based on these bit values:
//  Bit Capability
//  0   Digital input
//  1   Digital output
//  2   Analogue input
//  3   PWM output
//
#define DI 0x01       // Digital input only
#define DO 0x02       // Digital output only
#define DIO 0x03      // Digital input/output
#define AI 0x04       // Analogue input only
#define AIDI 0x05     // Analogue and digital input
#define AIDO 0x06     // Analogue input, digital output
#define AIDIO 0x07    // Analogue input, digital input and output
#define PO 0x08       // PWM output only
#define DIPO 0x09     // Digital input, PWM output
#define DOPO 0x0A     // Digital output, PWM output
#define DIOPO 0x0B    // Digital input/output, PWM output
#define AOPO 0x0C     // Analogue input, PWM output
#define AODIPO 0x0D   // Analogue input, digital input, PWM output
#define AODOPO 0x0E   // Analogue input, digital output, PWM output
#define AODIOPO 0x0F  // Analogue input, digital input/output, PWM output

#endif