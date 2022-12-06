/*
 *  © 2022 xxxxxxxxxx. All rights reserved.
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
//  Define the register hex values as used in IO_MCP23017.h
//  Note only registers related to A are ever used by IO_MCP23017.h
//
#define REG_IODIRA 0x00
#define REG_GPINTENA 0x04
#define REG_INTCONA 0x08
#define REG_IOCON 0x0A
#define REG_GPPUA 0x0C
#define REG_GPIOA 0x12

#endif