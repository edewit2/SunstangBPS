/*
Author: Dustin Dobransky
Date:   08/11/13 [dd,mm,yy]
*/

/* Rename PACKETS_TEMPLATE_H to
          PACKETS_<SUBSYSTEM>_H
   without the '<>' brackets
   Example: "PACKETS_BPS_H", "PACKETS_CONTROLS_H"
*/
#ifndef PACKETS_TEMPLATE_H
#define PACKETS_TEMPLATE_H

#include "Packet.h"
#include "inttypes.h"

/* Rename to Packet_<Subsystem>
   Example: "Packet_Bps", "Packet_Controls" */
namespace Packet_Template {
	
	struct Packet_HighPriority {
		uint8_t data1;
		uint8_t data2;
	} packet0;

	struct Packet_MedPriority {
		float longVal;
		int   data[10];
	} packet1;

	/* You may add additional 'low priority' packets here.
	   Follow naming convention above.
	   Example:
	struct Packet_LowPriority {
	    // data types
    } packet2; */

	Packet packetList[] = {
		Packet(0, PACKET_INFO(packet0)), // Highest priority first
		Packet(1, PACKET_INFO(packet1))  // Second highest next, etc.
		/* Add other packets as necessary,
		   increment packet ID by 1 each time.
		   (Each packet must have a unique ID)
		   Example:
		Packet(2, PACKET_INFO(packet2)*/
	};

	/* Provide a unique address for each subsystem:
	SlaveNode slaveNode(<uniqueID>, SETUP(<packetArray>)); */
	SlaveNode slaveNode(0x00, SETUP(packetList));

}

#endif

/*
DESIGNING THE PACKETS
    >> Only critical data goes in high priority packet
        - Battery voltage, motor temperature, vehicle speed, etc.

	>> Less important data goes in medium/low priority packets
	    - Blinker status, cabin temperature, etc.

VARIABLE WIDTHS AND RANGES
    * uint<size>_t --> unsigned number, occupies <size> bits
	* int<size>_t  -->   signed number, occupies <size> bits
	_______________________________________________
	| Datatype |  # of |      Range               |
	|          | Bytes |                          |
	| int8_t   | 1     |   -128 - 127             |
	| uint8_t  | 1     |      0 - 255             |
	| int16_t  | 2     | -32768 - 32767           |
	| uint16_t | 2     |      0 - 65535           |
	| int32_t  | 4     | -2147483648 - 2147483647 |
	| uint32_t | 4     | 0 - 4294967296           |
	| float    | 4     | Decimal, 6 sig. figs.    |
	|_____________________________________________|

	Equivalent primitive types:
	int8_t   -> char
	uint8_t  -> unsigned char
	int16_t  -> int
	uint16_t -> unsigned int
	int32_t  -> long
	uint32_t -> unsigned long
	int64_t  -> long long

VARIABLE TYPE CHOICE GUIDELINES
    >> Use fixed-width variable types when possible.
	    - int8_t, uint8_t, int16_t, uint16_t, int32_t, etc.

	>> Avoid unneccesarily large data types
	    - Use float/double sparingly
		- choose data type to fit range of sensor data
		> Example:
		The Atmega328 has a built-in 10-bit ADC.  This means
		a reading from the ADC will return a number between
		0-1023.
		--> First note the range, 0-1023.  Since there are no
		negative values, an unsigned number is prefered.
		--> Second, look at which variable types support the
		range.  In this case, uint16_t works.
		--> ALTERNATIVELY >> If the data is not very important
		you can round the raw data down to save space:
		i.e. {0,1023}/4 == {0,255}.  This new range of 0-255
		can fit inside a uint8_t.

>> ***IMPORTANT*** : MAXIMUM PACKET SIZE == 30 bytes
    - Each packet may NOT exceed a maximum of 30 bytes
	Example 1: A valid Packet
	struct Packet_OK {
	    float   var0; // 4 bytes
		uint8_t var1; // 1 byte
		char  ch[10]; // 10 * 1 byte = 10 bytes
	};
	Total number of bytes: 4 + 1 + 10 = 15 Bytes --> OK!

	Example 2: An invalid Packet
	struct Packet_BAD {
	    int8_t bigArray[50]; // 50 * 1 byte = 50 bytes
	};
	Total Bytes: 50 Bytes --> NOT OK!
*/
