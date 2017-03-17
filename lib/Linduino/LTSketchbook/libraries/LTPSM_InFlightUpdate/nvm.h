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
    Library Header File for NVM
*/


#ifndef NVM_H_
#define NVM_H_

#include <stdbool.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "../LT_PMBUS/LT_PMBus.h"
#include "../LT_PMBUS/LT_SMBusNoPec.h"
#include "../LT_PMBUS/LT_SMBusPec.h"
#include "../LT_PMBUS/LT_I2CBus.h"
#include "main_record_processor.h"
#include "hex_file_parser.h"


extern LT_PMBus *pmbus__;         //!< global accessor for IFU helper files
extern LT_SMBusNoPec *smbusNoPec__;   //!< global accessor for IFU helper files
extern LT_SMBusPec *smbusPec__;     //!< global accessor for IFU helper files


// extern uint8_t get_hex_data(void);

//! Data retrieving method.
extern uint8_t get_hex_data(void);
//! Data retrieving method.
extern uint8_t get_record_data(void);
//! Data retrieving method.
extern pRecordHeaderLengthAndType get_record(void);

class NVM
{
  private:
    uint8_t *addr;
    uint8_t numAddrs;

  public:
    //! Constructor.
    NVM(LT_PMBus *,   //!< reference to pmbus object for this library to use
        LT_SMBusNoPec *, //!< reference to no pec smbus object
        LT_SMBusPec *,  //!< reference to pec smbus object
        uint8_t *,    //!< i2c addresses on the board to program.
        uint8_t     //!< number of addresses
       );

    //! Destructor
    ~NVM();

    //! Program with hex data.
    //! @return true if data loaded.
    bool programWithData(const unsigned char * //!< array of hex data
                        );

    //! Verifies board NVM with hex data..
    //! @return true if NVM configuration matches the hex data.
    bool verifyWithData(const unsigned char *);

};

#endif /* NVM_H_ */
