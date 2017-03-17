/*!
Linear Technology DC980A/B Demonstration Board Control

LTC2970: Dual I2C Power Supply Monitor and Margining Controller

@verbatim
http://www.linear.com/product/LTC2970

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC2970

http://www.linear.com/demo/#demoboards

REVISION HISTORY
$Revision: 4037 $
$Date: 2015-09-22 10:20:48 -0600 (Tue, 22 Sep 2015) $

Copyright (c) 2015, Linear Technology Corp.(LTC)
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
!*/

/*! @file
    @ingroup LTC2970
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SMBusNoPec.h"
#include "LTC2970.h"

#define LTC2970_I2C_ADDRESS 0x5B //global 7-bit address
//#define LTC2970_I2C_ADDRESS 0x6F //SLAVE_HH 7-bit address


/****************************************************************************/
// Global variables
static uint8_t ltc2970_i2c_address;

static LT_SMBusNoPec *smbus = new LT_SMBusNoPec();

uint16_t dac0_value, dac0_ctrl;
uint16_t servo0_value;
uint16_t servo0_value_marg;
uint16_t servo0_value_nom;
uint16_t idac0_reg;
uint16_t dac1_value, dac1_ctrl;
uint16_t servo1_value;
uint16_t servo1_value_marg;
uint16_t servo1_value_nom;
uint16_t idac1_reg;


// definitions of information about the system
static float    dac_step_size = 0.193; // volts per DAC step
static uint16_t dac_max_code = 0x0087; // maximum allowed DAC code
static uint16_t dac_min_code = 0x0049; // minimum allowed DAC code

static float    adc_step_size = 0.003980; // volts per ADC step
static uint16_t adc_max_code = 0x18B7; // maximum allowed ADC reading
static uint16_t adc_min_code = 0x0AE0; // minimum allowed ADC reading

static uint16_t SOFT_CONNECT_DELAY = 1000; // milliseconds to wait for soft connect

/****************************************************************************/
//! Initialize Linduino
//! @return void
void setup()
{
  uint16_t return_val;

  // initialize the i2c port
  //  i2c_enable();

  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  print_prompt();

  ltc2970_i2c_address = LTC2970_I2C_ADDRESS;

  dac0_value = 0x0084;
  dac0_ctrl = 0x0000;
  idac0_reg = dac0_ctrl + dac0_value;
  dac1_value = 0x0084;
  dac1_ctrl = 0x0000;
  idac1_reg = dac1_ctrl + dac1_value;

  servo0_value = 0x2733;
  servo1_value = 0x1A24;
  servo0_value_nom = 0x2733;
  servo1_value_nom = 0x1A24;
  servo0_value_marg = 0x2347; // 10% low
  servo1_value_marg = 0x1786; // 10% low

  //************************  init_voltage_transition();
}

