/*!
LTC2607: 16-Bit, Dual Rail-to-Rail DACs with I2C Interface

@verbatim

The LTC2607/LTC2617/LTC2627 are dual 16-, 14- and 12-bit, 2.7V to 5.5V
rail-to-rail voltage output DACs in a 12-lead DFN package. They have built-in
high performance output buffers and are guaranteed monotonic.

These parts establish new board-density benchmarks for 16- and 14-bit DACs and
advance performance standards for output drive and load regulation in single-
supply, voltage-output DACs.

The parts use a 2-wire, I2C compatible serial interface. The
LTC2607/LTC2617/LTC2627 operate in both the standard mode (clock rate of 100kHz)
and the fast mode (clock rate of 400kHz). An asynchronous DAC update pin (LDAC)
is also included.

The LTC2607/LTC2617/LTC2627 incorporate a power-on reset circuit. During power-
up, the voltage outputs rise less than 10mV above zero scale; and after power-
up, they stay at zero scale until a valid write and update take place. The
power-on reset circuit resets the LTC2607-1/LTC2617-1/ LTC2627-1 to mid-scale.
The voltage outputs stay at midscale until a valid write and update takes place.

I2C DATA FORMAT (MSB First):

       Byte #1                             Byte #2                          Byte #3                              Byte #4

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C3 C2 C1 C0  A3  A2 A1 A0  SACK  D15 D14 D13 D12 D11 D10 D9 D8  SACK  D7 D6  D5  D4  D3  D2  D1  D0 SACK STOP

SACK : Slave Acknowlege
SAx  : I2C Address
W    : I2C Write (0)
R    : I2C Read  (1)
SACK : I2C Slave Generated Acknowledge (Active Low)
Cx   : DAC Command Code
Ax   : DAC Address
Dx   : DAC Data Bits
X    : Don't care


Example Code:

Set DAC A to Full Scale.

    dac_code = 0x0FFFF; // Set dac code to full scale

    // Write dac code to the LTC2607 and update dac
    ack = LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_DAC_A, dac_code);

@endverbatim

http://www.linear.com/product/LTC2607

http://www.linear.com/product/LTC2607#demoboards

REVISION HISTORY
 $Revision: 3659 $
 $Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

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

/*! @file
    @ingroup LTC2607
    Header File for LTC2607: 16-Bit, Dual Rail-to-Rail DACs with I2C Interface
*/

#ifndef LTC2607_H
#define LTC2607_H

#include <Wire.h>

/*! @name I2C_Addresses
@{ */

//! I2C address of the LTC2607.
//! Configured with the CA0, CA1, and CA2 pins. See Table 1 of datasheet.
//! Uncomment LTC2607_I2C_ADDRESS to match demo board configuration.
//  Address assignment
// LTC2607 I2C Address                 //  AD2       AD1       AD0
#define LTC2607_I2C_ADDRESS 0x10      //  LOW       LOW       LOW
// #define LTC2607_I2C_ADDRESS 0x11    //  LOW       LOW       Float
// #define LTC2607_I2C_ADDRESS 0x12    //  LOW       LOW       HIGH
// #define LTC2607_I2C_ADDRESS 0x13    //  LOW       Float     LOW
// #define LTC2607_I2C_ADDRESS 0x20    //  LOW       Float     Float
// #define LTC2607_I2C_ADDRESS 0x21    //  LOW       Float     High
// #define LTC2607_I2C_ADDRESS 0x22    //  LOW       HIGH      LOW
// #define LTC2607_I2C_ADDRESS 0x23    //  LOW       HIGH      Float
// #define LTC2607_I2C_ADDRESS 0x30    //  LOW       High      HIGH
// #define LTC2607_I2C_ADDRESS 0x31    //  Float     LOW       LOW
// #define LTC2607_I2C_ADDRESS 0x32    //  Float     LOW       Float
// #define LTC2607_I2C_ADDRESS 0x33    //  Float     LOW       HIGH
// #define LTC2607_I2C_ADDRESS 0x40    //  Float     Float     LOW
// #define LTC2607_I2C_ADDRESS 0x41    //  Float     Float     Float
// #define LTC2607_I2C_ADDRESS 0x42    //  Float     Float     HIGH
// #define LTC2607_I2C_ADDRESS 0x43    //  Float     High      LOW
// #define LTC2607_I2C_ADDRESS 0x50    //  Float     High      Float
// #define LTC2607_I2C_ADDRESS 0x51    //  Float     High      HIGH
// #define LTC2607_I2C_ADDRESS 0x52    //  High      LOW       LOW
// #define LTC2607_I2C_ADDRESS 0x53    //  High      LOW       Float
// #define LTC2607_I2C_ADDRESS 0x60    //  High      LOW       High
// #define LTC2607_I2C_ADDRESS 0x61    //  High      Float     LOW
// #define LTC2607_I2C_ADDRESS 0x62    //  High      Float     Float
// #define LTC2607_I2C_ADDRESS 0x63    //  High      Float     High
// #define LTC2607_I2C_ADDRESS 0x70    //  High      High      LOW
// #define LTC2607_I2C_ADDRESS 0x71    //  High      High      Float
// #define LTC2607_I2C_ADDRESS 0x72    //  High      High      High

