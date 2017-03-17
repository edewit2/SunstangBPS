using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataView
{
    class BPSPacket : Packet
    {
        UInt16 voltage;
        UInt16 temprature;
        UInt16 current;
        UInt16 averageCurrent;
        UInt16 full;
        UInt16 activeEmpty;
        UInt16 standbyEmpty;
        UInt32 flags;

        public BPSPacket() { }
        public BPSPacket(byte[] d) : base(d){}

        public override uint getLength()
        {
            return 26; //bytes
        }

        public override uint getPacketId()
        {
            return 0;
        }

        public override uint getNodeId()
        {
            return 0xAB;
        }

        public override void parseData(byte[] rawData)
        {
            throw new NotImplementedException();
        }
    }
}
