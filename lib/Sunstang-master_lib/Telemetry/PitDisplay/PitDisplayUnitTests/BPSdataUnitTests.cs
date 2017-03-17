using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace Sunstang
{
    [TestClass]
    public class BPSdataUnitTests
    {
        [TestMethod]
        public void DefaultConstructorVoltageInit()
        {
            int numVoltageReadings = 32;

            BPSdata data = new BPSdata();


            Assert.AreEqual(numVoltageReadings, data.Voltage.Length, "Incorrect number of voltage readings");

            for (int i = 0; i < numVoltageReadings; i++)
            {
                Assert.AreEqual(0, data.Voltage[i].Count, "Expecting an empty list");
            }
        }

        [TestMethod]
        public void IDOutsideRangeLow()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = {};
            canFrame frame = new canFrame(1,frameData, new DateTime());

            bool accepted =BPS.parse(frame);

            Assert.AreEqual(false, accepted, "BPS accepted frame outside range.");
        }

        [TestMethod]
        public void IDOutsideRangeHigh()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { };
            canFrame frame = new canFrame(139, frameData, new DateTime());

            bool accepted = BPS.parse(frame);

            Assert.AreEqual(false, accepted, "BPS accepted frame outside range.");
        }

        [TestMethod]
        public void VoltageAdded()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(101, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            bool accepted = BPS.parse(frame);

            Assert.AreEqual(true, accepted, "BPS didn't accept a voltage frame.");

            for (int i = 4; i < 8; i++)
            {
                Assert.AreEqual(1, BPS.Voltage[i].Count, "BPS didn't add a11 4 voltages");
            }

        }

        
        [TestMethod]
        public void VoltageEventThrown()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(100, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            LinkedList<int> cellIds = new LinkedList<int>();

            BPS.voltageAdded += delegate(object sender, EventArgs e)
            {
                VoltageEventArgs ev = (VoltageEventArgs)e;
                cellIds.AddLast(ev.Cell);
            };

            bool accepted = BPS.parse(frame);
            Assert.AreEqual(4, cellIds.Count, "Wrong amount of event throwns.");

            for(int i = 0; i < cellIds.Count; i++)
            {
                Assert.AreEqual(true, cellIds.Contains(i), "Missing cell id " + i);
            }


        }

        [TestMethod]
        public void CurrentAdded()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(108, frameData, new DateTime());

            bool accepted = BPS.parse(frame);

            Assert.AreEqual(true, accepted, "BPS didn't accept the Current frame.");

            Assert.AreEqual(1, BPS.Current.Count, "BPS didn't add the current");

        }

        [TestMethod]
        public void CurrentEventThrown()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(108, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            int count = 0;

            BPS.CurrentAdded += delegate(object sender, EventArgs e)
            {
                count++;
            };

            bool accepted = BPS.parse(frame);
            Assert.AreEqual(1, count, "Wrong amount of current events thrown.");
        }

        [TestMethod]
        public void TemperatureAdded()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(109, frameData, new DateTime());

            bool accepted = BPS.parse(frame);

            Assert.AreEqual(true, accepted, "BPS didn't accept the Temperature frame.");

            for (int i = 0; i < 4; i++)
            {
                Assert.AreEqual(1, BPS.Temperature[i].Count, "BPS didn't add the current");
            }
        }
        [TestMethod]
        public void temperatureEventThrown()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canFrame frame = new canFrame(109, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            LinkedList<int> cellIds = new LinkedList<int>();

            BPS.TemperatureAdded += delegate(object sender, EventArgs e)
            {
                TemperatureEventArgs ev = (TemperatureEventArgs)e;
                cellIds.AddLast(ev.Cell);
            };

            bool accepted = BPS.parse(frame);
            Assert.AreEqual(4, cellIds.Count, "Wrong amount of event throwns.");

            for (int i = 0; i < cellIds.Count; i++)
            {
                Assert.AreEqual(true, cellIds.Contains(i), "Missing cell id " + i);
            }
        }
        [TestMethod]
        public void testCurrentConversion()
        {

            // Raw is 1137
            byte high = 0x04;
            byte low = 0x71;

            float expected = 50; //amps

            float current = BPSdata.currentConversion(high, low);

            Assert.AreEqual(expected, current, 0.161);

        }

        [TestMethod]
        public void VoltageStatusEventThrown()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 100 };
            canFrame frame = new canFrame(133, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            int count = 0;

            BPS.voltageStatusAdded += delegate(object sender, EventArgs e)
            {
                count++;
            };

            bool accepted = BPS.parse(frame);
            Assert.AreEqual(frameData[0], BPS.VoltageStatus.First.Value.statusCode, "Incorrect status set");
            Assert.AreEqual(1, count, "Wrong amount of voltage status events thrown.");
        }

        [TestMethod]
        public void TempStatusEventThrown()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 100 };
            canFrame frame = new canFrame(135, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            int count = 0;

            BPS.temperatureStatusAdded += delegate(object sender, EventArgs e)
            {
                count++;
            };

            bool accepted = BPS.parse(frame);
            Assert.AreEqual(frameData[0], BPS.TemperatureStatus.First.Value, "Incorrect status set");
            Assert.AreEqual(1, count, "Wrong amount of temperature status events thrown.");
        }

        [TestMethod]
        public void CurrentStatusEventThrown()
        {
            BPSdata BPS = new BPSdata();
            byte[] frameData = { 100 };
            canFrame frame = new canFrame(137, frameData, new DateTime());  //TODO: Make this a real input that can be parsed properly

            int count = 0;

            BPS.currentStatusAdded += delegate(object sender, EventArgs e)
            {
                count++;
            };

            bool accepted = BPS.parse(frame);

            Assert.AreEqual(frameData[0], BPS.CurrentStatus.First.Value, "Incorrect  status set");
            Assert.AreEqual(1, count, "Wrong amount of current status events thrown.");
        }


    }
}
