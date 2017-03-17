using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sunstang
{
    public class VoltageStatus
    {
        public byte statusCode{
            get;
            set;
        }

        public String Meaning
        {
            get{
                switch(statusCode){
                    case 100:
                        return "PROGRAM_STARTED";
                    case 101:
                        return "MAIN_RELAY_ENABLED";
                    case 102:
                        return "MAIN_RELAY_DISABLED";
                    case 103:
                        return "CHARGE_RELAY_ENABLED";
                    case 104:
                        return "CHARGE_RELAY_DISABLED";
                    case 105:
                        return "START_BUTTON_PRESSED";
                    case 106:
                        return "TASK_CHECKBATTERY_RUN_ACTIVE";
                    case 107:
                        return "TASK_CHECKBATTERY_RUN_IDLE";
                    case 108:
                        return "TASK_UPDATELCD_RUN";
                    case 109:
                        return "TASK_BROADCASTMEASUREMENTS_RUN";
                    case 110:
                        return "TASK_BROADCASTSTATE_ACTIVE";
                    case 111:
                        return "TASK_BROADCASTSTATE_IDLE";
                    case 120:
                        return "FUNCTION_RUN_resetLocalRegisters";
                    case 121:
                        return "FUNCTION_RUN_initVoltageSensorSPI";
                    case 122:
                        return "FUNCTION_RUN_writeConfigurationRegisterGroup";
                    case 123:
                        return "FUNCTION_RUN_readConfigurationRegisterGroup";
                    case 124:
                        return "FUNCTION_RUN_readCellVoltageRegisterGroupA";
                    case 125:
                        return "FUNCTION_RUN_readCellVoltageRegisterGroupB";
                    case 126:
                        return "FUNCTION_RUN_readCellVoltageRegisterGroupC";
                    case 127:
                        return "FUNCTION_RUN_readCellVoltageRegisterGroupD";
                    case 128:
                        return "FUNCTION_RUN_startCellVoltageADCConversion";
                    case 129:
                        return "FUNCTION_RUN_readStatusRegisterGroupA";
                    case 130:
                        return "FUNCTION_RUN_readStatusRegisterGroupB";
                    case 131:
                        return "FUNCTION_RUN_wakeUpSerialInterface";
                    case 140:
                        return "RECEIVED_PEC_MISMATCH";
                    case 141:
                        return "BOARD_OUTDATED";
                    default:
                        return "Unknown status(" + statusCode +")";
                }
            }
        }

        public VoltageStatus(byte code)
        {
            statusCode = code;
        }

    }
}
