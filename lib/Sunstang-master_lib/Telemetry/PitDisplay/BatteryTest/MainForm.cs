using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace BatteryTest
{
    public partial class MainForm : Form
    {
        SerialPort input;
        FileStream csvOut;

        public MainForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SerialChooser dialog = new SerialChooser();
            DialogResult dr = dialog.ShowDialog();

            
            //Check to make sure the user clicked a COM port
            if(dr == DialogResult.OK)
            {
                Console.WriteLine( dialog.response.BaudRate + " " + dialog.response.PortName );
                
                //If we Already done a test clear all the things
                if ( input != null )
                {
                    foreach (var series in BatteryChart.Series)
                    {
                        series.Points.Clear();
                    }
                }

                input = dialog.response;
                input.ReadTimeout = 500;

                button1.Enabled = false;
                button2.Enabled = true;

                SaveFileDialog saveDialog = new SaveFileDialog();
                saveDialog.RestoreDirectory = true;
                saveDialog.OverwritePrompt = true;
                saveDialog.Filter = "csv files (*.csv)|*.csv|All files (*.*)|*.*";

                if (saveDialog.ShowDialog() == DialogResult.OK)
                {
                    if ((csvOut = (FileStream)saveDialog.OpenFile()) != null)
                    {
                        Console.Write(saveDialog.FileName);
                    }
                }
                else
                {
                    MessageBox.Show("Careful, This session will not be saved to file!", "File Not Selected!", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }

                //Start Reading the Serial Stream!
                SerialParse.RunWorkerAsync();

            }
        }

        private void SerialParse_DoWork(object sender, DoWorkEventArgs e)
        {
            input.Open();
            int i =0;
            BackgroundWorker worker = sender as BackgroundWorker;
            while (!worker.CancellationPending)
            {
                try
                {
                    string message = input.ReadLine();
                    worker.ReportProgress(++i, message);
                }
                catch (TimeoutException) { }
            }
            input.Close();
        }

        private void SerialParse_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            Console.WriteLine("Data Received:" + e.UserState);
            String inputData = e.UserState as String;

            String[] seperatedData = inputData.Trim().Split();

            if (seperatedData.Length == 2)
            {
                String id = seperatedData[0];
                try
                {
                    double voltage = Convert.ToDouble(seperatedData[1]);
                    DateTime now = DateTime.Now;

                    //Write out to file
                    ///Probably a prettier way of doing this using <see cref="File">
                    String outputLine = id + ", " + voltage + ", " + now + System.Environment.NewLine;
                    if(csvOut != null && csvOut.CanWrite)
                        csvOut.Write(Encoding.Unicode.GetBytes(outputLine), 0, Encoding.Unicode.GetByteCount(outputLine));

                    if (BatteryChart.Series.FindByName(id) == null)
                    {
                            BatteryChart.Series.Add(id);
                            BatteryChart.Series[id].ChartType = SeriesChartType.FastLine;
                            BatteryChart.Series[id].XValueType = ChartValueType.Time;
                    }

                        BatteryChart.Series[id].Points.AddXY(now, voltage);
                }catch(System.FormatException)
                {
                    Console.Error.WriteLine("Failed to read voltage(Not a number?)");
                }
                catch(NullReferenceException)
                {
                    Console.Error.WriteLine("Tried to draw to the screen after you removed it.");

                }
            }
            else
            {
                Console.Error.WriteLine("Input length incorrect (Might happen if we start reading in the wrong spot. No big deal)");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            button2.Enabled = false;
            button1.Enabled = true;
            SerialParse.CancelAsync();  //Stop the serial stream
            
            if(csvOut != null)
                csvOut.Close(); //Close the Outputfile

        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            button2_Click(sender, e);
        }
    } 
}
