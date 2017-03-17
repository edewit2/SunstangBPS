using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BatteryTest
{
    public partial class SerialChooser : Form
    {

        public SerialPort response;

        public SerialChooser()
        {
            InitializeComponent();
        }

        private void SerialChooser_Load(object sender, EventArgs e)
        {
            this.SerialPortCombo.Items.AddRange(SerialPort.GetPortNames().OrderBy(n => n).ToArray());
        }

        private void OkayButton_Click(object sender, EventArgs e)
        {
            response = new SerialPort(SerialPortCombo.Text, Convert.ToInt32(BaudRateCombo.Text));
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        }
    }
}
