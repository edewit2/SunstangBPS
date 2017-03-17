/******************************************************************************
*
* Copyright (c) 2012-2014 Linear Technology Corporation (LTC)
* All Rights Reserved
*
*******************************************************************************
*
* Subject to Customer’s acceptance of the following terms and conditions, both
* redistribution of this software by Customer in binary form only and internal
* use of this software by Customer in source form, with or without modification,
* are permitted:
*
* - LTC has granted use rights to Customer in writing.
*
* - Any redistributions by Customer in binary form must include the above
*   copyright notice, this list of conditions and the following disclaimer in
*   the documentation and/or other materials provided with the redistribution.
*
* - Permission for internal use of the Software in source form is strictly
*   limited to Customer only and its controlled subsidiaries. Contractors,
*   consultants and all other third parties must seek and obtain separate
*   written use rights from LTC.
*
* - Other than for the above copyright notice and the below disclaimer, neither
*   the name of Linear Technology, Inc., Linear Technology Corporation (LTC)
*   nor the names of any of their contributors may be used by Customer for any
*   purpose, including, without limitation, the endorsement or promotion of any
*   products derived from this software without the fully informed, specific,
*   prior written permission from LTC.
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY LTC "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL LTC OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/


/*! @file
    @ingroup LTPSM_InFlightUpdate
    Library File
*/

#include "hex_file_parser.h"

uint8_t ascii_to_hex(uint8_t ascii)
{
  return 0;
}

uint8_t filter_terminations(uint8_t (*get_data)(void))
{
  uint8_t c = '\0';

  c = get_data();
  while (c == '\n' || c == '\r')
    c = get_data();
  return c;
}

uint8_t detect_colons(uint8_t (*get_data)(void))
{
  uint8_t c;

  c = get_data();
  if (c == ':')
  {
    Serial.print(".");
  }
  return c;
}

/*
 * Parse a list of hex file bytes returning a list of ltc record bytes
 *
 * in_data:   buffer of hex data
 * in_length: length of the hex data
 * out_data:  preallocated buffer where the output data will be put
 * return:    actual size of the output data
 *
 * Notes:     CRC is ignored. No error processing; original data must be correct.
 *        Copies char input data so that it can be const, and frees when done.
 *        It is best to use const input so that memory usage is only temporary.
 */
uint16_t parse_hex_block(char *in_data, uint16_t in_length, uint8_t *out_data)
{
  uint16_t    in_position;
  uint16_t    out_position;
  uint8_t     start_code;
  uint16_t    byte_count;
  uint16_t    record_type;
  uint32_t    address;
  uint16_t    crc;
  uint8_t     save;
  uint16_t    i;

  in_position = 0;
  out_position = 0;

  while (1)
  {
    // Get colon
    start_code = in_data[in_position];
    in_position += 1;

    save = in_data[in_position+2];
    in_data[in_position+2] = '\0';
    byte_count = httoi(&in_data[in_position]);
    in_data[in_position+2] = save;
    in_position += 2;

    save = in_data[in_position+4];
    in_data[in_position+4] = '\0';
    address = httoi(&in_data[in_position]);
    in_data[in_position+4] = save;
    in_position += 4;

    save = in_data[in_position+2];
    in_data[in_position+2] = '\0';
    record_type = httoi(&in_data[in_position]);
    in_data[in_position+2] = save;
    in_position += 2;


    for (i = 0; i < byte_count; i++)
    {
      save = in_data[in_position+2];
      in_data[in_position+2] = '\0';
      out_data[out_position] = httoi(&in_data[in_position]);
      in_data[in_position+2] = save;
      in_position += 2;

      out_position += 1;
    }


    save = in_data[in_position+2];
    in_data[in_position+2] = '\0';
    crc = httoi(&in_data[in_position]);
    in_data[in_position+2] = save;
    in_position += 2;

    if (in_position >= in_length)
      break;
  }
  return out_position;
}

static uint8_t *parse_data = NULL;
static uint16_t parse_data_length = 0;
static uint16_t parse_data_position = 0;

void reset_parse_hex()
{
  if (parse_data != NULL)
    free(parse_data);
  parse_data = NULL;
  parse_data_length = 0;
  parse_data_position = 0;
}

/*
 * Parse a list of hex file bytes returning a list of ltc record bytes
 *
 * in_data:   Function to get one line of data to parse.
 * return:    One byte of data
 *
 * Notes:     CRC is ignored. No error processing; original data must be correct.
 *        Copies char input data so that it can be const, and frees when done.
 *        It is best to use const input so that memory usage is only temporary.
 */
uint8_t parse_hex(uint8_t (*get_data)(void))
{
  uint16_t    out_position;
  uint8_t     start_code;
  uint16_t    byte_count;
  uint16_t    record_type;
  uint32_t    address;
  uint16_t    crc;
  uint8_t     save;
  uint16_t    i;
  char      data[5];

  if (parse_data_position == parse_data_length)
  {
    if (parse_data != NULL)
      free(parse_data);

    // Get colon
    start_code = 0x00;
    while (start_code != ':')
      start_code = get_data();

    data[0] = get_data();
    data[1] = get_data();
    data[2] = '\0';
    byte_count = httoi(data);

    data[0] = get_data();
    data[1] = get_data();
    data[2] = get_data();
    data[3] = get_data();
    data[4] = '\0';
    address = httoi(data);

    data[0] = get_data();
    data[1] = get_data();
    data[2] = '\0';
    record_type = httoi(data);

    if (record_type == 0)
    {
      parse_data = (uint8_t *)malloc(byte_count);
      for (i = 0; i < byte_count; i++)
      {
        data[0] = get_data();
        data[1] = get_data();
        data[2] = '\0';
        parse_data[i] = httoi(data);
      }

      data[0] = get_data();
      data[1] = get_data();
      data[2] = '\0';
      crc = httoi(data);

      parse_data_position = 0;
      parse_data_length = byte_count;
    }
    else if (record_type == 1)
    {
      parse_data = (uint8_t *)malloc(4);
      parse_data[0] = 4;
      parse_data[1] = 0;
      parse_data[2] = 0x22;
      parse_data[3] = 0;
      parse_data_position = 0;
      parse_data_length = 4;
    }
  }
  return parse_data[parse_data_position++];
}

