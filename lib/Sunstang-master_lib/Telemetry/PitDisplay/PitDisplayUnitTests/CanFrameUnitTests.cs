using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Sunstang;

namespace Sunstang
{
    [TestClass]
    public class CanFrameUnitTests
    {
        [TestMethod]
        public void checkDefaultConstructor()
        {
            int id = 100;
            byte[] data = { 0, 1, 2, 3, 4, 5, 6, 7 };
            DateTime time = new DateTime(1993, 12, 9);

            canFrame frame = new canFrame(id, data, time);

            Assert.AreEqual(id, frame.ID, "Incorrect ID set.");
            Assert.AreEqual(data, frame.Data, "Incorrect Data set.");
            Assert.AreEqual(time, frame.Time, "Incorrect Time set.");

        }

        [TestMethod]
        public void checkCSVConstructor()
        {
            int id = 0x100;
            int length = 8;
            byte[] data = { 0, 1, 2, 3, 4, 5, 6, 7 };
            DateTime time = new DateTime(1993, 12, 9);

            canFrame frame = new canFrame("755413200,100,0001020304050607");
            Assert.AreEqual(id, frame.ID, "Incorrect ID set.");
            Assert.AreEqual(length, frame.Length, "Incorrect Length set.");

            for (int i = 0; i < data.Length; i++)
            {
                Assert.AreEqual(data[i], frame.Data[i], "Incorrect Data set: "+ i);
            }

            Assert.AreEqual(time, frame.Time, "Incorrect Time set.");

        }

    }
}
