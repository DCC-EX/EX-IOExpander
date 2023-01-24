/*
 *  © 2022, Peter Cole. All rights reserved.
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

/*
* EX-IOExpander is a native DCC-EX I/O expander utilising a microcontroller's digital and analogue I/O pins
* to expand the I/O capability of an EX-CommandStation.
*
* All digital I/O pins are available as digital inputs or outputs.
* Analogue I/O pins are available as digital inputs or outputs or analogue inputs (depending on architecture).
*/

#include <Arduino.h>
#include "SupportedDevices.h"
#if defined(ARDUINO_ARCH_AVR)
#include <avr/wdt.h>
#endif
#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif

#ifdef CPU_TYPE_ERROR
#error Unsupported microcontroller architecture detected, you need to use a type listed in defines.h
#endif

/*
If we haven't got a custom config.h, use the example.
*/
#if __has_include ("myConfig.h")
  #include "myConfig.h"
#else
  #warning myConfig.h not found. Using defaults from myConfig.example.h
  #include "myConfig.example.h"
#endif

/*
* Struct to define the digital pin parameters and keep state.
*/
typedef struct {
  bool direction;       // 0 = output, 1 = input
  bool pullup;          // 0 = no pullup, 1 = pullup (input only)
  bool enable;          // 0 = disabled (default), set to 1 = enabled
} digitalConfig;

/*
* Struct to define the analogue pin assignment and keep state
*/
typedef struct {
  bool enable;          // Flag if it's enabled (1) or not (0)
} analogueConfig;

/*
* Create digitalConfig array using struct.
* Size of the array needs to be able to include analogue pins as well.
*/
digitalConfig digitalPins[NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS];

/*
* Create array for analogue pin assignments.
*/
analogueConfig analoguePins[NUMBER_OF_ANALOGUE_PINS];

/*
* Include required files and libraries.
*/
#include "version.h"
#include <Wire.h>

/*
* Global variables here
*/
/*
* If for some reason the I2C address isn't defined, define our default here.
*/
#ifndef I2C_ADDRESS
#define I2C_ADDRESS 0x65
#endif
uint8_t i2cAddress = I2C_ADDRESS;   // Assign address to a variable for validation and serial input
uint8_t numDigitalPins = NUMBER_OF_DIGITAL_PINS;    // Init with default, will be overridden by config
uint8_t numAnaloguePins = NUMBER_OF_ANALOGUE_PINS;  // Init with default, will be overridden by config
int digitalPinBytes;  // Used for configuring and sending/receiving digital pins
int analoguePinBytes; // Used for enabling/disabling analogue pins
bool setupComplete = false;   // Flag when initial configuration/setup has been received
uint8_t outboundFlag;   // Used to determine what data to send back to the CommandStation
byte analogueOutBuffer[2];  // Array to send requested LSB/MSB of the analogue value to the CommandStation
byte digitalOutBuffer[1];   // Array to send digital value to CommandStation
bool newSerialData = false;   // Flag for new serial data being received
const byte numSerialChars = 10;   // Max number of chars for serial input
char serialInputChars[numSerialChars];  // Char array for serial input
char * version;   // Pointer for getting version
uint8_t versionBuffer[3];   // Array to hold version info to send to device driver
unsigned long lastPinDisplay = 0;   // Last time in millis we displayed DIAG input states
bool diag = false;    // Enable/disable diag outputs
unsigned long displayDelay = DIAG_CONFIG_DELAY * 1000;    // Delay in ms between diag display updates
bool analogueTesting = false;   // Flag that analogue input testing is enabled/disabled
bool inputTesting = false;    // Flag that digital input testing without pullups is enabled/disabled
bool outputTesting = false;   // Flag that digital output testing is enabled/disabled
bool pullupTesting = false;   // Flag that digital input testing with pullups is enabled/disabled
unsigned long lastOutputTest = 0;   // Last time in millis we swapped output test state
bool outputTestState = LOW;   // Flag to set outputs high or low for testing
byte digitalPinStates[(NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS) / 8];
byte analoguePinStates[NUMBER_OF_ANALOGUE_PINS * 2];

