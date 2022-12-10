/*
 *  © 2022 Peter Cole. All rights reserved.
 *
 *  This file is part of EX-IOExpander.
 *  
 *  This file contains the defines required and should not need to be edited.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 * 
 */
#ifndef DEFINES_H
#define DEFINES_H

/////////////////////////////////////////////////////////////////////////////////////
//  Define the register hex values we need to act on or respond with
//
#define REG_EXIOINIT 0x00     // Flag to start setup procedure
#define REG_EXIODPIN 0x01     // Flag we're receiving digital pin assignments
#define REG_EXIOAPIN 0x02     // Flag we're receiving analogue pin assignments
#define REG_EXIORDY 0x03      // Flag setup procedure done, return to CS to ACK
#define REG_EXIODDIR 0x04     // Flag we're receiving digital pin direction configuration
#define REG_EXIODPUP 0x05     // Flag we're receiving digital pin pullup configuration

/////////////////////////////////////////////////////////////////////////////////////
//  Defines for the supported hardware architectures
//
#if defined(ARDUINO_AVR_UNO)
#define NUMBER_OF_DIGITAL_PINS 12   // D2 - D13
#define NUMBER_OF_ANALOGUE_PINS 4     // A0 - A3, cannot use A4/A5
#elif defined(ARDUINO_AVR_NANO)
#define NUMBER_OF_DIGITAL_PINS 12   // D2 - D13
#define NUMBER_OF_ANALOGUE_PINS 6     // A0 - A3, A6/A7, cannot use A4/A5
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#define NUMBER_OF_DIGITAL_PINS 46   // D2 - D19, D22 - D49
#define NUMBER_OF_ANALOGUE_PINS 16     // A0 - A15
#else
#define CPU_TYPE_ERROR
#endif

#endif