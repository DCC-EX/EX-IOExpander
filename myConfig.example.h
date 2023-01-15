/*
 *  © 2022 Peter Cole. All rights reserved.
 *
 *  This is the example configuration file for EX-IOExpander.
 * 
 *  It is highly recommended to copy this to "config.h" and modify to suit your specific
 *  requirements.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 */
#ifndef MYCONFIG_H
#define MYCONFIG_H

/////////////////////////////////////////////////////////////////////////////////////
//  Define I2C address
//  Default 0x90, can be any valid, available I2C address
// 
#define I2C_ADDRESS 0x65

/////////////////////////////////////////////////////////////////////////////////////
//  Uncomment to enable diag output
// 
// #define DIAG

/////////////////////////////////////////////////////////////////////////////////////
//  Delay between dumping the status of the port config if DIAG enabled
// 
#define DIAG_CONFIG_DELAY 5

/////////////////////////////////////////////////////////////////////////////////////
//  Enable test mode - ensure only one test mode is active at one time.
//  This is handy if serial input doesn't work for commands for some reason. 
// 
//  ANALOGUE - equivalent of <A>
//  INPUT - equivalent of <I>
//  OUTPUT - equivalent of <O>
//  PULLUP - equivalent of <P>
// 
// #define TEST_MODE ANALOGUE_TEST
// #define TEST_MODE INPUT_TEST
// #define TEST_MODE OUTPUT_TEST
// #define TEST_MODE PULLUP_TEST

#endif