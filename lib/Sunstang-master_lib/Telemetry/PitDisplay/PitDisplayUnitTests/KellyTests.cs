using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Sunstang
{
    [TestClass]
    public class KellyTests
    {
        [TestMethod]
        public void BrakeAndThrottleConversion()
        {
            const byte input = 255;
            float result = KellyData.ThrottleAndBrakeA2DtoV(input);

            Assert.AreEqual(5, result, 0.0001, "Brake A2D conversion incorrect");
        }

        [TestMethod]
        public void HighVoltageConversion()
        {
            const byte input = 200;
            float result = KellyData.HighVoltageA2DtoV(input);

            Assert.AreEqual(144, result, 0.719, "High Voltage A2D conversion incorrect");
        }

        [TestMethod]
        public void SupplyVoltageConversion()
        {
            const byte input = 134;
            float result = KellyData.SupplyVoltageConversion(input);

            Assert.AreEqual(5.25, result, 1f/28, "Supply A2D conversion incorrect");
        }

        [TestMethod]
        public void RejectUnknownIDs()
        {
            byte[] data = {};

            canFrame frame = new canFrame(0, data, DateTime.Now);

            KellyData d = new KellyData();

            bool isAccepted = d.Parse(frame);

            Assert.IsFalse(isAccepted, "ID 0 was accepted, and shouldn't have been");
        }

        [TestMethod]
        public void AcceptRequestIDs()
        {
            byte[] data = {};
            canFrame requestFrame = new canFrame(352, data, DateTime.Now);
            KellyData d = new KellyData();

            bool isAccepted = d.Parse(requestFrame);

            Assert.IsTrue(isAccepted, "The request wasn't accepted");
        }

        [TestMethod]
        public void RejectResponseWithoutRequest()
        {
            byte[] data = {};
            canFrame responseFrame = new canFrame(353, data, DateTime.Now);
            KellyData d = new KellyData();

            bool isAccepted = d.Parse(responseFrame);

            Assert.IsFalse(isAccepted, "The response was accepted without a request");
        }

        [TestMethod]
        public void AcceptCCP_A2D_BATCH_READ1()
        {
            byte[] reqData = {0x1b};
            byte[] resData = {0, 255, 0, 134, 139};
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellyA2D1EventArgs eventData = null;

            d.AddedA2D1Response += delegate(object sender, KellyA2D1EventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.BrakeRegenPosition.Count, "Brake Regen Position not added");
            Assert.AreEqual(0, d.BrakeRegenPosition.First.Value.Value, "Incorrect brake position value");
            Assert.AreEqual(0, eventData.BrakeRegenPosition.Value, "Incorret Brake Position set in event");

            Assert.AreEqual(1, d.ThrottlePosition.Count, "Throttle regen position not added.");
            Assert.AreEqual(5, d.ThrottlePosition.First.Value.Value, "Incorrect throttle position value");
            Assert.AreEqual(5, eventData.ThrottlePosition.Value, "Incorrect throttle position value in event");

            Assert.AreEqual(1, d.OperationVoltage.Count, "Operation Voltage not added");
            Assert.AreEqual(0, d.OperationVoltage.First.Value.Value, "Incorrect Operation Voltage Added");
            Assert.AreEqual(0, eventData.OperationVoltage.Value, "Incorrect Operation Voltage Added to event");

            Assert.AreEqual(1, d.SupplyVoltage.Count, "Supply Voltage not added");
            Assert.AreEqual(5.25, d.SupplyVoltage.First.Value.Value, "Supply voltage incorrectly set");
            Assert.AreEqual(5.25, eventData.SupplyVoltage.Value, "Supply voltage incorrectly set in event");

            Assert.AreEqual(1, d.BPlusVoltage.Count, "B+ Voltage not set");
            Assert.AreEqual(100, d.BPlusVoltage.First.Value.Value, "B+ value set incorrectly");
            Assert.AreEqual(100, eventData.BPlusVoltage.Value, "B+ value set incorrectly in event");
        }

        [TestMethod]
        public void AcceptCCP_A2D_BATCH_READ2()
        {
            byte[] reqData = {0x1a};
            byte[] resData = {0, 1, 2, 0, 125, 139};
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellyA2D2EventArgs eventData = null;

            d.AddedA2D2Response += delegate(object sender, KellyA2D2EventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.CurrentPhaseA.Count, "Current A not added correctly");
            Assert.AreEqual(0, d.CurrentPhaseA.First.Value.Value, "Current  A incorrectly set");
            Assert.AreEqual(0, eventData.CurrentPhaseA.Value, "Current  A incorrectly set in event");

            Assert.AreEqual(1, d.CurrentPhaseB.Count, "Current B not added correctly");
            Assert.AreEqual(1, d.CurrentPhaseB.First.Value.Value, "Current  B incorrectly set");
            Assert.AreEqual(1, eventData.CurrentPhaseB.Value, "Current  B incorrectly set in event");

            Assert.AreEqual(1, d.CurrentPhaseC.Count, "Current C not added correctly");
            Assert.AreEqual(2, d.CurrentPhaseC.First.Value.Value, "Current  C incorrectly set");
            Assert.AreEqual(2, eventData.CurrentPhaseC.Value, "Current  C incorrectly set in event");

            Assert.AreEqual(1, d.VoltagePhaseA.Count, "Voltage A not added");
            Assert.AreEqual(0, d.VoltagePhaseA.First.Value.Value, 1/1.39, "Voltage A incorrectly set");
            Assert.AreEqual(0, eventData.VoltagePhaseA.Value, 1/1.39, "Voltage A incorrectly set in event");

            Assert.AreEqual(1, d.VoltagePhaseB.Count, "Voltage B not added");
            Assert.AreEqual(89.928, d.VoltagePhaseB.First.Value.Value, 1/1.39, "Voltage B incorrectly set");
            Assert.AreEqual(89.928, eventData.VoltagePhaseB.Value, 1/1.39, "Voltage B incorrectly set in event");

            Assert.AreEqual(1, d.VoltagePhaseC.Count, "Voltage C not added");
            Assert.AreEqual(100, d.VoltagePhaseC.First.Value.Value, 1/1.39, "Voltage C incorrectly set");
            Assert.AreEqual(100, eventData.VoltagePhaseC.Value, 1/1.39, "Voltage C incorrectly set in event");
        }

        [TestMethod]
        public void AcceptCCP_MONITOR1()
        {
            byte[] reqData = {0x33};
            byte[] resData = {0, 1, 2, 3, 4, 5};
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellyMonitor1EventArgs eventData = null;

            d.AddedMonitor1Response += delegate(object sender, KellyMonitor1EventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.Pwm.Count, "PWM not added correctly");
            Assert.AreEqual(0, d.Pwm.First.Value.Value, "PWM incorrectly set");
            Assert.AreEqual(0, eventData.PWM.Value, "PWM incorrectly set in event");

            Assert.AreEqual(1, d.MotorRotationEnabled.Count, "Enable rotation not added correctly");
            Assert.AreEqual(true, d.MotorRotationEnabled.First.Value.Value, "Enabled rotation incorrectly set");
            Assert.AreEqual(true, eventData.MotorRotationEnabled.Value, "Enabled rotation incorrectly set in event");

            Assert.AreEqual(1, d.MotorTemperature.Count, "Motor Temp not added correctly");
            Assert.AreEqual(2, d.MotorTemperature.First.Value.Value, "Motor temp incorrectly set");
            Assert.AreEqual(2, eventData.MotorTemperature.Value, "Motor temp incorrectly set in event");

            Assert.AreEqual(1, d.ControllerTemperature.Count, "Controller Temp not added correctly");
            Assert.AreEqual(3, d.ControllerTemperature.First.Value.Value, "Controller Temp incorrectly set");
            Assert.AreEqual(3, eventData.ControllerTemp.Value, "Controller Temp incorrectly set in event");

            Assert.AreEqual(1, d.MosfetTemperatureHighSide.Count, "High side mosfet not added correctly");
            Assert.AreEqual(4, d.MosfetTemperatureHighSide.First.Value.Value, "High side mosfet incorrectly set");
            Assert.AreEqual(4, eventData.MosfetTempHighSide.Value, "high side mosfet incorrectly set in event");

            Assert.AreEqual(1, d.MosfetTemperatureLowSide.Count, "Low side mosfet not added correctly");
            Assert.AreEqual(5, d.MosfetTemperatureLowSide.First.Value.Value, "Low side mosfet incorrectly set");
            Assert.AreEqual(5, eventData.MosfetTempLowSide.Value, "Low side mosfet incorrectly set in event");
        }

        [TestMethod]
        public void AcceptCCP_MONITOR2()
        {
            byte[] reqData = {0x37};
            byte[] resData = {1, 2, 3, 4, 8};
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellyMonitor2EventArgs eventData = null;

            d.AddedMonitor2Response += delegate(object sender, KellyMonitor2EventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.RPM.Count, "RPM not added correctly");
            Assert.AreEqual(0x0102, d.RPM.First.Value.Value, "RPM incorrectly set");
            Assert.AreEqual(0x0102, eventData.RPM.Value, "RPM incorrectly set in event");

            Assert.AreEqual(1, d.CurrentPercent.Count, "Current Percent not added correctly");
            Assert.AreEqual(3, d.CurrentPercent.First.Value.Value, "Current Percent incorrectly set");
            Assert.AreEqual(3, eventData.CurrentPercent.Value, "Current Percent incorrectly set in event");

            Assert.AreEqual(1, d.ErrorCode.Count, "Error Code not added correctly");
            Assert.AreEqual(0x0408, d.ErrorCode.First.Value.Value, "Error Code incorrectly set");
            Assert.AreEqual(0x0408, eventData.ErrorCode.Value, "Error Code incorrectly set in event");
        }

        [TestMethod]
        public void AcceptCOM_SW_ACC()
        {
            byte[] reqData = {0x42};
            byte[] resData = {1};
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellySwitchEventArgs eventData = null;

            d.addedThrottleSwitchResponse += delegate(object sender, KellySwitchEventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.ThrottleSwitch.Count, "Throttle value not added correctly");
            Assert.AreEqual(true, d.ThrottleSwitch.First.Value.Value, "throttle value incorrectly set");
            Assert.AreEqual(true, eventData.Value.Value, "Throttle value incorrectly set in event");
        }

        [TestMethod]
        public void AcceptCOM_SW_BRK()
        {
            byte[] reqData = { 0x43 };
            byte[] resData = { 1 };
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellySwitchEventArgs eventData = null;

            d.addedBrakeSwitchResponse += delegate(object sender, KellySwitchEventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.BrakeSwitch.Count, "Brake value not added correctly");
            Assert.AreEqual(true, d.BrakeSwitch.First.Value.Value, "Brake value incorrectly set");
            Assert.AreEqual(true, eventData.Value.Value, "Brake value incorrectly set in event");
        }

        [TestMethod]
        public void AcceptCOM_SW_REV()
        {
            byte[] reqData = { 0x44 };
            byte[] resData = { 1 };
            canFrame requestFrame = new canFrame(352, reqData, DateTime.Now);
            canFrame responseFrame = new canFrame(353, resData, DateTime.Now);
            KellyData d = new KellyData();
            KellySwitchEventArgs eventData = null;

            d.addedReverseSwitchResponse += delegate(object sender, KellySwitchEventArgs e)
            {
                eventData = e;
            };

            bool isRequestAccepted = d.Parse(requestFrame);
            bool isResponseAccepted = d.Parse(responseFrame);

            Assert.IsNotNull(eventData, "Event never fired.");
            Assert.IsTrue(isRequestAccepted, "The request wasn't accepted");
            Assert.IsTrue(isResponseAccepted, "The response wasn't accepted");

            Assert.AreEqual(1, d.ReverseSwitch.Count, "Reverse value not added correctly");
            Assert.AreEqual(true, d.ReverseSwitch.First.Value.Value, "Reverse value incorrectly set");
            Assert.AreEqual(true, eventData.Value.Value, "Revesrs value incorrectly set in event");
        }

    }
}