// Ensure test modes defined in myConfig.h have values
#define ANALOGUE_TEST 1
#define INPUT_TEST 2
#define OUTPUT_TEST 3
#define PULLUP_TEST 4

/*
* Main setup function here.
*/
void setup() {
  Serial.begin(115200);
  Serial.print(F("DCC-EX EX-IOExpander v"));
  Serial.println(VERSION);
  Serial.print(F("Detected device: "));
  Serial.println(BOARD_TYPE);
  if (getI2CAddress() != 0) {
    i2cAddress = getI2CAddress();
  }
  if (i2cAddress < 0x08 || i2cAddress > 0x77) {
    Serial.print(F("ERROR: Invalid I2C address configured: 0x"));
    Serial.print(i2cAddress, HEX);
    Serial.println(F(", using myConfig.h instead"));
    i2cAddress = I2C_ADDRESS;
  }
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(i2cAddress, HEX);
  setVersion();
#ifdef DIAG
  diag = true;
#endif
  Wire.begin(i2cAddress);
// If desired and pins defined, disable I2C pullups
#if defined(DISABLE_I2C_PULLUPS) && defined(I2C_SDA) && defined(I2C_SCL)
  Serial.print(F("Disabling I2C pullups on pins SDA|SCL: "));
  Serial.print(I2C_SDA);
  Serial.print(F("|"));
  Serial.println(I2C_SCL);
  digitalWrite(I2C_SDA, LOW);
  digitalWrite(I2C_SCL, LOW);
#endif
// Need to intialise every pin in INPUT mode (no pull ups) for safe start
  initialisePins();
  Serial.println(F("Initialised all pins as input only"));
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
#if (TEST_MODE == ANALOGUE_TEST)
  testAnalogue(true);
#elif (TEST_MODE == INPUT_TEST)
  testInput(true);
#elif (TEST_MODE == OUTPUT_TEST)
  testOutput(true);
#elif (TEST_MODE == PULLUP_TEST)
  testPullup(true);
#endif
}

/*
* Main loop here, just processes our inputs and updates the writeBuffer.
*/
void loop() {
  if (setupComplete) {
    for (uint8_t dPin = 0; dPin < numDigitalPins; dPin++) {
      uint8_t pinByte = dPin / 8;
      if (digitalPinMap[dPin]) {
        if (digitalPins[dPin].direction == 1 && digitalPins[dPin].enable == 1) {
          if (digitalPins[dPin].pullup == 1) {
            pinMode(digitalPinMap[dPin], INPUT_PULLUP);
          } else {
            pinMode(digitalPinMap[dPin], INPUT);
          }
          bool currentState = digitalRead(digitalPinMap[dPin]);
          if (digitalPins[dPin].pullup) currentState = !currentState;
          uint8_t pinBit = dPin - pinByte * 8;
          if (currentState) {
            bitSet(digitalPinStates[pinByte], pinBit);
          } else {
            bitClear(digitalPinStates[pinByte], pinBit);
          }
        }
      }
    }
    for (uint8_t aPin = 0; aPin < numAnaloguePins; aPin++) {
      uint8_t pinLSBByte = aPin * 2;
      uint8_t pinMSBByte = pinLSBByte + 1;
      if (analoguePins[aPin].enable == 1) {
        uint16_t value = analogRead(analoguePinMap[aPin]);
        analoguePinStates[pinLSBByte] = value & 0xFF;
        analoguePinStates[pinMSBByte] = value >> 8;
      }
    }
    if (outputTesting) {
      if (millis() - lastOutputTest > 500) {
        outputTestState = !outputTestState;
        lastOutputTest = millis();
        for (uint8_t dPin = 0; dPin < numDigitalPins; dPin++) {
          if (digitalPinMap[dPin]) {
            pinMode(digitalPinMap[dPin], OUTPUT);
            digitalWrite(digitalPinMap[dPin], outputTestState);
          }
        }
      }
    }
  }
  if (diag) {
    displayPins();
  }
  processSerialInput();
}