//! LTC2607 Global I2C Address.
#define LTC2607_I2C_GLOBAL_ADDRESS 0x73    //  Global Address

/*! @} */
/*! @name DAC Command
@{ */

#define LTC2607_WRITE_COMMAND             0x00 //!< Command to write to internal register of LTC2607, but not update output voltage yet.
#define LTC2607_UPDATE_COMMAND            0x10 //!< Command to update (and power up) LTC2607. Output voltage will be set to the value stored in the internal register by previous write command.
#define LTC2607_WRITE_UPDATE_COMMAND      0x30 //!< Command to write and update (and power up) the LTC2607. The output voltage will immediate change to the value being written to the internal register.
#define LTC2607_POWER_DOWN_COMMAND        0x40 //!< Command to power down the LTC2607.

/*! @} */
/*! @name DAC Address
@{ */

// DAC Address
#define LTC2607_DAC_A                     0x00 //!< Command (and DAC code) will modify DAC A
#define LTC2607_DAC_B                     0x01 //!< Command (and DAC code) will modify DAC B
#define LTC2607_ALL_DACS                  0x0F //!< Command (and DAC code) will modify both DAC A and DAC B

/*! @} */

const float LTC2607_TYPICAL_lsb = 7.6295109E-5; //!< The LTC2607 typical least significant bit value with 5V full-scale
const float LTC2607_TYPICAL_OFFSET = 0;         //!< The LTC2607 typical offset voltage

//! Writes command, DAC address, and DAC code to the LTC2607.
//! Configures command (write, update, power down, etc.), address (DAC A, DAC B, or BOTH), and 16-bit dac_code for output voltage.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2607_write(uint8_t i2c_address,  //!< I2C address of the LTC2607. Configured by CA0, CA1, and CA2 pins. See Table 1 of datasheet.
                     uint8_t dac_command,   //!< DAC command (4-bits) that will be written to LTC2607. (Write, update, power down, etc.)
                     uint8_t dac_address,   //!< DAC address (4-bits) that will be written to LTC2607. Configures DAC A, DAC B, or both.
                     uint16_t dac_code      //!< DAC code that will be written to LTC2607. This configures the output voltage.
                    );

//! Calculates an LTC2607 DAC code for the desired output voltage.
//! Based on the desired output voltage, the offset, and lsb parameters, return the corresponding DAC code that should be written to the LTC2607.
//! @return DAC code for desired output voltage
uint16_t LTC2607_voltage_to_code(float dac_voltage,     //!< Desired DAC output voltage.
                                 float LTC2607_lsb,     //!< The LSB weight. Use calibrated value for best results, otherwise use typical value from datasheet.
                                 int32_t LTC2607_offset //!< The offset voltage. Use calibrated value for best results, otherwise use typical value from datasheet.
                                );

//! Calculates the LTC2607 offset and lsb voltage given two measured voltages and their corresponding DAC codes.
//! Prior to calling this function, write two DAC codes to the LTC2607, and measure the output voltage for each DAC code.
//! When passed the DAC codes and measured voltages as parameters, this function calculates the calibrated lsb and offset values.
void LTC2607_calibrate(uint16_t dac_code1,      //!< DAC Code 1.
                       uint16_t dac_code2,      //!< DAC Code 2.
                       float voltage1,          //!< Measured output voltage corresponding to DAC Code 1.
                       float voltage2,          //!< Measured output voltage corresponding to DAC Code 2.
                       float *LTC2607_lsb,      //!< Overwritten with new calculated lsb voltage.
                       int32_t *LTC2607_offset  //!< Overwritten with new calculated offset voltage.
                      );

#endif  // LTC2607_H
