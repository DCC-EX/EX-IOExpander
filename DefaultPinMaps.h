/*
 *  © 2022 Peter Cole. All rights reserved.
 *
 *  This file is part of EX-IOExpander.
 *  
 *  This file contains the default pin maps for each architecture and should not need to be edited.
 *  This pin map is only used to ensure all pins start in a safe INPUT mode for startup.
 *  Any new architectures will need a pin map defined to suit.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 * 
 */

#ifndef DEFAULTPINMAPS_H
#define DEFAULTPINMAPS_H

#if defined(ARDUINO_AVR_UNO)
static uint8_t defaultPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  2,3,4,5,6,7,8,9,10,11,12,13,A0,A1,A2,A3
};
#elif defined(ARDUINO_AVR_NANO)
static uint8_t defaultPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  2,3,4,5,6,7,8,9,10,11,12,13,A0,A1,A2,A3,A6,A7
};
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
static uint8_t defaultPinMap[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS] = {
  2,3,4,5,6,7,8,9,10,11,12,13,A0,A1,A2,A3
};
#endif

#endif