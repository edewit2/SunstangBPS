/*!
Linear Technology DC2094A Demonstration Board.
LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC

@verbatim

NOTES
 Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Ensure all jumpers on the demo board are installed in their default positions
   as described in Demo Manual DC2094A. Apply +/- 16V to the indicated terminals.
   Make sure the input range of channels are configured to measure according to
   the input range required.


 Menu Entry 1: Display ADC Output Data and Calculated ADC input voltage

 Menu Entry 2: Display Configuration Setting

 Menu Entry 3: Change Configuration Setting

@endverbatim

REVISION HISTORY
$Revision: 3659 $
$Date: 2015-1-17

Copyright (c) 2013, Linear Technology Corp.(LTC)
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

Copyright 2013 Linear Technology Corp. (LTC)
 */


/*! @file
    @ingroup LTC2348
*/
#include <Arduino.h>
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <Wire.h>
#include <stdint.h>
#include <SPI.h>
#include "LTC2348.h"

// Macros
#define  CONFIG_WORD_POSITION  0X07

// Global variable
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected
uint8_t channel;

// Setting input range of all channels - 0V to 0.5 Vref with no gain compression (SS2 = 0, SS1 = 0, SS0 = 1)
struct Config_Word_Struct CWSTRUCT = { 1, 1, 1, 1, 1, 1, 1, 1};

// Function declarations
void print_title();
void print_prompt();
uint8_t menu1_display_adc_output();
uint8_t menu2_display_channel_CW();
uint8_t menu3_changeCW();

//! Initialize Linduino
void setup()
{
  uint8_t value = 0;
  uint8_t *p = &value;
  char demo_name[] = "LTC2348-18"; //! Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();             //! Initializes Linduino I2C port.
  quikeval_SPI_init();         //! Initializes Linduino SPI port.

  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();

  demo_board_connected = discover_DC2094(demo_name);
  if (demo_board_connected)
  {
    print_prompt();
  }

  i2c_read_byte(0x20, &value);      // 0x20 is the port address for i/o expander for I2C.
  delay(100);
  value = value & 0x7F;                 // P7 = WRIN = 0
  value = value | 0x04;                 // P2 = WRIN2 = 1
  i2c_write_byte(0x20, value);
  delay(100);

  quikeval_SPI_connect();           //! Connects to main SPI port
}

//! Repeats Linduino Loop
void loop()
{
  int8_t user_command;                 // The user input command
  uint8_t acknowledge = 0;
  if (Serial.available())             // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        acknowledge |= menu1_display_adc_output();
        break;
      case 2:
        acknowledge |= menu2_display_channel_CW();
        break;
      case 3:
        acknowledge |= menu3_changeCW();
        break;
      default:
        Serial.println(F("Incorrect Option"));
    }
    if (acknowledge)
      Serial.println(F("***** I2C ERROR *****"));
    Serial.print(F("\n****************************** Press Enter to Continue ******************************\n"));
    read_int();
    print_prompt();
  }
}

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
uint8_t discover_DC2094(char *demo_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);

  if (!strcmp(demo_board.product_name, demo_name))
  {
    Serial.print("Demo Board Name: ");
    Serial.println(demo_board.name);
    Serial.print("Product Name: ");
    Serial.println(demo_board.product_name);
    if (demo_board.option)
    {
      Serial.print("Demo Board Option: ");
      Serial.println(demo_board.option);
    }
    Serial.println(F("Demo board connected"));
    Serial.println(F("\n\n\t\t\t\tPress Enter to Continue..."));
    read_int();
    return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
    return 0;
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC2094A Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 18-bit ADC.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\n\n\n\t\t\t\tCONFIGURATION SETTINGS (Vref = 4.096V)\n"));
  Serial.println(F("|Config Number| SS2 | SS1 | SS0 | ANALOG INPUT RANGE      | DIGITAL COMPRESSION | RESULT BINARY FORMAT |"));
  Serial.println(F("|-------------|-----------------|-------------------------|---------------------|----------------------|"));
  Serial.println(F("|      0      |  0  |  0  |  0  | Disable Channel         | N/A                 | All Zeros            |"));
  Serial.println(F("|      1      |  0  |  0  |  1  | 0 - 1.25 Vref           | 1                   | Straight Binary      |"));
  Serial.println(F("|      2      |  0  |  1  |  0  | -1.25 Vref - +1.25 Vref | 1/1.024             | Two's Complement     |"));
  Serial.println(F("|      3      |  0  |  1  |  1  | -1.25 Vref - +1.25 Vref | 1                   | Two's Complement     |"));
  Serial.println(F("|      4      |  1  |  0  |  0  | 0 - 2.5 Vref            | 1/1.024             | Straight Binary      |"));
  Serial.println(F("|      5      |  1  |  0  |  1  | 0 - 2.5 Vref            | 1                   | Straight Binary      |"));
  Serial.println(F("|      6      |  1  |  1  |  0  | -2.5 Vref - +2.5 Vref   | 1/1.024             | Two's Complement     |"));
  Serial.println(F("|      7      |  1  |  1  |  1  | -2.5 Vref - +2.5 Vref   | 1                   | Two's Complement     |"));

  Serial.print(F("\n\n\n\t\t\t\tOPTIONS\n"));
  Serial.print(F("\n1 - Display ADC output\n"));
  Serial.print(F("2 - Display configuration setting\n"));
  Serial.print(F("3 - Change configuration setting\n"));

  Serial.print(F("\nENTER A COMMAND: "));
}

