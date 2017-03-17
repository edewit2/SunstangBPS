/*!
LTC SMBus Support: API for a LTC3880 Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 3997 $
$Date: 2015-09-17 11:33:59 -0700 (Thu, 17 Sep 2015) $

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

//! @defgroup LT_3380FaultLog LT_3880FaultLog

/*! @file
    @ingroup LT_3880FaultLog
    Library Header File for LT_3880FaultLog
*/

#include <Arduino.h>
#include "LT_3880FaultLog.h"
#include "LT_FaultLog.h"

#define _RAW_EEPROM

LT_3880FaultLog::LT_3880FaultLog(LT_SMBus *smbus):LT_FaultLog(smbus)
{

  faultLog3880 = NULL;

  buffer = NULL;
}


/*
 * Read LTC3880 fault log
 *
 * address: PMBUS address
 */
void
LT_3880FaultLog::read(uint8_t address)
{
#ifdef RAW_EEPROM

  // Notes:
  //
  // Polling BUSY without dealing with errors would be dangerous in a real app.
  // It could result in an infinite loop. Caveat emptor.

  uint8_t *data = (uint8_t *) malloc(160); // Only 147 get used.
  data[0] = 0x00;
  bool pecMatch = true;

  // Tell the device to get ready and set the index at the beginning of EEPROM
  while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
  smbus_->writeByte(address, 0xBD, 0x2B);
  while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
  smbus_->writeByte(address, 0xBD, 0x91);
  while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
  smbus_->writeByte(address, 0xBD, 0xE4);
  // Read the ID
  while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
  uint16_t id = smbus_->readWord(address, 0xBF);
  // Read the size and ingore it because we know it will not change
  while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
  uint16_t size = smbus_->readWord(address, 0xBF);
  // Consume words we need to ignore
  for (int i = 0; i < 176; i++)
  {
    while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
    uint16_t w = smbus_->readWord(address, 0xBF);
  }
  // Consume words of the fault log and check the CRC
  int pos = 0;
  smbus_->pecClear();
  for (int i = 0; i < 80; i++)
  {
    while (smbus_->readByte(address, 0xEF) & 0x40 == 0); // Wait until not busy
    uint16_t w = smbus_->readWord(address, 0xBF);
    // Toss CRC every 32 bytes
    if ((i + 1) % 16 == 0)
    {
      data[pos] = 0xFF & w; // If this is not obvious, consider the endianess. This is the 32nd byte.
      smbus_->pecAdd(data[pos]);
      pos++;
      uint8_t calcPec = smbus_->pecGet();
      uint8_t devPec = 0xFF & (w >> 8); // This depends on endianess as well.
      smbus_->pecClear();
      if (calcPec != devPec)
        pecMatch = false;
//            printf("PEC 0x%x 0x%x\n", calcPec, devPec);
    }
    else   // And endianess applies here too.
    {
      data[pos] = 0xFF & w;
      smbus_->pecAdd(data[pos]);
      pos++;
      data[pos] = 0xFF & (w >> 8);
      smbus_->pecAdd(data[pos]);
      pos++;
    }
  }
  // It is possible to have a partial good/bad CRC. The device will not load any Fault Log data
  // to RAM if there is ANY CRC mismatch. Then the read Fault Log will return all zero data or
  // random data depending on revision of silicon. This code mimics the most recent silicon
  // and returns all zeros if there is a CRC mismatch.
  if (!pecMatch)
    memset (data, 0x00, 150);
#else
  uint8_t *data = (uint8_t *) malloc(147);
  data[0] = 0x00;

  smbus_->readBlock(address, MFR_FAULT_LOG, data, 147);
#endif
  faultLog3880 = (struct LT_3880FaultLog::FaultLogLtc3880 *) (data);
}


void LT_3880FaultLog::release()
{
  free(faultLog3880);
  faultLog3880 = 0;
}


void LT_3880FaultLog::dumpBinary(Print *printer)
{
  dumpBin(printer, (uint8_t *)faultLog3880, 147);
}

void LT_3880FaultLog::print(Print *printer)
{
  if (printer == 0)
    printer = &Serial;
  buffer = new char[FILE_TEXT_LINE_MAX];

  printTitle(printer);

  printTime(printer);

  printPeaks(printer);

  printAllLoops(printer);


  delete [] buffer;
}


