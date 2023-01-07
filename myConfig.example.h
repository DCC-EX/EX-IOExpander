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

#endif