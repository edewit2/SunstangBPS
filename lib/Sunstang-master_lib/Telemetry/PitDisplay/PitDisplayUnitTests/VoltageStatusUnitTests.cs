using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Sunstang
{
    [TestClass]
    public class VoltageStatusUnitTests
    {
        [TestMethod]
        public void Error_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(0);
            Assert.AreEqual(vs.Meaning, "Unknown status(0)");
        }
        [TestMethod]
        public void PROGRAM_STARTED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(100);
            Assert.AreEqual(vs.Meaning, "PROGRAM_STARTED");
        }

        [TestMethod]
        public void MAIN_RELAY_ENABLED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(101);
            Assert.AreEqual(vs.Meaning, "MAIN_RELAY_ENABLED");
        }

        [TestMethod]
        public void MAIN_RELAY_DISABLED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(102);
            Assert.AreEqual(vs.Meaning, "MAIN_RELAY_DISABLED");
        }

        [TestMethod]
        public void CHARGE_RELAY_ENABLED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(103);
            Assert.AreEqual(vs.Meaning, "CHARGE_RELAY_ENABLED");
        }

        [TestMethod]
        public void CHARGE_RELAY_DISABLED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(104);
            Assert.AreEqual(vs.Meaning, "CHARGE_RELAY_DISABLED");
        }

        [TestMethod]
        public void START_BUTTON_PRESSED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(105);
            Assert.AreEqual(vs.Meaning, "START_BUTTON_PRESSED");
        }

        [TestMethod]
        public void TASK_CHECKBATTERY_RUN_ACTIVE_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(106);
            Assert.AreEqual(vs.Meaning, "TASK_CHECKBATTERY_RUN_ACTIVE");
        }

        [TestMethod]
        public void TASK_CHECKBATTERY_RUN_IDLE_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(107);
            Assert.AreEqual(vs.Meaning, "TASK_CHECKBATTERY_RUN_IDLE");
        }

        [TestMethod]
        public void TASK_UPDATELCD_RUN_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(108);
            Assert.AreEqual(vs.Meaning, "TASK_UPDATELCD_RUN");
        }

        [TestMethod]
        public void TASK_BROADCASTMEASUREMENTS_RUN_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(109);
            Assert.AreEqual(vs.Meaning, "TASK_BROADCASTMEASUREMENTS_RUN");
        }

        [TestMethod]
        public void TASK_BROADCASTSTATE_ACTIVE_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(110);
            Assert.AreEqual(vs.Meaning, "TASK_BROADCASTSTATE_ACTIVE");
        }

        [TestMethod]
        public void TASK_BROADCASTSTATE_IDLE_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(111);
            Assert.AreEqual(vs.Meaning, "TASK_BROADCASTSTATE_IDLE");
        }

        [TestMethod]
        public void FUNCTION_RUN_resetLocalRegisters_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(120);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_resetLocalRegisters");
        }

        [TestMethod]
        public void FUNCTION_RUN_initVoltageSensorSPI_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(121);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_initVoltageSensorSPI");
        }

        [TestMethod]
        public void FUNCTION_RUN_writeConfigurationRegisterGroup_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(122);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_writeConfigurationRegisterGroup");
        }

        [TestMethod]
        public void FUNCTION_RUN_readConfigurationRegisterGroup_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(123);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readConfigurationRegisterGroup");
        }

        [TestMethod]
        public void FUNCTION_RUN_readCellVoltageRegisterGroupA_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(124);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readCellVoltageRegisterGroupA");
        }

        [TestMethod]
        public void FUNCTION_RUN_readCellVoltageRegisterGroupB_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(125);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readCellVoltageRegisterGroupB");
        }

        [TestMethod]
        public void FUNCTION_RUN_readCellVoltageRegisterGroupC_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(126);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readCellVoltageRegisterGroupC");
        }

        [TestMethod]
        public void FUNCTION_RUN_readCellVoltageRegisterGroupD_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(127);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readCellVoltageRegisterGroupD");
        }

        [TestMethod]
        public void FUNCTION_RUN_startCellVoltageADCConversion_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(128);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_startCellVoltageADCConversion");
        }

        [TestMethod]
        public void FUNCTION_RUN_readStatusRegisterGroupA_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(129);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readStatusRegisterGroupA");
        }

        [TestMethod]
        public void FUNCTION_RUN_readStatusRegisterGroupB_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(130);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_readStatusRegisterGroupB");
        }

        [TestMethod]
        public void FUNCTION_RUN_wakeUpSerialInterface_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(131);
            Assert.AreEqual(vs.Meaning, "FUNCTION_RUN_wakeUpSerialInterface");
        }

        [TestMethod]
        public void RECEIVED_PEC_MISMATCH_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(140);
            Assert.AreEqual(vs.Meaning, "RECEIVED_PEC_MISMATCH");
        }

        [TestMethod]
        public void BOARD_OUTDATED_StatusCheck()
        {
            VoltageStatus vs = new VoltageStatus(141);
            Assert.AreEqual(vs.Meaning, "BOARD_OUTDATED");
        }



    }
}