//! Main Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint16_t return_val;

  //  uint16_t dac_value_start, dac_value_end;

  int i = 0;

  if (Serial.available())                //! Checks for user input
  {
    user_command = read_int();         //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)              //! Prints the appropriate submenu
    {

      case 1 :
        Serial.print(F("\n****INITIALIZING THE LTC2970****\n"));
        ltc2970_configure();
        break;

      case 2 :
        Serial.print(F("\n****ENABLE LTC2970 CHANNEL 0 AND CHANNEL 1****\n"));
        ltc2970_dac_disconnect(0);
        ltc2970_gpio_up(0);

        ltc2970_dac_disconnect(1);
        ltc2970_gpio_up(1);
        break;

      case 3 :
        Serial.print(F("\n****SOFT CONNECT LTC2970 DAC0 and DAC1****\n"));
        ltc2970_soft_connect_dac(0);
        ltc2970_soft_connect_dac(1);
        break;

      case 4 :
        Serial.print(F("\n****SERVO CHANNEL 0 and 1 VOLTAGES 10% LOW****\n"));
        ltc2970_servo_to_adc_val(0, servo0_value_marg);
        ltc2970_servo_to_adc_val(1, servo1_value_marg);
        break;

      case 5 :
        Serial.print(F("\n****SERVO CHANNEL 0 and 1 VOLTAGES TO NOMINAL****\n"));
        ltc2970_servo_to_adc_val(0, servo0_value_nom);
        ltc2970_servo_to_adc_val(1, servo1_value_nom);
        break;

      case 6 :
        Serial.print(F("\n****ADC CH_0 VOLTAGE =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_A_ADC);
        Serial.println(((return_val & 0x7FFF)*500e-6), DEC);
        Serial.println(return_val, HEX);

        Serial.print(F("\n****ADC CH_1 VOLTAGE =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH1_A_ADC);
        Serial.println(((return_val & 0x7FFF)*500e-6), DEC);
        Serial.println(return_val, HEX);
        break;

      case 7 :
        Serial.print(F("\n****ADC CH_0 CURRENT =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_B_ADC);
        Serial.println((((return_val & 0x7FFF)*500e-6)/0.007), DEC);
        Serial.println(return_val, HEX);

        Serial.print(F("\n****ADC CH_1 CURRENT =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH1_B_ADC);
        Serial.println((((return_val & 0x7FFF)*500e-6)/0.008), DEC);
        Serial.println(return_val, HEX);
        break;

      case 8 :
        Serial.print(F("\n****PRINT FAULTS, CLEAR LATCHED FAULTS \n"));
        ltc2970_read_faults();
        break;

      case 9 :
        Serial.print(F("\n****PRINT DIE TEMPERATURE \n"));
        ltc2970_print_die_temp ();
        break;

      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }
}


/************************************************************************/
// Function Definitions

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n***************************************************************\n"));
  Serial.print(F("* DC980 Regulator Control Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program provides a simple interface to control the       *\n"));
  Serial.print(F("* the DC980 regulators through the LTC2970                      *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n"));
  Serial.print(F("  1  - Reset the LTC2970, Disable Regulators\n"));
  Serial.print(F("  2  - Enable Channel 0 and Channel 1; DACs disconnected\n"));
  Serial.print(F("  3  - Soft-Connect DAC0 and DAC1, and Confirm Connection\n"));
  Serial.print(F("  4  - Servo Channel 0 and Channel 1 Voltages 10% low\n"));
  Serial.print(F("  5  - Servo Channel 0 and Channel 1 Voltages to nominal\n"));
  Serial.print(F("  6  - Print Channel 0 & 1 Voltages\n"));
  Serial.print(F("  7  - Print Channel 0 & 1 Currents\n"));
  Serial.print(F("  8  - Print Fault Register Contents\n"));
  Serial.print(F("  9  - Print LTC2970 Temperature\n"));
  Serial.print(F("\nEnter a command number:"));
}


//! Writes configuration values to the LTC2970 registers
//! @return void
void ltc2970_configure()
{
  uint16_t return_val;
  //start the 2970 by configuring all of its registers for this application
  // use SMbus commands
  smbus->writeWord(ltc2970_i2c_address, LTC2970_FAULT_EN, 0x0168);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, 0x000A);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_ADC_MON, 0x007F);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_VDD_OV, 0x2CEC);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_VDD_UV, 0x2328);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_V12_OV, 0x3FFF);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_V12_UV, 0x00000);

  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_OV, 0x2AF8);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_UV, 0x2328);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_SERVO, 0x0000);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, 0x0084);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_B_OV, 0x3FFF);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_B_UV, 0x0000);

  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_OV, 0x1C5D);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_UV, 0x1770);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_SERVO, 0x0000);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, 0x0084);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_B_OV, 0x3FFF);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_B_UV, 0x0000);
}

//! Read FAULT, FAULT_LA, and FAULT_LA_INDEX registers
//! @return void
void ltc2970_read_faults()
{
  uint16_t return_val;

  return_val =
    smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
  Serial.print(F("\n LTC2970_FAULT: "));
  Serial.println(return_val, HEX);
  return_val =
    smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT_LA);
  Serial.print(F("\n LTC2970_FAULT_LA: "));
  Serial.println(return_val, HEX);
  return_val =
    smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT_LA_INDEX);
  Serial.print(F("\n LTC2970_FAULT_LA_INDEX: "));
  Serial.println(return_val, HEX);

}