/*
* Function triggered when CommandStation is sending data to this device.
*/
void receiveEvent(int numBytes) {
  if (numBytes == 0) {
    return;
  }
  byte buffer[numBytes];
  for (uint8_t byte = 0; byte < numBytes; byte++) {
    buffer[byte] = Wire.read();   // Read all received bytes into our buffer array
  }
  switch(buffer[0]) {
    // Initial configuration start, must be 3 bytes
    case EXIOINIT:
      if (numBytes == 3) {
        initialisePins();
        numDigitalPins = buffer[1];
        numAnaloguePins = buffer[2];
        if (numDigitalPins + numAnaloguePins == NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS) {
          // Calculate number of bytes required to cover pins
          digitalPinBytes = (numDigitalPins + 7) / 8;
          analoguePinBytes = numAnaloguePins * 2;
          Serial.print(F("Received pin configuration (digital|analogue): "));
          Serial.print(numDigitalPins);
          Serial.print(F("|"));
          Serial.println(numAnaloguePins);
          setupComplete = true;
        } else {
          Serial.print(F("ERROR: Invalid pins sent by device driver! (Digital|Analogue): "));
          Serial.print(numDigitalPins);
          Serial.print(F("|"));
          Serial.println(numAnaloguePins);
          setupComplete = false;
        }
        outboundFlag = EXIOINIT;
      } else {
#ifdef DIAG
        Serial.println(F("EXIOINIT received with incorrect data"));
#endif
      }
      break;
    // Flag to set digital pin pullups, 0 disabled, 1 enabled
    case EXIODPUP:
      if (numBytes == 3) {
        uint8_t pin = buffer[1];
        if (digitalPins[pin].enable == 1 && digitalPins[pin].direction == 0) {
          Serial.print(F("ERROR! pin *"));
          Serial.print(digitalPinMap[pin]);
          Serial.println(F(" already defined as output pin, cannot use as input"));
          break;
        }
        if (digitalPins[pin].enable == 0) {
          digitalPins[pin].direction = 1;   // Must be an input if we got a pullup config
          digitalPins[pin].pullup = buffer[2];
          digitalPins[pin].enable = 1;
          if (digitalPins[pin].pullup == 1) {
            pinMode(digitalPinMap[pin], INPUT_PULLUP);
          } else {
            pinMode(digitalPinMap[pin], INPUT);
          }
        }
      } else {
#ifdef DIAG
      Serial.println(F("EXIODPUP received with incorrect number of bytes"));
#endif
      }
      break;
    case EXIORDAN:
      if (numBytes == 1) {
        outboundFlag = EXIORDAN;
      }
      break;
    case EXIOWRD:
      if (numBytes == 3) {
        uint8_t dPin = buffer[1];
        bool state = buffer[2];
        uint8_t dPinByte = dPin / 8;
        uint8_t dPinBit = dPin - dPinByte * 8;
        if (digitalPins[dPin].enable == 1 && digitalPins[dPin].direction == 1) {
          Serial.print(F("ERROR! pin "));
          Serial.print(digitalPinMap[dPin]);
          Serial.println(F(" already defined as input pin, cannot use as output"));
          break;
        }
        if (digitalPins[dPin].enable == 0) {
          digitalPins[dPin].enable = 1;
          pinMode(digitalPinMap[dPin], OUTPUT);
          digitalPins[dPin].direction = 0;
        }
        if (state) {
          bitSet(digitalPinStates[dPinByte], dPinBit);
        } else {
          bitClear(digitalPinStates[dPinByte], dPinBit);
        }
        digitalWrite(digitalPinMap[dPin], state);
      }
      break;
    case EXIORDD:
      if (numBytes == 1) {
        outboundFlag = EXIORDD;
      }
      break;
    case EXIOVER:
      if (numBytes == 1) {
        outboundFlag = EXIOVER;
      }
      break;
    case EXIOENAN:
      if (numBytes == 2) {
        uint8_t pin = buffer[1] - NUMBER_OF_DIGITAL_PINS;
        analoguePins[pin].enable = 1;
      }
    default:
      break;
  }
}

