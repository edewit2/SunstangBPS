using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Sunstang
{
    [TestClass]
    public class MPPTtests
    {
        [TestMethod]
        public void MPPTConstructorInit()
        {
            MPPTData m = new MPPTData();

            Assert.AreEqual(2, m.BatteryVoltageLevelReaced.Length, "Battery voltage level reached should have 2 members");
            Assert.AreEqual(2, m.OverTemperature.Length, "OverTemp should have 2 members");
            Assert.AreEqual(2, m.NoCharge.Length, "No charge should have 2 members");
            Assert.AreEqual(2, m.UnderVoltage.Length, "No charge should have 2 members");
            Assert.AreEqual(2, m.VoltageIn.Length, "Voltage in should have 2 members");
            Assert.AreEqual(2, m.CurrentIn.Length, "Current In should have 2 members");
            Assert.AreEqual(2, m.VoltageOut.Length, "Voltage Out should have 2 members");
            Assert.AreEqual(2, m.Temperature.Length, "Temperature should have 2 members");
        }

        [TestMethod]
        public void testOutOfRangeMPPT()
        {
            MPPTData m = new MPPTData();
            byte[] frameData = { };
            canFrame frame = new canFrame(1, frameData, new DateTime());

            bool result = m.parse(frame);

            Assert.AreEqual(false, result, "MPPT parsed frame it shouldn't have.");
        }

        [TestMethod]
        public void acceptMPPT1()
        {
            MPPTData m = new MPPTData();
            byte[] frameData = {0,0,0,0,0,0,0 };
            canFrame frame = new canFrame(1905, frameData, new DateTime());

            int cellID = -1;
            m.MPPTUpdated += delegate(object sender, EventArgs e)
            {
                Sunstang.MPPTData.MPPTEventArgs MPPTe = e as Sunstang.MPPTData.MPPTEventArgs;
                cellID = MPPTe.MPPT;
            };

            bool result = m.parse(frame);

            Assert.AreEqual(0, cellID, "Incorrect mppt number");
            Assert.AreEqual(true, result, "MPPT parsed frame it shouldn't have.");
        }

        [TestMethod]
        public void acceptMPPT2()
        {
            MPPTData m = new MPPTData();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(1906, frameData, new DateTime());
            int cellID =-1;
            m.MPPTUpdated += delegate(object sender, EventArgs e)
            {
                Sunstang.MPPTData.MPPTEventArgs MPPTe = e as Sunstang.MPPTData.MPPTEventArgs;
                cellID = MPPTe.MPPT;
            };

            bool result = m.parse(frame);

            Assert.AreEqual(1, cellID, "Incorrect mppt number");
            Assert.AreEqual(true, result, "MPPT parsed frame it shouldn't have.");
        }
        
        [TestMethod]
        public void parseMPPT()
        {
            MPPTData m = new MPPTData();
            byte[] frameData = { 0xF0, 0x01, 0x00, 0x02, 0x00, 0x03, 0x04};
            canFrame frame = new canFrame(1906, frameData, new DateTime());

            bool result = m.parse(frame);

            Assert.AreEqual(true, result, "MPPT didn't parse frame");

            Assert.AreEqual(true, m.OverTemperature[1].First.Value.Value, "Wrong value added to Over Temperature");
            Assert.AreEqual(true, m.NoCharge[1].First.Value.Value, "Wrong value added to Nocharge");
            Assert.AreEqual(true, m.UnderVoltage[1].First.Value.Value, "Wrong value added to undervoltage");

            Assert.AreEqual(1, m.VoltageIn[1].First.Value.Value, "Incorrected Voltage in");
            Assert.AreEqual(2, m.CurrentIn[1].First.Value.Value, "incorrect Current in");
            Assert.AreEqual(3, m.VoltageOut[1].First.Value.Value, "Incorrect Voltage out");
            Assert.AreEqual(4, m.Temperature[1].First.Value.Value, "Incorrect temperature");
        }

        [TestMethod]
        public void testEventMPPT()
        {
            MPPTData m = new MPPTData();
            byte[] frameData = { 0xF0, 0x01, 0x00, 0x02, 0x00, 0x03, 0x04 };
            canFrame frame = new canFrame(1906, frameData, new DateTime());

            bool overTemp = false, noCharge = false, underVoltage = false;
            int voltageIn = -1, currentIn = -1, voltageOut = -1, temp = -1;
            int cellID =-1;

            m.MPPTUpdated += delegate(object sender, EventArgs e)
            {
                Sunstang.MPPTData.MPPTEventArgs MPPTe = e as Sunstang.MPPTData.MPPTEventArgs;

                overTemp = MPPTe.OverTemperature.Value;
                noCharge = MPPTe.NoCharge.Value;
                underVoltage = MPPTe.UnderVoltage.Value;
                voltageIn = MPPTe.VoltageIn.Value;
                currentIn = MPPTe.CurrentIn.Value;
                voltageOut = MPPTe.VoltageOut.Value;
                temp = MPPTe.Temperature.Value;

                cellID = MPPTe.MPPT;
            };

            bool result = m.parse(frame);

            Assert.AreEqual(true, result, "MPPT didn't parse frame");

            Assert.AreEqual(true, overTemp, "Wrong value added to Over Temperature");
            Assert.AreEqual(true, noCharge, "Wrong value added to Nocharge");
            Assert.AreEqual(true, underVoltage, "Wrong value added to undervoltage");

            Assert.AreEqual(1, voltageIn, "Incorrected Voltage in");
            Assert.AreEqual(2, currentIn, "incorrect Current in");
            Assert.AreEqual(3, voltageOut, "Incorrect Voltage out");
            Assert.AreEqual(4, temp, "Incorrect temperature");

            Assert.AreEqual(1, cellID, "Incorrect cell number set.");
        }

    }
}