//! Set GPIO_n high
//! @return void
void ltc2970_gpio_up(int gpio_number)
{
  uint16_t return_val;
  if (gpio_number == 0)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val | 0xFEEC) | 0x0012;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else if (gpio_number == 1)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val | 0xFED3) | 0x0028;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else
  {
    // error, no such GPIO
  }
}

//! Set GPIO_n low
//! @return void
void ltc2970_gpio_down(int gpio_number)
{
  uint16_t return_val;
  if (gpio_number == 0)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val | 0xFEEC) | 0x0010;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else if (gpio_number == 1)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val | 0xFED3) | 0x0020;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else
  {
    // error, no such GPIO
  }
}

//! Unceremoniously connect DAC0 to the control node
//!  no attempt to equalize voltages
//! @return void
void ltc2970_hard_connect_dac(int dac_number)
{
  uint16_t return_val;
  if (dac_number == 0)
  {
    // use the global DAC variables
    Serial.print(F("\nHARD CONNECT CHANNEL 0 : "));
    dac0_ctrl = 0x0300;
    idac0_reg = dac0_ctrl + (0x00FF | dac0_value);

    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, idac0_reg);
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_A_IDAC);
    Serial.println(return_val, HEX);
  }
  else if (dac_number == 1)
  {
    // use the global DAC variables
    Serial.print(F("\nHARD CONNECT CHANNEL 1 : "));
    dac1_ctrl = 0x0300;
    idac1_reg = dac1_ctrl + (0x00FF | dac1_value);

    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, idac1_reg);
  }
  else
  {
    Serial.print(F("\nERROR CANNOT HARD CONNECT NON-EXISTANT CHANNEL"));
    // error, no such DAC
  }
}

//! soft-connect DACn to its controlled node
//! @return int
int ltc2970_soft_connect_dac(int dac_number)
{
  uint16_t return_val;
  if (dac_number == 0)
  {
    // check for existing faults
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
    //    if ((return_val & 0x001F) == 0x0000) {
    if ((return_val & 0x001B) == 0x0000)
    {
      // make sure that the channel is not already connected
      return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_A_IDAC);
      if ((return_val & 0x0300) == 0x0000)
      {
        // the soft-connect operation can succeed with no faults, setting IDAC[9] = 1
        // or it can fail, and not set IDAC[9]
        // we wait a safe amount of time, then check for results
        Serial.print(F("\nSOFT CONNECT CHANNEL 0 : "));
        dac0_ctrl = 0x0100;
        dac0_value = 0x0080;
        idac0_reg = dac0_ctrl + (0x00FF | dac0_value);
        smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, idac0_reg);
        delay(SOFT_CONNECT_DELAY);
        return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_A_IDAC);
        Serial.println(return_val, HEX);
        if ((return_val & 0x0300) == 0x0000)
        {
          Serial.print(F("\nCHANNEL 0 FAILED TO CONNECT"));
          Serial.print(F("\n  FAULT REGISTER: "));
          return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
          Serial.println(return_val, HEX);
        }
        else
        {
          Serial.print(F("\nCHANNEL 0 SOFT CONNECT SUCCESS"));
          return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_A_ADC);
          servo0_value = (return_val & 0x7FFF);
        }
      }
      else
      {
        Serial.print(F("\nCHANNEL 0 ALREADY CONNECTED"));
      }
    }
    else
    {
      Serial.print(F("\nERROR: CANNOT SOFT-CONNECT WITH FAULTS ON CHANNEL 0: "));
      Serial.println(return_val, HEX);
    }
  }
  else if (dac_number == 1)
  {
    // check for existing faults
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
    //    if ((return_val & 0x03E0) == 0x0000) {
    if ((return_val & 0x0360) == 0x0000)
    {
      // make sure that the channel is not already connected
      return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH1_A_IDAC);
      if ((return_val & 0x0300) == 0x0000)
      {
        // the soft-connect operation can succeed with no faults, setting IDAC[9] = 1
        // or it can fail, and not set IDAC[9]
        // we wait a safe amount of time, then check for results
        Serial.print(F("\nSOFT CONNECT CHANNEL 1 : "));
        dac1_ctrl = 0x0100;
        dac1_value = 0x0080;
        idac1_reg = dac1_ctrl + (0x00FF | dac1_value);
        smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, idac1_reg);
        delay(SOFT_CONNECT_DELAY);
        return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH1_A_IDAC);
        Serial.println(return_val, HEX);
        if ((return_val & 0x0300) == 0x0000)
        {
          Serial.print(F("\nCHANNEL 1 FAILED TO CONNECT"));
          Serial.print(F("\n  FAULT REGISTER: "));
          return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
          Serial.println(return_val, HEX);
        }
        else
        {
          Serial.print(F("\nCHANNEL 1 SOFT CONNECT SUCCESS"));
          return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH1_A_ADC);
          servo1_value = (return_val & 0x7FFF);
        }
      }
      else
      {
        Serial.print(F("\nCHANNEL 1 ALREADY CONNECTED"));
      }
    }
    else
    {
      Serial.print(F("\nERROR: CANNOT SOFT-CONNECT WITH FAULTS ON CHANNEL 1: "));
      Serial.println(return_val, HEX);
    }
  }
  else
  {
    Serial.print(F("\nERROR: CANNOT HARD CONNECT NON-EXISTANT CHANNEL"));
    // error, no such DAC
  }
  Serial.print(F("\n\n"));
}