/*
* Function triggered when CommandStation polls for inputs on this device.
*/
void requestEvent() {
  switch(outboundFlag) {
    case EXIOINIT:
      if (setupComplete) {
        Wire.write(EXIORDY);
      } else {
        Wire.write(0);
      }
      break;
    case EXIORDAN:
      Wire.write(analoguePinStates, analoguePinBytes);
      break;
    case EXIORDD:
      Wire.write(digitalPinStates, digitalPinBytes);
      break;
    case EXIOVER:
      Wire.write(versionBuffer, 3);
      break;
    default:
      break;
  }
}

/*
* Function to display pin configuration and states when DIAG enabled
*/
void displayPins() {
  if (millis() - lastPinDisplay > displayDelay) {
    lastPinDisplay = millis();
    Serial.println(F("Digital Pin|Enable|Direction|Pullup|State:"));
    for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
      uint8_t dPinByte = pin / 8;
      uint8_t dPinBit = pin - dPinByte * 8;
      Serial.print(digitalPinMap[pin]);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].enable);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].direction);
      Serial.print(F("|"));
      Serial.print(digitalPins[pin].pullup);
      Serial.print(F("|"));
      if (pin == NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS - 1 || (pin % 15 == 0 && pin > 0)) {
        Serial.println(bitRead(digitalPinStates[dPinByte], dPinBit));
      } else {
        Serial.print(bitRead(digitalPinStates[dPinByte], dPinBit));
        Serial.print(F(","));
      }
    }
    Serial.println(F("Analogue Pin|Enable|Value|LSB|MSB:"));
    for (uint8_t pin = 0; pin < NUMBER_OF_ANALOGUE_PINS; pin++) {
      uint8_t lsbByte = pin * 2;
      uint8_t msbByte = lsbByte + 1;
      Serial.print(analoguePinMap[pin]);
      Serial.print(F("|"));
      Serial.print(analoguePins[pin].enable);
      Serial.print(F("|"));
      Serial.print((analoguePinStates[msbByte] << 8) + analoguePinStates[lsbByte]);
      Serial.print(F("|"));
      Serial.print(analoguePinStates[lsbByte]);
      Serial.print(F("|"));
      if (pin == NUMBER_OF_ANALOGUE_PINS - 1) {
        Serial.println(analoguePinStates[msbByte]);
      } else {
        Serial.print(analoguePinStates[msbByte]);
        Serial.print(F(","));
      }
    }
  }
}

/*
* Function to get the version from version.h char array to bytes nicely
*/
void setVersion() {
  const String versionString = VERSION;
  char versionArray[versionString.length() + 1];
  versionString.toCharArray(versionArray, versionString.length() + 1);
  version = strtok(versionArray, "."); // Split version on .
  versionBuffer[0] = atoi(version);  // Major first
  version = strtok(NULL, ".");
  versionBuffer[1] = atoi(version);  // Minor next
  version = strtok(NULL, ".");
  versionBuffer[2] = atoi(version);  // Patch last
}

