#define DEBUG

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;



namespace DataView
{

    class xbeeHandler
    {

        public event EventHandler<PacketRecievedEventArgs> PacketArrived;

        public const byte START_FLAG = 0xA2;
        public const byte STOP_FLAG = 0x69;
        private SerialPort s;
        static int c = 0;

        /**
         * Only parameter is the port name EG COM14
         */
        public xbeeHandler(String port)
        {
            s = new SerialPort(port);

            //Default arduino setting http://arduino.cc/en/Serial/begin
            //Except for Baud rate,  this can be lowered but you'll need to chage it on the tele arduino, and both xbees
            s.Parity = Parity.None;
            s.StopBits = StopBits.One;
            s.DataBits = 8;
            s.BaudRate = 9600;

            s.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            s.Open();

        }

        private void DataReceivedHandler(
                        object sender,
                        SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            int subsysID, packetID, readLength;

            int tmp = sp.ReadByte();
            if (tmp != START_FLAG)
            {
                Console.WriteLine("I dun Goofed" + tmp);
                return;
            }

            //Read the SubSystemID; 
            while (sp.BytesToRead == 0) { }
            subsysID=sp.ReadByte();

            //Read the PacketID;
            while (sp.BytesToRead == 0) { }
            packetID = sp.ReadByte();
            
            //Read The length;
            while (sp.BytesToRead == 0) { }
            readLength = sp.ReadByte();
           
            //Create a Packet object for the data go in
            Packet currentPack = PacketFactory.generatePacket(subsysID, packetID);

            //Make sure the length we recieve is the actual length
            //as a cheap error check

            if (readLength != currentPack.getLength())
            {
                Console.WriteLine("Bad Length!");
                return;
            }
            //Allocate somewhere to read the data into
            byte[] buff = new byte[readLength];
            while (sp.BytesToRead < readLength) { }
            sp.Read(buff, 0, readLength);
            
            while (sp.BytesToRead == 0) { }
            tmp = 0;
            tmp = sp.ReadByte();
            if (tmp != STOP_FLAG)
            {
                Console.WriteLine("End Byte Expected");
                return;
            }

            //Throw the data in to the packet
            currentPack.parseData(buff);

            PacketRecievedEventArgs args = new PacketRecievedEventArgs();
            args.data = currentPack;
            args.time = new DateTime();

            OnPacketRecieved(args);
        }

        protected virtual void OnPacketRecieved(PacketRecievedEventArgs e)
        {
            EventHandler<PacketRecievedEventArgs> handler = PacketArrived;
            if (handler != null)
            {
                handler(this, e);
            }
        }
    }

    public class PacketRecievedEventArgs : EventArgs
    {
        public Packet data { get; set; }
        public DateTime time { get; set; }
    }
}

