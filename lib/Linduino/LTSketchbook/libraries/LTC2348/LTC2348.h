
/*!
LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC

@verbatim

The LTC2348 is an 18-bit,low noise 8-channel simultaneous sampling successive
approximation register(SAR) ADC with differential,wide common mode range
inputs. Operating from a 5V low voltage supply, flexible high voltage supplies,
and using the internal reference and buffer, each channel of this SoftSpan ADC
can be independently configured on a conversion-by-conversion basis to accept
±10.24V, 0V to 10.24V, ±5.12V,or 0V to 5.12V signals. Individual channels may
also be disabled to increase throughput on the remaining channels

SPI DATA FORMAT :
            SoftSpan Configuration Word for Conversion N+1
Data In  :  S23 S22 S21 S20 S19 S18 S17 S16 S15 S14 S13 S12 S11 S10 S9  S8  S7  S6  S5  S4  S3  S2  S1  S0

            Conversion N   Channel 0
SDO0   :  D17 D16 D15 D14 D13 D12 D11 D10 D9  D8  D7  D6  D5  D4  D3  D2  D1  D0  C2  C1  C0  SS2 SS1 SS0

Sx   : SoftSpan Configuration Bits
Dx   : Data Bits
Cx   : Channel ID Bits
SSx  : SoftSpan Configuration Bits of corresponding channel

@endverbatim

http://www.linear.com/product/LTC2348

http://www.linear.com/product/LTC2348#demoboards

REVISION HISTORY
$Revision: 3659 $
$Date: 2013-07-15

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

/*! @file
    @ingroup LTC2348
    Header for LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC
*/

#ifndef LTC2348_H
#define LTC2348_H

//! SoftSpan Configuration Bits for 8 channels (3 bits each)
struct Config_Word_Struct
{
  uint8_t LTC2348_CHAN0_CONFIG : 3;
  uint8_t LTC2348_CHAN1_CONFIG : 3;
  uint8_t LTC2348_CHAN2_CONFIG : 3;
  uint8_t LTC2348_CHAN3_CONFIG : 3;
  uint8_t LTC2348_CHAN4_CONFIG : 3;
  uint8_t LTC2348_CHAN5_CONFIG : 3;
  uint8_t LTC2348_CHAN6_CONFIG : 3;
  uint8_t LTC2348_CHAN7_CONFIG : 3;
};

extern Config_Word_Struct CWSTRUCT;

//! Calculates the voltage from ADC output data depending on the channel configuration
float LTC2348_voltage_calculator(int32_t data,    //!< ADC output data
                                 uint8_t channel  //!< channel number
                                );

//! Creates 24-bit configuration word for the 8 channels.
uint32_t LTC2348_create_config_word();

//! Transmits 24 bits (3 bytes) of configuration information and
//! reads back 24 bytes of data (3 bytes/ 24 bits for each channel)
//! 24 bits: 18 bit data + 3 bit config + 3 bit channel number
//! Read back is done in a new cycle
void LTC2348_write(uint8_t Result[24]       //!< 3 bytes each of ADC output data from 8 channels
                  );

#endif