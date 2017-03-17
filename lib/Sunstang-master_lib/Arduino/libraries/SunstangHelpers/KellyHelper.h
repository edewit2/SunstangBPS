#define CCP_FLASH_READ  0xF2

#define INFO_MODULE_NAME 64
#define INFO_SOFTWARE_VER 83
#define CAL_TPS_DEAD_ZONE_LOW 4
#define CAL_BRAKE_DEAD_ZONE_LOW 5
#define CAL_TPS_DEAD_ZONE_HIGH 38
#define CAL_BRAKE_DEAD_ZONE_HIGH 39

#define CCP_A2D_BATCH_READ1 0x1B

#define CCP_A2D_BATCH_READ2 0x1A
#define CCP_MONITOR1 0x33
#define CCP_MONITOR2 0x37
#define CCP_INVALID_COMMAND 0xE3

#define COM_SW_ACC 0x42
#define COM_SW_BRK 0x43
#define COM_SW_REV 0x44
#define COM_READING 0

#define KELLY_REQUEST 352
#define KELLY_RESPONSE 353

typedef struct KellyRequest{
	byte data[8];
	byte len;
}KellyRequest;

struct KellyRequest INFO_MODULE_NAME_REQUEST		= {{CCP_FLASH_READ, INFO_MODULE_NAME, 8}, 3};
struct KellyRequest INFO_SOFTWARE_VER_REQUEST		= {{CCP_FLASH_READ, INFO_SOFTWARE_VER, 2}, 3};
struct KellyRequest CAL_TPS_DEAD_ZONE_LOW_REQUEST	= {{CCP_FLASH_READ, CAL_TPS_DEAD_ZONE_LOW, 1}, 3};
struct KellyRequest CAL_BRAKE_DEAD_ZONE_LOW_REQUEST	= {{CCP_FLASH_READ, CAL_BRAKE_DEAD_ZONE_LOW, 1}, 3};
struct KellyRequest CAL_TPS_DEAD_ZONE_HIGH_REQUEST	= {{CCP_FLASH_READ, CAL_TPS_DEAD_ZONE_HIGH, 1}, 3};
struct KellyRequest CAL_BRAKE_DEAD_ZONE_HIGH_REQUES	= {{CCP_FLASH_READ, CAL_BRAKE_DEAD_ZONE_HIGH, 1}, 3};

struct KellyRequest CCP_A2D_BATCH_READ1_REQUEST		= {{CCP_A2D_BATCH_READ1},1};
struct KellyRequest CCP_A2D_BATCH_READ2_REQUEST		= {{CCP_A2D_BATCH_READ2},1};

struct KellyRequest CCP_MONITOR1_REQUEST 			= {{CCP_MONITOR1},1};
struct KellyRequest CCP_MONITOR2_REQUEST 			= {{CCP_MONITOR2},1};

struct KellyRequest COM_SW_ACC_REQUEST				= {{COM_SW_ACC, COM_READING},2};
struct KellyRequest COM_SW_BRK_REQUEST				= {{COM_SW_BRK, COM_READING},2};
struct KellyRequest COM_SW_REV_REQUEST				= {{COM_SW_REV, COM_READING},2};

namespace kelly{

	/** CCP_MONITOR2 **/

	/*motor Controller speed in RPM 8*/
	int16_t getSpeed(byte buff[]){
		return (buff[0] << 8) | buff[1];
	}

	uint16_t getErrorCode(byte buff[])
	{
		return ((buff[3]) << 8) | buff[4]; 
	}

	uint8_t getPresentCurrentPercent(byte buff[]){
		return buff[2];
	}

	//**CCP_MONITOR1 **/

	uint16_t getPWM(byte buff[])
	{
		return buff[0];
	}

	bool isMotorEnabled(byte buff[]){
		return buff[1] == 1;
	}

	uint8_t getMotorTemp(byte buff[])
	{
		return buff[2];
	}

	uint8_t getControllerTemp(byte buff[])
	{
		return buff[3];
	}

	uint8_t getHighFetmosTemp(byte buff[])
	{
		return buff[4];
	}

	uint8_t getLowFetmosTemp(byte buff[])
	{
		return buff[5];
	}

	/** CPP A2D BATCH READ 2 **/

	int8_t getCurrentA(byte buff[]){
		return buff[0];
	}

	int8_t getCurrentB(byte buff[]){
		return buff[1];
	}

	int8_t getCurrentC(byte buff[]){
		return buff[2];
	}

	uint8_t getVoltageA(byte buff[]){
		return buff[3];
	}

	uint8_t getVoltageB(byte buff[]){
		return buff[4];
	}

	uint8_t getVoltageC(byte buff[]){
		return buff[5];
	}

	/** CPP A2D BATCH READ 1 **/

	uint8_t getBrakeA2D(byte buff[])
	{
		return buff[0];
	}

	uint8_t getThrottleA2D(byte buff[])
	{
		return buff[1];
	}

	uint8_t getOperationVolatage(byte buff[])
	{
		return buff[2];
	}

	uint8_t getSupplyVoltage(byte buff[]){
		return buff[3];
	}

	uint8_t getBPlusVoltage(byte buff[]){
		return buff[4];
	}


}