#ifndef VERSION_H
#define VERSION_H

// Version must only ever be numeric in order to be able to send it to the CommandStation
#define VERSION "0.0.7"

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
