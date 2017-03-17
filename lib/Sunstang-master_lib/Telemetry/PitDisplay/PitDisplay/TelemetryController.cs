using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using XBee;

namespace Sunstang
{
    class TelemetryController
    {
        private XBeeController xb = new XBeeController();
        public BPSdata BPS = new BPSdata();
        public MPPTData MPPT = new MPPTData();
        public KellyData Kelly = new KellyData();

        public TelemetryController()
        {
        }

        public void addXbee(String port, int baud)
        {
            if (!xb.IsOpen)
            {
                xb.OpenAsync(port, baud);
                xb.DataReceived += xb_DataReceived;
            }
        }

        public void addFile(String filepath)
        {
            string line;
            System.IO.StreamReader file = new System.IO.StreamReader(filepath);
            Console.WriteLine("Opening File: " + filepath);
            while ((line = file.ReadLine()) != null)
            {
                canFrame cFrame = new canFrame(line);
                addData(cFrame);
            }
        }


        private void xb_DataReceived(object sender, SourcedDataReceivedEventArgs e)
        {
            canFrame cFrame = new canFrame(Encoding.ASCII.GetString(e.Data, 0, e.Data.Length));
            addData(cFrame);
            Console.WriteLine(cFrame.ID + ": " + BitConverter.ToString(cFrame.Data));
        }

        private void addData(canFrame inFrame)
        {
            Console.WriteLine(inFrame.ID);
            if (inFrame.ID == 0x771 || inFrame.ID == 0x772)
            {
                MPPT.parse(inFrame);
            }
            else if (inFrame.ID > 100 && inFrame.ID < 136)
            {
                BPS.parse(inFrame);
            }
            else if (inFrame.ID == KellyData.RequestId || inFrame.ID == KellyData.ResponseId)
            {
                Kelly.Parse(inFrame);
            }
        }
        public bool close()
        {
            if (xb.IsOpen)
            {
                xb.Close();
            }
            return true;
        }
    }
}