//! Disconnect a DAC from its channel
//! @return void
void ltc2970_dac_disconnect(int dac_number)
{
  uint16_t return_val;
  if (dac_number == 0)
  {
    Serial.print(F("\nDISCONNECT CHANNEL 0 : "));
    dac0_ctrl = 0x0000;
    idac0_reg = dac0_ctrl + (0x00FF | dac0_value);

    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, idac0_reg);
  }
  else if (dac_number == 1)
  {
    Serial.print(F("\nDISCONNECT CHANNEL 1 : "));
    dac1_ctrl = 0x0000;
    idac1_reg = dac1_ctrl + (0x00FF | dac1_value);

    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, idac1_reg);
  }
  else
  {
    Serial.print(F("\nERROR CANNOT DISCONNECT NON-EXISTANT CHANNEL"));
    // error, no such DAC
  }
}





//! Servo once to a given ADC value
//! @return void
void ltc2970_servo_to_adc_val(int channel_number, uint16_t code)
{
  uint16_t return_val;

  uint16_t code_in;
  uint16_t code_max = 0x18B7,
           code_min = 0x0AE0;

  // get rid of bit 15 to make calculations easier
  code_in = (code & 0x7FFF);

  //  code_in = (code_in > code_max) ? code_max : code_in;
  //  code_in = (code_in < code_min) ? code_min : code_in;

  // ensure that bit 15 is high to enable servoing
  code_in = (code_in + 0x8000);

  if (channel_number == 0)
  {
    Serial.print(F("\nSERVO CHANNEL 0  "));
    servo0_value = code_in;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_SERVO, code_in);

    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_CH0_A_SERVO);
    Serial.println(return_val, HEX);
  }
  else if (channel_number == 1)
  {
    Serial.print(F("\nSERVO CHANNEL 1  "));
    Serial.println(code_in, HEX);
    servo1_value = code_in;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_SERVO, code_in);
  }
  else
  {
    Serial.print(F("\nERROR CANNOT SERVO NON-EXISTANT CHANNEL"));
    // error, no such channel
  }
}

//! Prints die temperature on the LTC2970
//! @return void
void ltc2970_print_die_temp ()
{
  static float temp_scale = 4;
  static float temp_offset = 1093; //adc codes

  float temperature;
  uint16_t return_val;
  //print the on-die temperature for the LTC2970
  return_val =
    smbus->readWord(ltc2970_i2c_address, LTC2970_TEMP_ADC);
  return_val = return_val & 0x7FFF; // drop bit 15

  temperature = ((float(return_val) - temp_offset) / temp_scale);

  Serial.print(F("\n LTC_2970 DIE TEMP: "));
  Serial.println(temperature, DEC);
  Serial.println(return_val, HEX);
}
