#ifndef VERSION_H
#define VERSION_H

// Version must only ever be numeric in order to be able to send it to the CommandStation
#define VERSION "0.0.23"

// 0.0.23 includes:
//  - Add use of Servo library for controlling servos for supported platforms
//  - Add SuperPin class for neat dimming of LEDs without needing hardware PWM pins
//  - Adjust pin maps for F411RE and F412ZG for more logical Vpin to physical pin mapping
//  - Enable fast digital writes for SuperPin on AVR platforms
// 0.0.22 includes:
//  - Add experimental support for Bluepill STM32F103C8
//  - Enhance <V> command to show board type, I2C address as well as Vpin map
// 0.0.21 includes:
//  - Fix bug in output testing where LEDs weren't turning on/off at the correct interval
//  - Add pin label map for diagnostic output to show pin labels instead of raw numbers
//  - Enable Vpin map command <V>
//  - Change testing command to parameters <T A>, <T I>, <T O>, <T P>
//  - Fix <D delay> command so enabling when providing delay interval actually sets the interval
//  - Disable STM32F103C8 Bluepill
// 0.0.20 includes:
//  - Fix bug for digital inputs due to incorrect memory allocation
// 0.0.19 includes:
//  - Fix bug where "diag" was not declared correctly causing compiler error when diags enabled
//  - Fix bug where the analogue inputs were not working
// 0.0.18 includes:
//  - Move all serial output away from I2C receiveEvent() to avoid slowing events down
// 0.0.17 includes:
//  - Bug fix for WAITFOR() not waiting
// 0.0.16 includes:
//  - Move servo animations from device driver to EX-IOExpander software
// 0.0.15 includes:
//  - Add initial STM32F103C8T6 Bluepill support (untested)
// 0.0.14 includes:
//  - *Breaking change* to interaction with device driver
//  - *Breaking change* to pin maps
//  - Use EX-CommandStation v4.2.15 or later
//  - Add experimental PWM support
//  - Code restructure to multiple files
// 0.0.13 includes:
//  - Experimental support for SAMD21 Arduino Zero clones using native USB serial comms
// 0.0.12 includes:
//  - Add configuration option to disable internal I2C pullups
// 0.0.11 includes:
//  - *Breaking change* to optimise speed of transferring inputs to the device driver by sending all at once
//  - Use EX-CommandStation v4.2.10pre1 or later
// 0.0.10 includes:
//  - Add support for Nucleo F412ZG
//  - Correct F411RE pin map
// 0.0.9 includes:
//  - Add support for Arduino Pro Mini
//  - Add option to enable tests via myConfig.h
// 0.0.8 includes:
//  - Add pin initialisation function to tests and when receiving pin config
//  - Always display when receiving pin config
//  - Disable diag output when disabling tests
//  - Fix bug where setting pinMode for output pins was missing
//  - Update display delay comparison to prevent hangs
//  - Prevent input being used as output and vice versa
// 0.0.7 includes:
//  - Optimise sending version to device driver to remove compiler warnings
//  - Diagnostics enabled/disabled via serial console
//  - Diagnostic display delay configured via serial console
//  - Added test modes for digital input with/without pullups, analogue input, digital output
// 0.0.6 includes:
//  - Add experimental support for NUCLEO-F411RE
// 0.0.5 includes:
//  - Send device version to CommandStation device driver
// 0.0.4 includes:
//  - Add support for configuring I2C address via serial, save to EEPROM
//  - Serial commands <R>, <W address>, <E>, <Z> - see doco
// 0.0.3 includes:
//  - Bug fix for compilation on Mega
// 0.0.2 includes:
//  - Change from MCP23017 emulation to native DCC-EX device driver.
//  - Digital input/output.
//  - Analogue input.
// 0.0.1 includes:
//  - MCP23017 digital I/O emulation (minus interrupts).

#endif
