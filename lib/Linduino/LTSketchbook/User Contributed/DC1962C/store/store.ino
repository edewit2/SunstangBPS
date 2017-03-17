/*
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3880
http://www.linear.com/product/LTC2974
http://www.linear.com/product/LTC2977

http://www.linear.com/demo/DC1962C

REVISION HISTORY
$Revision:  $
$Date:  $

Copyright (c) 2014, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

#include <Arduino.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <LT_SMBus.h>
#include <LT_SMBusPec.h>
#include <LT_SMBusNoPec.h>
#include <LT_PMBus.h>
#include <avr/boot.h>

#define LTC3880_I2C_ADDRESS 0x30
#define LTC2974_I2C_ADDRESS 0x32
#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc3880_i2c_address;
static uint8_t ltc2974_i2c_address;
static uint8_t ltc2977_i2c_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);
static bool pec = false;

static uint8_t dc1613_addresses[] = { LTC3880_I2C_ADDRESS, LTC2974_I2C_ADDRESS, LTC2977_I2C_ADDRESS };

void wait_for_nvm()
{
  delay(2); // Allow time for action to start.

  smbus->waitForAck(LTC3880_I2C_ADDRESS, 0x00);
  pmbus->waitForNotBusy(LTC3880_I2C_ADDRESS);
  pmbus->waitForNvmDone(LTC3880_I2C_ADDRESS);

  smbus->waitForAck(LTC2974_I2C_ADDRESS, 0x00);
  pmbus->waitForNotBusy(LTC2974_I2C_ADDRESS);

  smbus->waitForAck(LTC2977_I2C_ADDRESS, 0x00);
  pmbus->waitForNotBusy(LTC2977_I2C_ADDRESS);
}

void store_nvm ()
{
  smbus->writeByte(LTC3880_I2C_ADDRESS, MFR_EEPROM_STATUS, 0x00);

  Serial.println(F("Store User All"));
  pmbus->storeToNvmGlobal();
  wait_for_nvm();
  // Storing creates a fault when a Poll on Ack generates a Busy Fault just after
  // a Clear Fault Log.
  pmbus->clearFaults(LTC2977_I2C_ADDRESS);
  Serial.println(F("Store User All Complete"));
}

void restore_nvm ()
{
  smbus->writeByte(LTC3880_I2C_ADDRESS, MFR_EEPROM_STATUS, 0x00);

  Serial.println(F("Restore User All"));
  pmbus->restoreFromNvmGlobal();
  wait_for_nvm();
  Serial.println(F("Restore User All Complete"));
}

void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC

  print_title();
  ltc3880_i2c_address = LTC3880_I2C_ADDRESS;
  ltc2974_i2c_address = LTC2974_I2C_ADDRESS;
  ltc2977_i2c_address = LTC2977_I2C_ADDRESS;
  print_prompt();
}

void loop()
{
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t revision[10];
  uint8_t *addresses = NULL;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        store_nvm();
        break;
      case 2:
        restore_nvm();
        break;
      case 3:
        pmbus->clearFaultsGlobal();
        break;
      case 4:
        pmbus->enablePec(ltc3880_i2c_address);
        pmbus->enablePec(ltc2974_i2c_address);
        pmbus->enablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        pec = true;
        break;
      case 5:
        pmbus->disablePec(ltc3880_i2c_address);
        pmbus->disablePec(ltc2974_i2c_address);
        pmbus->disablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        pec = false;
        break;
      case 6:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 7:
        pmbus->startGroupProtocol();
        pmbus->reset(ltc3880_i2c_address);
        pmbus->restoreFromNvm(ltc2974_i2c_address);
        pmbus->restoreFromNvm(ltc2977_i2c_address);
        pmbus->executeGroupProtocol();

        smbus->waitForAck(ltc3880_i2c_address, 0x00);
        pmbus->waitForNotBusy(ltc3880_i2c_address);

        smbus->waitForAck(ltc2974_i2c_address, 0x00);
        pmbus->waitForNotBusy(ltc2974_i2c_address);

        smbus->waitForAck(ltc2974_i2c_address, 0x00);
        pmbus->waitForNotBusy(ltc2974_i2c_address);
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n********************************************************************\n"));
  Serial.print(F("* DC1962C Store/Restore User All                                   *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("* This program demonstrates how to store and restore EEPROM.       *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.   *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("********************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Store\n"));
  Serial.print(F("  2-Restore\n"));
  Serial.print(F("  3-Clear Faults\n"));
  Serial.print(F("  4-PEC On\n"));
  Serial.print(F("  5-PEC Off\n"));
  Serial.print(F("  6-Bus Probe\n"));
  Serial.print(F("  7-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}