uint16_t parse_records(uint8_t *in_data, uint16_t in_length, tRecordHeaderLengthAndType **out_records)
{
  uint16_t    in_position;
  uint16_t  out_position;
  tRecordHeaderLengthAndType *record;

  in_position = 0;
  out_position = 0;

  while (1)
  {
    record = (tRecordHeaderLengthAndType *) &in_data[in_position];
    *(out_records+out_position) = record;
    in_position += (*out_records[out_position]).Length;
    out_position += 1;

    if (in_position >= in_length)
      break;
  }

  return out_position;
}

pRecordHeaderLengthAndType parse_record(uint8_t (*get_data)(void))
{
  uint32_t  header;
  uint8_t   *data;
  uint16_t  size;
  uint16_t  pos;
  pRecordHeaderLengthAndType record;

  header = (uint32_t)get_data() << 0 | (uint32_t)get_data() << 8 | (uint32_t)get_data() << 16 | (uint32_t)get_data() << 24;

  record = (pRecordHeaderLengthAndType) &header;
  size = record->Length;
  data = (uint8_t *)malloc(size + 4);

  data[0] = (header >> 0) & 0xFF;
  data[1] = (header >> 8) & 0xFF;
  data[2] = (header >> 16) & 0xFF;
  data[3] = (header >> 24) & 0xFF;

  for (pos = 0; pos < size - 4; pos++)
    data[pos + 4] = get_data();

  record = (pRecordHeaderLengthAndType) data;
  return record;
}

pRecordHeaderLengthAndType print_record(pRecordHeaderLengthAndType (*get_record)(void))
{
  pRecordHeaderLengthAndType record = get_record();

  switch (record->RecordType)
  {
    case 0x01:
      Serial.println(F("processWriteByteOptionalPEC(t_RECORD_PMBUS_WRITE_BYTE*);"));
      break;
    case 0x02:
      Serial.println(F("processWriteWordOptionalPEC(t_RECORD_PMBUS_WRITE_WORD*);"));
      break;
    case 0x04:
      Serial.println(F("processReadByteExpectOptionalPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT*);"));
      break;
    case 0x05:
      Serial.println(F("processReadWordExpectOptionalPEC(t_RECORD_PMBUS_READ_WORD_EXPECT*);"));
      break;
    case 0x09:
      Serial.println(F("bufferNVMData(t_RECORD_NVM_DATA*);"));
      break;
    case 0x0A:
      Serial.println(F("processReadByteLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK*);"));
      break;
    case 0x0B:
      Serial.println(F("processReadWordLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK*);"));
      break;
    case 0x0C:
      Serial.println(F("processPollReadByteUntilAckNoPEC(t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK*);"));
      break;
    case 0x0D:
      Serial.println(F("processDelayMs(t_RECORD_DELAY_MS*);"));
      break;
    case 0x0E:
      Serial.println(F("processSendByteOptionalPEC(t_RECORD_PMBUS_SEND_BYTE*);"));
      break;
    case 0x0F:
      Serial.println(F("processWriteByteNoPEC(t_RECORD_PMBUS_WRITE_BYTE_NOPEC*);"));
      break;
    case 0x10:
      Serial.println(F("processWriteWordNoPEC(t_RECORD_PMBUS_WRITE_WORD_NOPEC*);"));
      break;
    case 0x12:
      Serial.println(F("processReadByteExpectNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC*);"));
      break;
    case 0x13:
      Serial.println(F("processReadWordExpectNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC*);"));
      break;
    case 0x15:
      Serial.println(F("processReadByteLoopMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC*);"));
      break;
    case 0x16:
      Serial.println(F("processReadWordLoopMaskNoPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC*);"));
      break;
    case 0x17:
      Serial.println(F("processSendByteNoPEC(t_RECORD_PMBUS_SEND_BYTE_NOPEC*);"));
      break;
    case 0x18:
      Serial.println(F("processEvent(t_RECORD_EVENT*);"));
      break;
    case 0x19:
      Serial.println(F("processReadByteExpectMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC*);"));
      break;
    case 0x1A:
      Serial.println(F("processReadWordExpectMaskNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC*);"));
      break;
    case 0x1B:
      Serial.println(F("processVariableMetaData(t_RECORD_VARIABLE_META_DATA*);"));
      break;
    case 0x1C:
      Serial.println(F("modifyWordNoPEC(t_RECORD_PMBUS_MODIFY_WORD_NO_PEC*);"));
      break;
    case 0x1D:
      Serial.println(F("modifyByteNoPEC(t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC*);"));
      break;
    case 0x1E:
      Serial.println(F("writeNvmData(t_RECORD_NVM_DATA*);"));
      break;
    case 0x1F:
      Serial.println(F("read_then_verifyNvmData(t_RECORD_NVM_DATA*);"));
      break;
    case 0x20:
      Serial.println(F("modifyByteOptionalPEC(t_RECORD_PMBUS_MODIFY_BYTE*);"));
      break;
    case 0x21:
      Serial.println(F("modifyWordOptionalPEC(t_RECORD_PMBUS_MODIFY_WORD*);"));
      break;
    default :
      Serial.print(F("Unknown Record Type "));
      Serial.println(record->RecordType, HEX);
      break;
  }

  return record;
}