//! Displays the ADC output and calculated voltage for all channels
uint8_t menu1_display_adc_output()
{
  uint8_t i, j;
  uint8_t ack = 0;
  int32_t data;
  float voltage;
  uint8_t Result[24];
  uint8_t *p;

  Serial.print("\nEnter the channel number (0 - 7, 8: ALL): ");
  channel = read_int();
  if (channel < 0)
    channel = 0;
  else if (channel > 8)
    channel = 8;

  LTC2348_write(Result);    //discard the first reading
  LTC2348_write(Result);

  if (channel == 8)
  {
    Serial.println("ALL");
    j = 0;
    for (i = 0; i < 8; ++i)
    {
      Serial.print("\nChannel      : ");
      Serial.println(i);

      Serial.print("Data         : ");
      data = 0;
      data = (int32_t)Result[j] << 10;
      data |= (int32_t)Result[j + 1] << 2;
      data |= (int32_t)Result[j + 2] >> 6;
      Serial.print(F("0x"));
      Serial.println(data, HEX);

      Serial.print("Voltage      : ");
      voltage = LTC2348_voltage_calculator(data, i);
      Serial.print(voltage, 6);
      Serial.println(F(" V"));

      Serial.print("Config Number: ");
      Serial.println(Result[j + 2] & CONFIG_WORD_POSITION);
      j = j + 3;
    }
  }
  else
  {
    Serial.println(channel);
    Serial.print("Data         : ");
    data = 0;
    p = &Result[channel * 3];
    data = (int32_t)(*p) << 10;
    data |= (int32_t)(*(p + 1)) << 2;
    data |= (int32_t)(*(p + 2)) >> 6;
    Serial.print(F("0x"));
    Serial.println(data);

    Serial.print("Voltage      : ");
    voltage = LTC2348_voltage_calculator(data, channel);
    Serial.print(voltage, 6);
    Serial.println(F(" V"));

    Serial.print("Config Number: ");
    Serial.println(Result[channel * 3 + 2] & CONFIG_WORD_POSITION);
  }
  return(ack);
}
//! Displays the configuration number of channels
uint8_t menu2_display_channel_CW()
{
  uint8_t i, j;
  uint8_t ack = 0;
  uint8_t channel;
  uint8_t Result[24];

  Serial.print("\nEnter the channel number (0 - 7, 8: ALL): ");
  channel = read_int();
  if (channel < 0)
    channel = 0;
  else if (channel > 8)
    channel = 8;

  LTC2348_write(Result);    //discard the first reading
  LTC2348_write(Result);

  if (channel == 8)
  {
    Serial.println("ALL");
    Serial.print("\nConfig number for each channel:");
    j = 0;
    for (i = 0; i < 8; ++i)
    {
      Serial.print("\n\nChannel      : ");
      Serial.println(i);
      Serial.print("Config Number: ");
      Serial.print(Result[j + 2] & CONFIG_WORD_POSITION);
      j = j + 3;
    }
    Serial.print("\n");
  }
  else
  {
    Serial.println(channel);
    Serial.print("Config Number: ");
    Serial.println(Result[channel * 3 + 2] & CONFIG_WORD_POSITION);
  }
  return(ack);
}

//! Function to change the configuration setting
uint8_t menu3_changeCW()
{
  uint8_t i, j;
  uint8_t ack = 0;
  uint8_t channel;
  uint8_t configNum;
  uint8_t Result[24];

  Serial.print("\nEnter the channel number (0 - 7, 8: ALL): ");
  channel = read_int();
  if (channel < 0)
    channel = 0;
  else if (channel > 8)
    channel = 8;

  if (channel == 8)
    Serial.println("ALL");
  else
    Serial.println(channel);

  Serial.print("Enter the configuration number in decimal: ");
  configNum = read_int();
  Serial.println(configNum);

  if (channel == 8)
  {
    CWSTRUCT.LTC2348_CHAN0_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN1_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN2_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN3_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN4_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN5_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN6_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN7_CONFIG = configNum;
  }
  else
  {
    switch (channel)
    {
      case 0:
        CWSTRUCT.LTC2348_CHAN0_CONFIG = configNum;
        break;
      case 1:
        CWSTRUCT.LTC2348_CHAN1_CONFIG = configNum;
        break;
      case 2:
        CWSTRUCT.LTC2348_CHAN2_CONFIG = configNum;
        break;
      case 3:
        CWSTRUCT.LTC2348_CHAN3_CONFIG = configNum;
        break;
      case 4:
        CWSTRUCT.LTC2348_CHAN4_CONFIG = configNum;
        break;
      case 5:
        CWSTRUCT.LTC2348_CHAN5_CONFIG = configNum;
        break;
      case 6:
        CWSTRUCT.LTC2348_CHAN6_CONFIG = configNum;
        break;
      case 7:
        CWSTRUCT.LTC2348_CHAN7_CONFIG = configNum;
        break;
    }
  }

  Serial.print(F("\nCONFIGURATION CHANGED!"));
  return ack;
}