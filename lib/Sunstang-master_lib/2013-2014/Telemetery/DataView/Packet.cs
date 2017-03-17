using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataView
{
    public abstract class Packet
    {
        public abstract uint getLength();
        public abstract uint getPacketId();
        public abstract uint getNodeId();

        public abstract void parseData(byte[] rawData);

        public Packet() { }

        public Packet(byte[] data)
        {
            parseData(data);
        }

        public String ToString()
        {
            StringBuilder s = new StringBuilder();
            s.Append("Subsys: ");
            s.AppendLine(""+getNodeId());
            s.Append("Packet: ");
            s.AppendLine(""+getPacketId());
            s.Append("Length: ");
            s.AppendLine(""+getLength());
            return s.ToString();
        }

    }
}
