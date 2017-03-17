using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace Sunstang
{

    public partial class ChartWithData : UserControl
    {
        private int _numDataPoints;
        private double[] _presentData;  //This data is put into the table once every timer click
        private Timer redrawTimer = new Timer();
        private delegate void addDataCallback(int id, DateTime x, double y);
        private String[] _names;

        public int NumDataPoints
        {
            set
            {
                _numDataPoints = value;
                _presentData = new double[_numDataPoints];
                _names = new string[_numDataPoints];
                table.Rows.Clear();
                addRowsToTable();
                initializeChart();
            }
            get { return _numDataPoints; }
        }

        public int redrawInterval
        {
            set { redrawTimer.Interval = value; }
            get { return redrawTimer.Interval; }
        }

        public ChartWithData()
        {
            InitializeComponent();
            redrawTimer.Interval = 1000;
            redrawTimer.Tick += redrawTimer_Tick;

            table.CellValueChanged += showSeries;
        }

        private void addRowsToTable()
        {
            for (int i = 0; i < _numDataPoints; i++)
            {
                DataGridViewRow row = (DataGridViewRow)table.RowTemplate.Clone();
                row.CreateCells(table, i, i, "~", true);  
                table.Rows.Add(row);
            }
        }

        private void initializeChart()
        {
            chart.Series.Clear();
            for (int i = 0; i < _numDataPoints; i++)
            {
                Series series = chart.Series.Add(i.ToString());
                series.ChartType = SeriesChartType.FastLine;
                chart.ChartAreas[series.ChartArea].CursorX.AutoScroll = true;
                chart.ChartAreas[series.ChartArea].AxisX.LabelStyle.Format = "hh:mm:s";
                series.XValueType = ChartValueType.DateTime;
            }
        }


        public void addData(int id, DateTime x, double y)
        {
            if (chart.InvokeRequired)
            {
                addDataCallback d = new addDataCallback(addData);
                this.Invoke(d, id, x, y);
            }
            else
            {
                redrawTimer.Enabled = true;

                //TODO: ensure X is newer than what we have
                _presentData[id] = y;
                chart.Series[id].Points.AddXY(x, y);
            }
        }

        public void addData(int id, Measurement<float> data)
        {
            addData(id, data.Time, data.Value);
        }

        public void addData(int id, Measurement<sbyte> data)
        {
            addData(id, data.Time, data.Value);
        }
        public void addData(int id, Measurement<byte> data)
        {
            addData(id, data.Time, data.Value);
        }

        public void addData(int id, Measurement<ushort> data)
        {
            addData(id, data.Time, data.Value);
        }

        private void redrawTimer_Tick(object sender, EventArgs e)
        {
            chart.Series.ResumeUpdates();
            chart.Series.Invalidate();
            chart.Series.SuspendUpdates();

            foreach (DataGridViewRow row in table.Rows)
            {
                System.Int32 cellId = (System.Int32)row.Cells["id"].Value;
                row.Cells["data"].Value = _presentData[cellId];
            }

        }

        private void showSeries(Object sender, EventArgs e)
        {
            if (chart.InvokeRequired)
            {
                EventHandler d = new EventHandler(showSeries);
                this.Invoke(d, sender, e);
            }
            else
            {
                DataGridViewCellEventArgs args = (DataGridViewCellEventArgs)e;
                int moduleID = (int)table.Rows[args.RowIndex].Cells["ID"].Value;
                bool enabled = Convert.ToBoolean(table.Rows[args.RowIndex].Cells["isDisplayed"].Value);
                chart.Series[moduleID].Enabled = enabled;
            }
        }
        
        public void setName(int id, String name)
        {
            _names[id] = name;
            chart.Series[id].LegendText = name;
            foreach (DataGridViewRow row in table.Rows)
            {
                if ((System.Int32) row.Cells["id"].Value == id)
                {
                    row.Cells["DataName"].Value = name;
                }
            }

        }
    }
}
