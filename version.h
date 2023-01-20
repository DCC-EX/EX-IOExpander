#ifndef VERSION_H
#define VERSION_H

// Version must only ever be numeric in order to be able to send it to the CommandStation
#define VERSION "0.0.10"

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
