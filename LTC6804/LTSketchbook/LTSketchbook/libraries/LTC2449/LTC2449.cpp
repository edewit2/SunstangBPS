
/*!
LTC2449: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution.
LTC2442: 24-Bit, 4-Channel Delta Sigma ADC with Integrated Amplifier
LTC2448: 24-Bit, 8-/16-Channel Delta Sigma ADCs with Selectable Speed/Resolution

@verbatim

The LTC2444/LTC2445/LTC2448/LTC2449 are 8-/16-channel (4-/8-differential)
high speed 24-bit No Latency Delta Sigma ADCs. They use a proprietary
delta-sigma architecture enabling variable speed/resolution. Through a
simple 4-wire serial interface, ten speed/resolution combinations
6.9Hz/280nVRMS to 3.5kHz/25uVRMS (4kHz with external oscillator) can be
selected with no latency between conversion results or shift in DC accuracy
(offset, full-scale, linearity, drift). Additionally, a 2X speed mode can
be selected enabling output rates up to 7kHz (8kHz if an external
oscillator is used) with one cycle latency.

@endverbatim

http://www.linear.com/product/LTC2449
http://www.linear.com/product/LTC2442
http://www.linear.com/product/LTC2448

http://www.linear.com/product/LTC2449#demoboards
http://www.linear.com/product/LTC2442#demoboards
http://www.linear.com/product/LTC2448#demoboards

REVISION HISTORY
$Revision: 4777 $
$Date: 2016-03-14 11:30:22 -0700 (Mon, 14 Mar 2016) $

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
*/

//! @defgroup LTC2449 LTC2449: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution

/*! @file
    @ingroup LTC2449
    Library for LTC2449: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2449.h"
#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2449_CS
#define LTC2449_CS QUIKEVAL_CS
#endif

int8_t LTC2449_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
// Checks for EOC with a specified timeout (ms)
{
  uint16_t timer_count = 0;             // Timer count for MISO
  output_low(cs);                       //! 1) Pull CS low
  while (1)                             //! 2) Wait for SDO (MISO) to go low
  {
    if (input(MISO) == 0) break;        //! 3) If SDO is low, break loop
    if (timer_count++>miso_timeout)     // If timeout, return 1 (failure)
    {
      output_high(cs);                  // Pull CS high
      return(1);
    }
    else
      delay(1);
  }
  output_high(cs);                  // Pull CS high
  return(0);
}

// Reads from LTC2449.
void LTC2449_read(uint8_t cs, uint16_t adc_command, uint32_t *adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC2449 data and command
  LT_union_int16_2bytes temp_comm;
  temp_comm.LT_int16 = adc_command;
  command.LT_byte[3] = temp_comm.LT_byte[1];
  command.LT_byte[2] = temp_comm.LT_byte[0];
  command.LT_byte[1] = 0;
  command.LT_byte[0] = 0;

  output_low(cs);                                                       //! 1) Pull CS low

  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)4);    //! 2) Transfer arrays

  output_high(cs);                                                      //! 3) Pull CS high
  *adc_code = data.LT_int32;
}

// Calculates the voltage corresponding to an adc code, given lsb weight (in volts) and the calibrated
// adc offset code (zero code that is subtracted from adc_code).
float LTC2449_code_to_voltage(int32_t adc_code, float LTC2449_lsb, int32_t LTC2449_offset_code)
{
  float adc_voltage;
  adc_code -= 536870912;                                            //! 1) Converts offset binary to binary
  adc_voltage=(float)(adc_code+LTC2449_offset_code)*LTC2449_lsb;    //! 2) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}

// Calculate the lsb weight and offset code given a full-scale code and a measured zero-code.
void LTC2449_cal_voltage(int32_t zero_code, int32_t fs_code, float zero_voltage, float fs_voltage, float *LTC2449_lsb, int32_t *LTC2449_offset_code)
{
  zero_code -= 536870912;   //! 1) Converts zero code from offset binary to binary
  fs_code -= 536870912;     //! 2) Converts full scale code from offset binary to binary

  float temp_offset;
  *LTC2449_lsb = (fs_voltage-zero_voltage)/((float)(fs_code - zero_code));                              //! 3) Calculate the LSB

  temp_offset = (zero_voltage/ *LTC2449_lsb) - zero_code;                                               //! 4) Calculate Unipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 5) Round
  *LTC2449_offset_code = (int32_t)temp_offset;                                                          //! 6) Cast as int32_t
}
