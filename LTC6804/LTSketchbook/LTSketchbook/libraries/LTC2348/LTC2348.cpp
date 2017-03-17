/*!
LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC.
LTC2358: 18-bit, Buffered 8-channel Simultaneous Sampling SAR ADC.

@verbatim

The LTC2348 is an 18-bit,low noise 8-channel simultaneous sampling successive
approximation register(SAR) ADC with differential,wide common mode range
inputs. Operating from a 5V low voltage supply, flexible high voltage supplies,
and using the internal reference and buffer, each channel of this SoftSpan ADC
can be independently configured on a conversion-by-conversion basis to accept
±10.24V, 0V to 10.24V, ±5.12V,or 0V to 5.12V signals.

@endverbatim

http://www.linear.com/product/LTC2348-18
http://www.linear.com/product/LTC2358-18

http://www.linear.com/product/LTC2348-18#demoboards
http://www.linear.com/product/LTC2358-18#demoboards

REVISION HISTORY
$Revision: 5081 $
$Date: 2016-05-10 09:34:58 -0700 (Tue, 10 May 2016) $

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
***********************************************************/
//! @defgroup LTC2348 LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC

/*! @file
    @ingroup LTC2348
    Library for LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC
*/
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2348.h"

#define VREF 4.096
#define POW2_18 262143
#define POW2_17 131072

// Setting input range of all channels - 0V to 0.5 Vref with no gain compression (SS2 = 0, SS1 = 0, SS0 = 1)
struct Config_Word_Struct CWSTRUCT = { 7, 7, 7, 7, 7, 7, 7, 7};

// Calculates the voltage from ADC output data depending on the channel configuration
float LTC2348_voltage_calculator(int32_t data, uint8_t channel)
{
  float voltage;
  uint8_t CW;
  switch (channel)
  {
    case 0:
      CW = CWSTRUCT.LTC2348_CHAN0_CONFIG;
      break;
    case 1:
      CW = CWSTRUCT.LTC2348_CHAN1_CONFIG;
      break;
    case 2:
      CW = CWSTRUCT.LTC2348_CHAN2_CONFIG;
      break;
    case 3:
      CW = CWSTRUCT.LTC2348_CHAN3_CONFIG;
      break;
    case 4:
      CW = CWSTRUCT.LTC2348_CHAN4_CONFIG;
      break;
    case 5:
      CW = CWSTRUCT.LTC2348_CHAN5_CONFIG;
      break;
    case 6:
      CW = CWSTRUCT.LTC2348_CHAN6_CONFIG;
      break;
    case 7:
      CW = CWSTRUCT.LTC2348_CHAN7_CONFIG;
      break;
  }

  switch (CW)
  {
    case 0:
      voltage = 0;
      break;   // Disable Channel
    case 1:
      voltage = (float)data * (1.25 * VREF / 1.000) / POW2_18;
      break;
    case 2:
      {
        voltage = (float)data * (1.25 * VREF / 1.024) / POW2_17;
        if (voltage > 5.12)
          voltage -= 10.24;
        break;
      }
    case 3:
      {
        voltage = (float)data * (1.25 * VREF / 1.000) / POW2_17;
        if (voltage > 5.12)
          voltage -= 10.24;
        break;
      }
    case 4:
      voltage = (float)data * (2.50 * VREF / 1.024) / POW2_18;
      break;
    case 5:
      voltage = (float)data * (2.50 * VREF / 1.000) / POW2_18;
      break;
    case 6:
      {
        voltage = (float)data * (2.50 * VREF / 1.024) / POW2_17;
        if (voltage > 10.24)
          voltage -= 20.48;
        break;
      }
    case 7:
      {
        voltage = (float)data * (2.50 * VREF / 1.000) / POW2_17;
        if (voltage > 10.24)
          voltage -= 20.48;
        break;
      }
  }
  return voltage;
}

// Creates 24-bit configuration word for the 8 channels.
uint32_t LTC2348_create_config_word()
{
  uint32_t config_word;
  config_word = (uint32_t)CWSTRUCT.LTC2348_CHAN7_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN6_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN5_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN4_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN3_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN2_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN1_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN0_CONFIG;

  return config_word;
}

// Transmits 24 bits (3 bytes) of configuration information and
// reads back 24 bytes of data (3 bytes/ 24 bits for each channel)
// 24 bits: 18 bit data + 3 bit config + 3 bit channel number
// Read back is done in a new cycle
void LTC2348_write(uint8_t Result[24])
{
  int i, j;
  uint32_t config_word;
  config_word = LTC2348_create_config_word();

  output_low(QUIKEVAL_CS);                  //! Pull CS low

  SPI.transfer((uint8_t)(config_word >> 16));   //! Transferring byte 2
  SPI.transfer((uint8_t)(config_word >> 8));    //! Transferring byte 1
  SPI.transfer((uint8_t)config_word);         //! Transferring byte 0

  output_high(QUIKEVAL_CS);                 //! Pull CS high
  delay(100);
  output_low(QUIKEVAL_CS);

  for (i = 0; i < 24; ++i)
    Result[i] = SPI.transfer(0); //! Receiving 24B data by transferring null information

  output_high(QUIKEVAL_CS);
}