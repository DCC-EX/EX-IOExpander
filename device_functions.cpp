/*
 *  © 2023, Peter Cole. All rights reserved.
 *  
 *  This file is part of EX-IOExpander.
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include "globals.h"
#include "device_functions.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/wdt.h>
#endif
#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif

// EEPROM functions here, only for uCs with EEPROM support
#if defined(HAS_EEPROM)
/*
* Function to read I2C address from EEPROM
* Look for "EXIO" and the version EEPROM_VERSION at 0 to 5, address at 6
*/
uint8_t getI2CAddress() {
  char data[5];
  char eepromData[5] = {'E', 'X', 'I', 'O', EEPROM_VERSION};
  uint8_t eepromAddress;
  bool addressSet = true;
  for (uint8_t i = 0; i < 5; i ++) {
    data[i] = EEPROM.read(i);
    if (data[i] != eepromData[i]) {
      addressSet = false;
      break;
    }
  }
  if (addressSet) {
    eepromAddress = EEPROM.read(5);
    if(diag) {
      USB_SERIAL.print(F("I2C address defined in EEPROM: 0x"));
      USB_SERIAL.println(eepromAddress, HEX);
    }
    return eepromAddress;
  } else {
    if(diag) {
      USB_SERIAL.println(F("I2C address not defined in EEPROM"));
    }
    return 0;
  }
}

/*
* Function to store I2C address in EEPROM
*/
void writeI2CAddress(int16_t eepromAddress) {
  char eepromData[5] = {'E', 'X', 'I', 'O', EEPROM_VERSION};
  for (uint8_t i = 0; i < 5; i++) {
    EEPROM.write(i, eepromData[i]);
  }
  USB_SERIAL.print(F("Saving address 0x"));
  USB_SERIAL.print(eepromAddress, HEX);
  USB_SERIAL.println(F(" to EEPROM, reboot to activate"));
  EEPROM.write(5, eepromAddress);
}

/*
* Function to erase EEPROM contents
*/
void eraseI2CAddress() {
  for (uint8_t i = 0; i < 6; i++) {
    EEPROM.write(i, 0);
  }
  USB_SERIAL.println(F("Erased EEPROM, reboot to revert to myConfig.h"));
}

#else
// Placeholders for no EEPROM support
uint8_t getI2CAddress() {
  USB_SERIAL.println(F("No EEPROM support, use myConfig.h"));
  return 0;
}

void writeI2CAddress(int16_t notRequired) {
  USB_SERIAL.println(F("No EEPROM support, use myConfig.h"));
}

void eraseI2CAddress() {
  USB_SERIAL.println(F("No EEPROM support, use myConfig.h"));
}

#endif

void reset() {
#if defined(ARDUINO_ARCH_AVR)
  wdt_enable(WDTO_15MS);
  delay(50);
#elif defined(ARDUINO_ARCH_STM32)
  __disable_irq();
  NVIC_SystemReset();
  while(true) {};
#endif
}

/*
Disabling JTAG is required to avoid pin conflicts on Bluepill
*/
#if defined(ARDUINO_BLUEPILL_F103C8)
void disableJTAG() {
  // Disable JTAG and enable SWD by clearing the SWJ_CFG bits
  // Assuming the register is named AFIO_MAPR or AFIO_MAPR2
  AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG);
  // or
  // AFIO->MAPR2 &= ~(AFIO_MAPR2_SWJ_CFG);
}
#endif
