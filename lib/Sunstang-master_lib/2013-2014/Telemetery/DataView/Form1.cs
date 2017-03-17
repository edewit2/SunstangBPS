using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace DataView
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            xbeeHandler x = new xbeeHandler("COM13");
            x.PacketArrived += c_PacketRecieved;
            chart1.Series[0].ChartType = SeriesChartType.Line;
        }

        void c_PacketRecieved(object sender, PacketRecievedEventArgs e)
        {
            Packet pack = e.data;

            //IF we're debugging throw the data to the console
            #if DEBUG 
            Console.WriteLine(pack.ToString());
            #endif

            if (this.chart1.InvokeRequired)
            {
                this.chart1.Invoke((MethodInvoker)delegate
                {
                    chart1.Series[0].Points.Add(new DataPoint(e.time.Hour, pack.getLength()));
                });
            }
            else
            {
                chart1.Series[0].Points.Add(new DataPoint(e.time.Hour, 9));
            }

        }

    }
}
