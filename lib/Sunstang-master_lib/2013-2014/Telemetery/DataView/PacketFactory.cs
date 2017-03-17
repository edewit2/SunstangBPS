using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataView
{
    class PacketFactory
    {
        public static Packet generatePacket(int node, int pack)
        {
            Packet outPack = null;

            /****************************
             * Reserved For TestPackets *
             ****************************/

            if (node == 0 && pack == 0)
            {
                outPack = new TestPacket();
            }
            else
                throw new NotImplementedException("Unkown type: " + node + "," + pack);
            return outPack;
        }
    }
}