void LT_3880FaultLog::printTitle(Print *printer)
{
  printer->print(F("LTC3880 Log Data\n"));

  uint8_t position = faultLog3880->preamble.position_fault;
  switch (position)
  {
    case 0xFF :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position MFR_FAULT_LOG_STORE\n"));
      break;
    case 0x00 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position TON_MAX_FAULT Channel 0\n"));
      break;
    case 0x01 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_OV_FAULT Channel 0\n"));
      break;
    case 0x02 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_UV_FAULT Channel 0\n"));
      break;
    case 0x03 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position IOUT_OC_FAULT Channel 0\n"));
      break;
    case 0x05 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position OT_FAULT Channel 0\n"));
      break;
    case 0x06 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position UT_FAULT Channel 0\n"));
      break;
    case 0x07 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VIN_OV_FAULT Channel 0\n"));
      break;
    case 0x0A :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position MFR_OT_FAULT Channel 0\n"));
      break;
    case 0x10 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position TON_MAX_FAULT Channel 1\n"));
      break;
    case 0x11 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_OV_FAULT Channel 1\n"));
      break;
    case 0x12 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_UV_FAULT Channel 1\n"));
      break;
    case 0x13 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position IOUT_OC_FAULT Channel 1\n"));
      break;
    case 0x15 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position OT_FAULT Channel 1\n"));
      break;
    case 0x16 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position UT_FAULT Channel 1\n"));
      break;
    case 0x17 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VIN_OV_FAULT Channel 1\n"));
      break;
    case 0x1A :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position MFR_OT_FAULT Channel 1\n"));
      break;
  }

  printer->print(buffer);
}

void LT_3880FaultLog::printTime(Print *printer)
{
  uint8_t *time = (uint8_t *)&faultLog3880->preamble.shared_time;
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Time 0x%02x%02x%02x%02x%02x%02x\n"), time[5], time[4], time[3], time[2], time[1], time[0]);
  printer->print(buffer);
  printer->print((long) getSharedTime200us(faultLog3880->preamble.shared_time));
  printer->println(F(" Ticks (200us each)"));
}

void LT_3880FaultLog::printPeaks(Print *printer)
{
  printer->println(F("\nHeader Information:\n--------"));

  printer->print(F("VOUT Peak 0 "));
  printer->println(math_.lin16_to_float(getLin16WordReverseVal(faultLog3880->preamble.peaks.mfr_vout_peak_p0), 0x14), 6);
  printer->print(F("VOUT Peak 1 "));
  printer->println(math_.lin16_to_float(getLin16WordReverseVal(faultLog3880->preamble.peaks.mfr_vout_peak_p1), 0x14), 6);
  printer->print(F("IOUT Peak 0 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.mfr_iout_peak_p0)), 6);
  printer->print(F("IOUT Peak 1 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.mfr_iout_peak_p1)), 6);
  printer->print(F("VIN Peak "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.mfr_vin_peak)), 6);
  printer->print(F("Temp External Last Event Page 0 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.read_temperature_1_p0)), 6);
  printer->print(F("Temp External Last Event Page 1 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.read_temperature_1_p1)), 6);
  printer->print(F("Temp Internal Last Event "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.read_temperature_2)), 6);
  printer->print(F("Temp External Peak Page 0 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.mfr_temperature_1_peak_p0)), 6);
  printer->print(F("Temp External Peak Page 1 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->preamble.peaks.mfr_temperature_1_peak_p1)), 6);
}

void LT_3880FaultLog::printAllLoops(Print *printer)
{
  printer->println(F("\nFault Log Loops Follow:"));
  printer->println(F("(most recent data first)"));

  for (int index = 0; index < 6; index++)
  {
    printLoop(index, printer);
  }
}

void LT_3880FaultLog::printLoop(uint8_t index, Print *printer)
{
  printer->println(F("-------"));
  printer->print(F("Loop: "));
  printer->println(index);
  printer->println(F("-------"));

  printer->print(F("Input: "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->fault_log_loop[index].read_vin)), 6);
  printer->print(F(" V, "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->fault_log_loop[index].read_iin)), 6);
  printer->println(F(" A"));
  printer->print(F("Chan0: "));
  printer->print(math_.lin16_to_float(getLin16WordReverseVal(faultLog3880->fault_log_loop[index].read_vout_p0), 0x14), 6);
  printer->print(F(" V, "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->fault_log_loop[index].read_iout_p0)), 6);
  printer->println(F(" A"));
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT: 0x%02x\n"), getRawByteVal(faultLog3880->fault_log_loop[index].status_vout_p0));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR_SPECIFIC: 0x%02x\n"), getRawByteVal(faultLog3880->fault_log_loop[index].status_mfr_specificP0));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_WORD: 0x%04x\n"), getRawWordReverseVal(faultLog3880->fault_log_loop[index].status_word_p0));
  printer->print(buffer);
  printer->print(F("Chan1: "));
  printer->print(math_.lin16_to_float(getLin16WordReverseVal(faultLog3880->fault_log_loop[index].read_vout_p1), 0x14), 6);
  printer->print(F(" V, "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3880->fault_log_loop[index].read_iout_p1)), 6);
  printer->println(F(" A"));
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT: 0x%02x\n"), getRawByteVal(faultLog3880->fault_log_loop[index].status_vout_p1));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR_SPECIFIC: 0x%02x\n"), getRawByteVal(faultLog3880->fault_log_loop[index].status_mfr_specificP1));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_WORD: 0x%04x\n"), getRawWordReverseVal(faultLog3880->fault_log_loop[index].status_word_p1));
  printer->print(buffer);
}
