using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Sunstang;

namespace Sunstang
{
    [TestClass]
    public class TemperatureMeasurementUnitTests
    {
        [TestMethod]
        public void rawToLogicalOver550()
        {
            float expected = 36; //degrees
            UInt16 raw = 558;
            float actual = TemperatureMeasurement.rawToLogical(raw);

            Assert.AreEqual(expected, actual, 0.051395817,  "Incorrect Temperature Conversion");
        }

        [TestMethod]
        public void rawToLogicalLessThan175()
        {
            float expected = 58; //degrees
            UInt16 raw = 186;
            float actual = TemperatureMeasurement.rawToLogical(raw);

            Assert.AreEqual(expected, actual, 0.080820328, "Incorrect Temperature Conversion");
        }

        [TestMethod]
        public void rawToLogicalMiddle()
        {
            float expected = 40; //degrees
            UInt16 raw = 483;
            float actual = TemperatureMeasurement.rawToLogical(raw);

            Assert.AreEqual(expected, actual, 0.059458674, "Incorrect Temperature Conversion");
        }

        [TestMethod]
        public void ConstrutorSet()
        {
            DateTime time = new DateTime(1993, 12, 9);
            Byte high = 0x02;
            Byte low = 0x2E;

            float expectedTemp = 36; //degrees

            TemperatureMeasurement temp = new TemperatureMeasurement(time, high, low);

            Assert.AreEqual(time, temp.Time, "Time incorrectly set");
            Assert.AreEqual(expectedTemp, temp.Temperature, 0.06, "Temperature incorrectly set.");

        }

    }
}
