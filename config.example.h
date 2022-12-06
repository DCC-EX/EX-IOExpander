/*
 *  © 2022 xxxxxxxxxx. All rights reserved.
 *
 *  This is the example configuration file for EX-IOExpander.
 * 
 *  It is highly recommended to copy this to "config.h" and modify to suit your specific
 *  requirements.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 */
#ifndef CONFIG_H
#define CONFIG_H

/////////////////////////////////////////////////////////////////////////////////////
//  Define I2C address
//  Default 0x90, can be any valid, available I2C address
// 
#define I2C_ADDRESS 0x90

/////////////////////////////////////////////////////////////////////////////////////
//  Define the number of I/O pins to use
//  16 pins is the maximum here
#define NUMBER_OF_PINS 16

/////////////////////////////////////////////////////////////////////////////////////
//  Define the pin map
//  Defining a pin map should allow portability to other platforms
// 
//  You must define the correct number of ports as per NUMBER_OF_PINS above
//
static uint8_t pinMap[NUMBER_OF_PINS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3};

/////////////////////////////////////////////////////////////////////////////////////
//  Uncomment to enable diag output
// 
// #define DIAG

#endif