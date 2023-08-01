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

#ifndef DEFINES_H
#define DEFINES_H

/////////////////////////////////////////////////////////////////////////////////////
//  Define CPU specific pin counts
//
//  Arduino Nano or Pro Mini
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_PRO)
#if defined(ARDUINO_AVR_NANO)
#define BOARD_TYPE F("Nano")
#elif defined(ARDUINO_AVR_PRO)
#define BOARD_TYPE F("Pro Mini")
#endif
#define TOTAL_PINS 18
#define HAS_SERVO_LIB
#define MAX_SUPERPINS 16
#define HAS_EEPROM
#define USE_FAST_WRITES
//  Arduino Uno
#elif defined(ARDUINO_AVR_UNO)
#define BOARD_TYPE F("Uno")
#define TOTAL_PINS 16
#define HAS_SERVO_LIB
#define MAX_SUPERPINS 16
#define HAS_EEPROM
#define USE_FAST_WRITES
//  Arduino Mega2560
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#define BOARD_TYPE F("Mega")
#define TOTAL_PINS 62
#define HAS_SERVO_LIB
#define MAX_SUPERPINS 62
#define HAS_EEPROM
#define USE_FAST_WRITES
#elif defined(ARDUINO_NUCLEO_F411RE)
#define BOARD_TYPE F("Nucleo-F411RE")
#define TOTAL_PINS 40
#define NUM_PWM_PINS 25
#define MAX_SUPERPINS 40
#elif defined(ARDUINO_NUCLEO_F412ZG)
#define BOARD_TYPE F("Nucleo-F412ZG")
#define TOTAL_PINS 97
#define NUM_PWM_PINS 40
#define MAX_SUPERPINS 97
#elif defined(ARDUINO_ARCH_SAMD)
#define BOARD_TYPE F("Arduino Zero or Clone")
#define TOTAL_PINS 27
#define NUM_PWM_PINS 12
#define MAX_SUPERPINS 27
#elif defined(ARDUINO_BLUEPILL_F103C8)
#define BOARD_TYPE F("BLUEPILL-STM32F103C8")
#define TOTAL_PINS 28
#define NUM_PWM_PINS 19
#define MAX_SUPERPINS 28
#else
#define CPU_TYPE_ERROR
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  Define serial interfaces here
//

#undef USB_SERIAL           // Teensy has this defined by default (in case we ever support Teensy)
#define USB_SERIAL Serial   // Standard serial port most of the time!
#if defined(ARDUINO_ARCH_SAMD)
#undef USB_SERIAL
#define USB_SERIAL SerialUSB  // Most SAMD21 clones use native USB on the SAMD21G18 variants
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  Include myConfig.h or use the example
//
#if __has_include ("myConfig.h")
  #include "myConfig.h"
#else
  #warning myConfig.h not found. Using defaults from myConfig.example.h
  #include "myConfig.example.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  Define data structures here
//
/*
Struct to define the capability of each physical pin
*/
struct pinDefinition {
  uint8_t physicalPin;      // Physical pin name/number eg. 2, A0, PC0
  uint8_t capability;       // Defined as per the hex pin capability values below
};

/*
Define the structure of the pin config
*/
struct pinConfig {
  uint8_t mode;             // 1 = digital, 2 = analogue, 3 = PWM, 4 = PWM LED
  bool direction;           // 0 = output, 1 = input
  bool pullup;              // 0 = no pullup, 1 = pullup (input only)
  bool enable;              // 0 = disabled (default), 1 = enabled
  uint8_t analogueLSBByte;  // Stores the byte number of the LSB byte in analoguePinStates
  uint8_t servoIndex;       // Stores the servo or dimmer object array index used by the pin
};

/*
Define structure for a reverse pin map to display pin friendly names
*/
struct pinName {
  uint8_t pinNumber;        // Pin number
  String pinLabel;           // Pin name
};

/*
Servo data struct
*/
struct ServoData {
  uint16_t activePosition : 12; // Config parameter
  uint16_t inactivePosition : 12; // Config parameter
  uint16_t currentPosition : 12;
  uint16_t fromPosition : 12;
  uint16_t toPosition : 12; 
  uint8_t profile;  // Config parameter
  uint16_t stepNumber; // Index of current step (starting from 0)
  uint16_t numSteps;  // Number of steps in animation, or 0 if none in progress.
  uint8_t currentProfile; // profile being used for current animation.
  uint16_t duration; // time (tenths of a second) for animation to complete.
}; // 14 bytes per element, i.e. per pin in use

/////////////////////////////////////////////////////////////////////////////////////
//  Define the servo profile type values
//
#define SERVO_INSTANT 0x00      // Moves immediately between positions (if duration not specified)
#define SERVO_USEDURATION 0x00  // Use specified duration
#define SERVO_FAST 0x01         // Takes around 500ms end-to-end
#define SERVO_MEDIUM 0x02       // 1 second end-to-end
#define SERVO_SLOW 0x03         // 2 seconds end-to-end
#define SERVO_BOUNCE 0x04       // For semaphores/turnouts with a bit of bounce!!
#define USE_SUPERPIN 0x80       // Flag to use SuperPin for dimming rather than servo (NoPowerOff in device driver)

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
#define EXIOINITA 0xE8    // Flag to send analogue pin info
#define EXIOPINS 0xE9     // Flag we need to send pin counts
#define EXIOWRAN 0xEA     // Flag we're receiving an analogue write (PWM)
#define EXIOERR 0xEF      // Flag something has errored to send to device driver

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
#define DIGITAL_INPUT 0
#define DIGITAL_OUTPUT 1
#define ANALOGUE_INPUT 2
#define PWM_OUTPUT 3

#define NA 0x00       // Not suitable for use
#define DI 0x01       // Digital input only
#define DO 0x02       // Digital output only
#define DIO 0x03      // Digital input/output
#define AI 0x04       // Analogue input only
#define AIDI 0x05     // Analogue and digital input
#define AIDO 0x06     // Analogue input, digital output
#define AIDIO 0x07    // Analogue input, digital input and output
#define P 0x08        // PWM output only
#define DIP 0x09      // Digital input, PWM output
#define DOP 0x0A      // Digital output, PWM output
#define DIOP 0x0B     // Digital input/output, PWM output
#define AIP 0x0C      // Analogue input, PWM output
#define AIDIP 0x0D    // Analogue input, digital input, PWM output
#define AIDOP 0x0E    // Analogue input, digital output, PWM output
#define AIDIOP 0x0F   // Analogue input, digital input/output, PWM output

/////////////////////////////////////////////////////////////////////////////////////
//  Define the available pin modes
//
#define MODE_UNUSED 0
#define MODE_DIGITAL 1
#define MODE_ANALOGUE 2
#define MODE_PWM 3
#define MODE_PWM_LED 4

/////////////////////////////////////////////////////////////////////////////////////
// Ensure test modes defined in myConfig.h have values
// 
#define ANALOGUE_TEST 1
#define INPUT_TEST 2
#define OUTPUT_TEST 3
#define PULLUP_TEST 4

#endif