using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataView
{
    class TestPacket : Packet
    {

        UInt16 bigData;
        Byte age;
        Byte otherData;

        public TestPacket(byte[] d) : base(d){}

        public TestPacket() { }

        public override uint getLength()
        {
            return 4;
        }

        public override uint getPacketId()
        {
            return 0x0;
        }

        public override uint getNodeId()
        {
            return 0x00;
        }

        public override void parseData(byte[] rawData)
        {
            if (rawData.Length != getLength())
                throw new IndexOutOfRangeException("Data recieved is not the correct length");

            bigData = (UInt16)(rawData[0] << 8 | rawData[1]);
            age = rawData[2];
            otherData = rawData[3];
        }
    }
}
