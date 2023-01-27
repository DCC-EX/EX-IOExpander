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

#ifndef DEFINES_H
#define DEFINES_H

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