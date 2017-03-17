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
using XBee;

namespace Sunstang
{
    public partial class PitDisplay : Form
    {

        private TelemetryController tc = new TelemetryController();

        public PitDisplay()
        {
            InitializeComponent();

            currentDisplay.NumDataPoints = 1;
            tc.BPS.CurrentAdded += newCurrentHandler;

            voltageChartWithData1.NumDataPoints = BPSdata.NUM_VOLT;
            tc.BPS.voltageAdded += newVoltageHandler;

            temperatureChart.NumDataPoints = BPSdata.NUM_TEMP;
            tc.BPS.TemperatureAdded += newTemperatureHandler;

            tc.BPS.voltageStatusAdded += voltageStatusHandler;
            tc.BPS.temperatureStatusAdded += temperatureStatusHandler;
            tc.BPS.currentStatusAdded += currentStatusHandler;

            MPPTCurrent.NumDataPoints = MPPTData.numMPPT;
            MPPTVoltage.NumDataPoints = MPPTData.numMPPT*2;
            MPPTVoltage.setName(0, "Vin 0");
            MPPTVoltage.setName(1, "Vout 0");
            MPPTVoltage.setName(2, "Vin 1");
            MPPTVoltage.setName(3, "Vout 1");
            MPPTTempChart.NumDataPoints = MPPTData.numMPPT;

            tc.MPPT.MPPTUpdated += newMPPTDataHandler;

            //Insert Handlers for Kelly
            tc.Kelly.AddedA2D1Response += AddedA2D1ResponseDataHandler;
            tc.Kelly.AddedA2D2Response += AddedA2D2ResponseDataHandler;
            tc.Kelly.AddedMonitor1Response += AddedMonitor1ResponseDataHandler;
            tc.Kelly.AddedMonitor2Response += AddedMonitor2ResponseDataHandler;
        }

        private void xbeeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BatteryTest.SerialChooser serialDialog = new BatteryTest.SerialChooser();
            DialogResult dialogResult = serialDialog.ShowDialog();
            if (dialogResult == DialogResult.OK)
            {
                tc.addXbee(serialDialog.response.PortName, serialDialog.response.BaudRate);
                Console.WriteLine(serialDialog.response.BaudRate + " " + serialDialog.response.PortName);
            }
        }

        private void dataSetToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog fd = new OpenFileDialog();
            DialogResult result = fd.ShowDialog();