/*
* Function to read and process serial input for I2C address config
*/
void processSerialInput() {
  static bool serialInProgress = false;
  static byte serialIndex = 0;
  char startMarker = '<';
  char endMarker = '>';
  char serialChar;
  while (Serial.available() > 0 && newSerialData == false) {
    serialChar = Serial.read();
    if (serialInProgress == true) {
      if (serialChar != endMarker) {
        serialInputChars[serialIndex] = serialChar;
        serialIndex++;
        if (serialIndex >= numSerialChars) {
          serialIndex = numSerialChars - 1;
        }
      } else {
        serialInputChars[serialIndex] = '\0';
        serialInProgress = false;
        serialIndex = 0;
        newSerialData = true;
      }
    } else if (serialChar == startMarker) {
      serialInProgress = true;
    }
  }
  if (newSerialData == true) {
    newSerialData = false;
    char * strtokIndex;
    strtokIndex = strtok(serialInputChars," ");
    char activity = strtokIndex[0];    // activity is our first parameter
    strtokIndex = strtok(NULL," ");       // space is null, separator
    unsigned long parameter;
    if (activity == 'W') {
      parameter = strtol(strtokIndex, NULL, 16); // last parameter is the address in hex
    } else {
      parameter = strtol(strtokIndex, NULL, 10);
    }
    switch (activity) {
      case 'A': // Enable/disable analogue input testing
        if (analogueTesting) {
          testAnalogue(false);
          Serial.println(F("Analogue testing disabled"));
        } else {
          testAnalogue(true);
        }
        break;
      case 'D': // Enable/disable diagnostic output
        if (diag && parameter) {
          displayDelay = parameter * 1000;
          Serial.print(F("Diagnostics enabled, delay set to "));
          Serial.println(displayDelay);
          diag = true;
        } else if (diag && !parameter) {
          Serial.println(F("Diagnostics disabled"));
          diag = false;
        } else {
          Serial.print(F("Diagnostics enabled, delay set to "));
          Serial.println(displayDelay);
          diag = true;
        }
        break;
      case 'E': // Erase EEPROM
        eraseI2CAddress();
        break;
      case 'I': // Enable/disable digital input testing
        if (inputTesting) {
          testInput(false);
          Serial.println(F("Input testing (no pullups) disabled"));
        } else {
          testInput(true);
        }
        break;
      case 'O': // Enable/disable digital output testing
        if (outputTesting) {
          testOutput(false);
          Serial.println(F("Output testing disabled"));
        } else {
          testOutput(true);
        }
        break;
      case 'P': // Enable/disable digital input testing with pullups
        if (pullupTesting) {
          testPullup(false);
          Serial.println(F("Pullup input testing disabled"));
        } else {
          testPullup(true);
        }
        break;
      case 'R': // Read address from EEPROM
        if (getI2CAddress() == 0) {
          Serial.println(F("I2C address not stored, using myConfig.h"));
        } else {
          Serial.print(F("I2C address stored is 0x"));
          Serial.println(getI2CAddress(), HEX);
        }
        break;
      case 'T': // Display current state of test modes
        if (analogueTesting) {
          Serial.println(F("Analogue testing enabled"));
        } else if (inputTesting) {
          Serial.println(F("Input testing (no pullups) enabled"));
        } else if (outputTesting) {
          Serial.println(F("Output testing enabled"));
        } else if (pullupTesting) {
          Serial.println(F("Pullup input testing enabled"));
        } else {
          Serial.println(F("No testing in progress"));
        }
        break;
      case 'W': // Write address to EEPROM
        if (parameter > 0x07 && parameter < 0x78) {
          writeI2CAddress(parameter);
        } else {
          Serial.println(F("Invalid I2C address, must be between 0x08 and 0x77"));
        }
        break;
      case 'Z': // Software reboot
        reset();
        break;
      
      default:
        break;
    }
  }
}

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
#ifdef DIAG
      Serial.print(F("I2C address defined in EEPROM: 0x"));
      Serial.println(eepromAddress, HEX);
#endif
    return eepromAddress;
  } else {
#ifdef DIAG
    Serial.println(F("I2C address not defined in EEPROM"));
#endif
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
  Serial.print(F("Saving address 0x"));
  Serial.print(eepromAddress, HEX);
  Serial.println(F(" to EEPROM, reboot to activate"));
  EEPROM.write(5, eepromAddress);
}

/*
* Function to erase EEPROM contents
*/
void eraseI2CAddress() {
  for (uint8_t i = 0; i < 6; i++) {
    EEPROM.write(i, 0);
  }
  Serial.println(F("Erased EEPROM, reboot to revert to myConfig.h"));
}

