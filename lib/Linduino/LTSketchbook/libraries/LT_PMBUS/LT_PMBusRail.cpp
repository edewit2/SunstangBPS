/*!
LTC PMBusRail Support

@verbatim

This PMBus code does not cover the entire PMBus command set. The intention
is to cover the common commands. It was written for LTC PMBus devices,
and may not perform properly with other PMBus devices.

@endverbatim

REVISION HISTORY
$Revision:  $
$Date: $

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

//! @defgroup LT_PMBusRail LT_PMBusRail: PMBus Rail commands


/*! @file
    @ingroup LT_PMBusRail
    Library File for LT_PMBusRail
*/
#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "Linduino.h"
#include "LT_PMBusRail.h"

LT_PMBusRail::LT_PMBusRail (LT_PMBus *pmbus, uint8_t railAddress, tRailDef *railDef)
{
  pmbus_ = pmbus;
  railAddress_ = railAddress;
  railDef_ = railDef;
}

/*
 * Set the output voltage of a polyphase rail
 *
 * voltage: the target voltage
 */
void LT_PMBusRail::setVout(float voltage)
{
  // Add phases will move via the rail address.
  pmbus_->setVout(railAddress_, voltage);
}

/*
 * Read the input voltage of a polyphase rail
 *
 * polling: poll if true
 * return: the input voltage
 */
float LT_PMBusRail::readVin(bool polling)
{
  // This assumes that the VIN of all physical devices share
  // the same VIN.
  return pmbus_->readVin(railDef_[0].address, polling);
}

/*
 * Read the output voltage of a polyphase rail
 *
 * polling: poll if true
 * return: the output voltage
 */
float LT_PMBusRail::readVout(bool polling)
{
  // All VOUTs are connected, so any physical address and
  // page will do.
  pmbus_->setPage(railDef_[0].address, railDef_[0].pages[0]);
  return pmbus_->readVout(railDef_[0].address, polling);
}

/*
 * Read the input current of a polyphase rail
 *
 * polling: poll if true
 * return: the input current
 */
float LT_PMBusRail::readIin(bool polling)
{
  float current = 0.0;
  int i = 0;

  // Add up inputs from all physical devices. This
  // may include rail/phases that are not part of the rail.
  while (railDef_[i].address != 0)
  {
    for (int j = 0; j < railDef_[i].noOfPages; j++)
    {
      pmbus_->setPage(railDef_[i].address, railDef_[i].pages[j]);
      current += pmbus_->readIin(railDef_[i].address, polling);
    }
    i++;
  }

  return current;
}

/*
 * Read the output current of a polyphase rail
 *
 * polling: poll if true
 * return: the output current
 */
float LT_PMBusRail::readIout(bool polling)
{
  float current = 0.0;
  int i = 0;

  // Add up all phases. There will not be any unwanted phases.
  while (railDef_[i].address != 0)
  {
    for (int j = 0; j < railDef_[i].noOfPages; j++)
    {
      pmbus_->setPage(railDef_[i].address, railDef_[i].pages[j]);
      current += pmbus_->readIout(railDef_[i].address, polling);
    }
    i++;
  }

  return current;
}

/*
 * Read the input power of a polyphase rail
 *
 * polling: poll if true
 * return: the output current
 */
float LT_PMBusRail::readPin(bool polling)
{
  float power = 0.0;
  int i = 0;

  // Add up inputs from all physical devices. This
  // may include rail/phases that are not part of the rail.
  while (railDef_[i].address != 0)
    power += pmbus_->readPin(railDef_[i++].address, polling);

  return power;
}

/*
 * Read the output power of a polyphase rail
 *
 * polling: poll if true
 * return: the output power
 */
float LT_PMBusRail::readPout(bool polling)
{
  float power = 0.0;
  int i = 0;

  // Add up all phases. There will not be any unwanted phases.
  while (railDef_[i].address != 0)
  {
    for (int j = 0; j < railDef_[i].noOfPages; j++)
    {
      pmbus_->setPage(railDef_[i].address, railDef_[i].pages[j]);
      power += pmbus_->readPout(railDef_[i].address, polling);
    }
    i++;
  }

  return power;
}

/*
 * Read the status word of a polyphase rail
 *
 * return: status word
 */
uint16_t LT_PMBusRail::readStatusWord()
{
  uint16_t sw = 0.0;
  int i = 0;

  // Combine all words. Assumes 1 = notification, so that anything
  // with a 1 is interesting.
  while (railDef_[i].address != 0)
  {
    for (int j = 0; j < railDef_[i].noOfPages; j++)
    {
      pmbus_->setPage(railDef_[i].address, railDef_[i].pages[j]);
      sw |= pmbus_->readStatusWord(railDef_[i].address);
    }
    i++;
  }

  return sw;
}

/*
 * Clear faults of polyphase rail
 */
void LT_PMBusRail::clearFaults()
{
  int i = 0;
  int j = 0;

  pmbus_->startGroupProtocol();

  while (railDef_[i].address != 0)
    pmbus_->clearFaults(railDef_[i++].address);

  pmbus_->executeGroupProtocol();
}

/*
 * Sequence on a polyphase rail
 */
void LT_PMBusRail::sequenceOn()
{
  pmbus_->sequenceOn(&railAddress_, 1);
}

/*
 * Turn off a polyphase rail
 */
void LT_PMBusRail::immediateOff()
{
  pmbus_->immediateOff(&railAddress_, 1);
}

/*
 * Sequence off a polyphase rail
 *
 */
void LT_PMBusRail::sequenceOff()
{
  pmbus_->sequenceOff(&railAddress_, 1);
}

/*
 * Margin high polyphase rail
 */
void LT_PMBusRail::marginHigh()
{
  pmbus_->marginHigh(railAddress_);
}

/*
 * Margin low polyphase rail
 *
 */
void LT_PMBusRail::marginLow()
{
  pmbus_->marginLow(railAddress_);
}

/*
 * Margin off polyphase rail
 *
 */
void LT_PMBusRail::marginOff()
{
  pmbus_->marginOff(railAddress_);
}