            if (result == DialogResult.OK)
            {
                tc.addFile(fd.FileName);
            }
        }

        private void newCurrentHandler(object sender, EventArgs e)
        {
            CurrentEventArgs args = (CurrentEventArgs) e;
            currentDisplay.addData(0, args.Current.Time, args.Current.Value);
        }

        private void newVoltageHandler(object sender, EventArgs e)
        {
            VoltageEventArgs args = (VoltageEventArgs) e;
            voltageChartWithData1.addData(args.Cell, args.Voltage.Time, args.Voltage.Value);
        }

        private void newTemperatureHandler(object sender, EventArgs e)
        {
            TemperatureEventArgs args = (TemperatureEventArgs) e;
            temperatureChart.addData(args.Cell, args.Temperature.Time, args.Temperature.Temperature);
        }

        private void voltageStatusHandler(object sender, EventArgs e)
        {
            if (VoltageStatusTextBox.InvokeRequired)
            {
                EventHandler d = new EventHandler(voltageStatusHandler);
                this.Invoke(d, sender, e);
            }
            else
            {
                VoltageStatusTextBox.AppendText(tc.BPS.VoltageStatus.First().Meaning + "\n");
            }
        }

        private void temperatureStatusHandler(object sender, EventArgs e)
        {
            if (temperatureStatusTextBox.InvokeRequired)
            {
                EventHandler d = new EventHandler(temperatureStatusHandler);
                this.Invoke(d, sender, e);
            }
            else
            {
                temperatureStatusTextBox.AppendText(tc.BPS.TemperatureStatus.First().ToString() + "\n");
            }
        }

        private void currentStatusHandler(object sender, EventArgs e)
        {
            if (currentStatusTextBox.InvokeRequired)
            {
                EventHandler d = new EventHandler(currentStatusHandler);
                this.Invoke(d, sender, e);
            }
            else
            {
                currentStatusTextBox.AppendText(tc.BPS.CurrentStatus.First().ToString() + "\n");
            }
        }

        private void newMPPTDataHandler(object sender, EventArgs e)
        {
            Sunstang.MPPTData.MPPTEventArgs args = (Sunstang.MPPTData.MPPTEventArgs) e;
            MPPTVoltage.addData(args.MPPT*2, args.VoltageIn.Time, args.VoltageIn.Value);
                //Vin Mppt 0 = 0, Vin Mppt 1 = 2
            MPPTVoltage.addData(args.MPPT*2 + 1, args.VoltageOut.Time, args.VoltageOut.Value);
                //Vout Mppt 0 = 1, Vout Mppt 1 = 3

            MPPTCurrent.addData(args.MPPT, args.CurrentIn.Time, args.CurrentIn.Value);

            MPPTTempChart.addData(args.MPPT, args.Temperature.Time, args.Temperature.Value);

            if (BVLR0.InvokeRequired)
            {
                this.Invoke(new EventHandler(newMPPTDataHandler), sender, e);
            }
            else
            {
                if (args.MPPT == 0)
                {
                    BVLR0.Checked = args.BatteryLevelReached.Value;
                    OVT0.Checked = args.OverTemperature.Value;
                    NOC0.Checked = args.NoCharge.Value;
                    UNDV0.Checked = args.UnderVoltage.Value;
                }
                else if (args.MPPT == 1)
                {
                    BVLR1.Checked = args.BatteryLevelReached.Value;
                    OVT1.Checked = args.OverTemperature.Value;
                    NOC1.Checked = args.NoCharge.Value;
                    UNDV1.Checked = args.UnderVoltage.Value;
                }
                else
                {
                    Console.Error.WriteLine("Unknown MPPT.");
                }
            }
        }

        private void AddedA2D1ResponseDataHandler(object sender, KellyA2D1EventArgs e)
        {
            KellyThrottleChart.addData(0, e.ThrottlePosition); //Pedal charts, first data point is pot pos
            KellyThrottleChart.addData(2, e.SupplyVoltage); //Third will be the reference
            
            KellyBrakeChart.addData(0, e.BrakeRegenPosition);
            KellyBrakeChart.addData(2, e.SupplyVoltage);

            KellyHighVoltage.addData(0, e.BPlusVoltage);
            KellyHighVoltage.addData(1, e.OperationVoltage);
        }

        private void AddedA2D2ResponseDataHandler(object sender, KellyA2D2EventArgs e)
        {
            KellyCurrent.addData(1, e.CurrentPhaseA);
            KellyCurrent.addData(2, e.CurrentPhaseB);
            KellyCurrent.addData(3, e.CurrentPhaseC);

            KellyHighVoltage.addData(2, e.VoltagePhaseA);
            KellyHighVoltage.addData(3, e.VoltagePhaseB);
            KellyHighVoltage.addData(4, e.VoltagePhaseC);
        }

        private void AddedMonitor1ResponseDataHandler(object sender, KellyMonitor1EventArgs e)
        {
            //TODO: Dosomething with enabled
            KellySpeed.addData(1, e.PWM);
            KellyTemperatures.addData(0, e.MotorTemperature);
            KellyTemperatures.addData(1, e.ControllerTemp);
            KellyTemperatures.addData(2, e.MosfetTempHighSide);
            KellyTemperatures.addData(3, e.MosfetTempLowSide);
        }

        private void AddedMonitor2ResponseDataHandler(object sender, KellyMonitor2EventArgs e)
        {
            KellySpeed.addData(0, e.RPM);
            KellyCurrent.addData(0, e.CurrentPercent);
            //TODO: deal with error codes
        }

        private void AddedThrottleSw(object sender, KellySwitchEventArgs e )
        {
            KellyThrottleChart.addData(1, e.Value.Time, Convert.ToByte(e.Value.Value));
        }

        private void AddedBrakeSw(object sender, KellySwitchEventArgs e)
        {
            KellyBrakeChart.addData(1, e.Value.Time, Convert.ToByte(e.Value.Value));
        }

        private void AddedReverseSw(object sender, KellySwitchEventArgs e)
        {
            KellyThrottleChart.addData(3, e.Value.Time, Convert.ToByte(e.Value.Value));
        }
    }
}