#else
// Placeholders for no EEPROM support
uint8_t getI2CAddress() {
  Serial.println(F("No EEPROM support, use myConfig.h"));
  return 0;
}

void writeI2CAddress(int16_t notRequired) {
  Serial.println(F("No EEPROM support, use myConfig.h"));
}

void eraseI2CAddress() {
  Serial.println(F("No EEPROM support, use myConfig.h"));
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
* Testing functions below, just pass true/false to enable/disable the appropriate testing
* Note enabling any of these will disable Wire() (providing the library supports it) so the
* device will need to be rebooted once testing is completed to enable it again.
*/
void testAnalogue(bool enable) {
  if (enable) {
    Serial.println(F("Analogue input testing enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testInput(false);
    testOutput(false);
    testPullup(false);
    diag = true;
    analogueTesting = true;
    for (uint8_t pin = 0; pin < NUMBER_OF_ANALOGUE_PINS; pin++) {
      analoguePins[pin].enable = 1;
    }
  } else {
    analogueTesting = false;
    diag = false;
    initialisePins();
  }
}

void testInput(bool enable) {
  if (enable) {
    Serial.println(F("Input testing (no pullups) enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testAnalogue(false);
    testOutput(false);
    testPullup(false);
    diag = true;
    numDigitalPins = NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS;
    inputTesting = true;
    for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
      if (digitalPinMap[pin]) {
        digitalPins[pin].direction = 1;
        digitalPins[pin].pullup = 0;
        digitalPins[pin].enable = 1;
      }
    }
  } else {
    inputTesting = false;
    diag = false;
    initialisePins();
  }
}

void testOutput(bool enable) {
  if (enable) {
    Serial.println(F("Output testing enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testAnalogue(false);
    testInput(false);
    testPullup(false);
    diag = true;
    numDigitalPins = NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS;
    outputTesting = true;
    for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
      if (digitalPinMap[pin]) {
        digitalPins[pin].direction = 0;
      }
    }
  } else {
    outputTesting = false;
    diag = false;
    initialisePins();
  }
}

void testPullup(bool enable) {
  if (enable) {
    Serial.println(F("Pullup input testing enabled, I2C connection disabled, diags enabled, reboot once testing complete"));
    setupComplete = true;
    disableWire();
    testAnalogue(false);
    testOutput(false);
    testInput(false);
    diag = true;
    numDigitalPins = NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS;
    pullupTesting = true;
    for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
      if (digitalPinMap[pin]) {
        digitalPins[pin].direction = 1;
        digitalPins[pin].pullup = 1;
        digitalPins[pin].enable = 1;
      }
    }
  } else {
    pullupTesting = false;
    diag = false;
    initialisePins();
  }
}

void disableWire() {
#ifdef WIRE_HAS_END
  Wire.end();
#else
  Serial.println(F("WARNING! The Wire.h library has no end() function, ensure EX-IOExpander is disconnected from your CommandStation"));
#endif
}

/*
* Function to initialise all pins as input and initialise pin struct
*/
void initialisePins() {
  for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS; pin++) {
    pinMode(digitalPinMap[pin], INPUT);
  }
  for (uint8_t pin = 0; pin < NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS; pin++) {
    if (digitalPinMap[pin]) {
      digitalPins[pin].direction = 1;
      digitalPins[pin].enable = 0;
      digitalPins[pin].pullup = 0;
    }
  }
  for (uint8_t dPinByte = 0; dPinByte < (NUMBER_OF_DIGITAL_PINS + NUMBER_OF_ANALOGUE_PINS) / 8; dPinByte++) {
    digitalPinStates[dPinByte] = 0;
  }
  for (uint8_t pin = 0; pin < NUMBER_OF_ANALOGUE_PINS; pin++) {
    pinMode(analoguePinMap[pin], INPUT);
    analoguePins[pin].enable = 0;
  }
  for (uint8_t aPinByte = 0; aPinByte < NUMBER_OF_ANALOGUE_PINS * 2; aPinByte++) {
    analoguePinStates[aPinByte] = 0;
  }
}