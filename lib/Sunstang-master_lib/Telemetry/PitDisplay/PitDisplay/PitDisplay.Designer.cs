namespace Sunstang
{
    partial class PitDisplay
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PitDisplay));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.xbeeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.dataSetToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.BPSVoltageTab = new System.Windows.Forms.TabControl();
            this.BPSTabPages = new System.Windows.Forms.TabPage();
            this.voltageChartWithData1 = new Sunstang.ChartWithData();
            this.tabPage7 = new System.Windows.Forms.TabPage();
            this.currentDisplay = new Sunstang.ChartWithData();
            this.tabPage8 = new System.Windows.Forms.TabPage();
            this.temperatureChart = new Sunstang.ChartWithData();
            this.tabPage9 = new System.Windows.Forms.TabPage();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.VoltageStatusTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.temperatureStatusTextBox = new System.Windows.Forms.TextBox();
            this.currentStatusTextBox = new System.Windows.Forms.TextBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.UNDV1 = new System.Windows.Forms.CheckBox();
            this.UNDV0 = new System.Windows.Forms.CheckBox();
            this.NOC1 = new System.Windows.Forms.CheckBox();
            this.NOC0 = new System.Windows.Forms.CheckBox();
            this.OVT1 = new System.Windows.Forms.CheckBox();
            this.OVT0 = new System.Windows.Forms.CheckBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.BVLR0 = new System.Windows.Forms.CheckBox();
            this.BVLR1 = new System.Windows.Forms.CheckBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.MPPTVoltage = new Sunstang.ChartWithData();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.MPPTCurrent = new Sunstang.ChartWithData();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.MPPTTempChart = new Sunstang.ChartWithData();
            this.mainTabs = new System.Windows.Forms.TabControl();
            this.motorControllerTab = new System.Windows.Forms.TabPage();
            this.tabControl2 = new System.Windows.Forms.TabControl();
            this.tabPage10 = new System.Windows.Forms.TabPage();
            this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
            this.KellyThrottleChart = new Sunstang.ChartWithData();
            this.KellyBrakeChart = new Sunstang.ChartWithData();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.tabPage11 = new System.Windows.Forms.TabPage();
            this.KellyCurrent = new Sunstang.ChartWithData();
            this.tabPage12 = new System.Windows.Forms.TabPage();
            this.KellyHighVoltage = new Sunstang.ChartWithData();
            this.tabPage13 = new System.Windows.Forms.TabPage();
            this.KellySpeed = new Sunstang.ChartWithData();
            this.tabPage14 = new System.Windows.Forms.TabPage();
            this.KellyTemperatures = new Sunstang.ChartWithData();
            this.tabPage15 = new System.Windows.Forms.TabPage();
            this.menuStrip1.SuspendLayout();
            this.tabPage6.SuspendLayout();
            this.BPSVoltageTab.SuspendLayout();
            this.BPSTabPages.SuspendLayout();
            this.tabPage7.SuspendLayout();
            this.tabPage8.SuspendLayout();
            this.tabPage9.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.mainTabs.SuspendLayout();
            this.motorControllerTab.SuspendLayout();
            this.tabControl2.SuspendLayout();
            this.tabPage10.SuspendLayout();
            this.tableLayoutPanel4.SuspendLayout();
            this.tabPage11.SuspendLayout();
            this.tabPage12.SuspendLayout();
            this.tabPage13.SuspendLayout();
            this.tabPage14.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(553, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.xbeeToolStripMenuItem,
            this.dataSetToolStripMenuItem});
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(64, 20);
            this.connectToolStripMenuItem.Text = "&Connect";
            // 
            // xbeeToolStripMenuItem
            // 
            this.xbeeToolStripMenuItem.Name = "xbeeToolStripMenuItem";
            this.xbeeToolStripMenuItem.Size = new System.Drawing.Size(116, 22);
            this.xbeeToolStripMenuItem.Text = "&Xbee";
            this.xbeeToolStripMenuItem.Click += new System.EventHandler(this.xbeeToolStripMenuItem_Click);
            // 
            // dataSetToolStripMenuItem
            // 
            this.dataSetToolStripMenuItem.Name = "dataSetToolStripMenuItem";
            this.dataSetToolStripMenuItem.Size = new System.Drawing.Size(116, 22);
            this.dataSetToolStripMenuItem.Text = "Data set";
            this.dataSetToolStripMenuItem.Click += new System.EventHandler(this.dataSetToolStripMenuItem_Click);
            // 
            // tabPage6
            // 
            this.tabPage6.Controls.Add(this.BPSVoltageTab);
            this.tabPage6.Location = new System.Drawing.Point(4, 22);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage6.Size = new System.Drawing.Size(545, 312);
            this.tabPage6.TabIndex = 5;
            this.tabPage6.Text = "BPS";
            this.tabPage6.UseVisualStyleBackColor = true;
            // 
            // BPSVoltageTab
            // 
            this.BPSVoltageTab.Controls.Add(this.BPSTabPages);
            this.BPSVoltageTab.Controls.Add(this.tabPage7);
            this.BPSVoltageTab.Controls.Add(this.tabPage8);
            this.BPSVoltageTab.Controls.Add(this.tabPage9);
            this.BPSVoltageTab.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BPSVoltageTab.Location = new System.Drawing.Point(3, 3);
            this.BPSVoltageTab.Name = "BPSVoltageTab";
            this.BPSVoltageTab.SelectedIndex = 0;
            this.BPSVoltageTab.Size = new System.Drawing.Size(539, 306);
            this.BPSVoltageTab.TabIndex = 0;
            // 
            // BPSTabPages
            // 
            this.BPSTabPages.Controls.Add(this.voltageChartWithData1);
            this.BPSTabPages.Location = new System.Drawing.Point(4, 22);
            this.BPSTabPages.Name = "BPSTabPages";
            this.BPSTabPages.Padding = new System.Windows.Forms.Padding(3);
            this.BPSTabPages.Size = new System.Drawing.Size(531, 280);
            this.BPSTabPages.TabIndex = 0;
            this.BPSTabPages.Text = "Voltage";
            this.BPSTabPages.UseVisualStyleBackColor = true;
            // 
            // voltageChartWithData1
            // 
            this.voltageChartWithData1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.voltageChartWithData1.Location = new System.Drawing.Point(3, 3);
            this.voltageChartWithData1.Name = "voltageChartWithData1";
            this.voltageChartWithData1.NumDataPoints = 0;
            this.voltageChartWithData1.redrawInterval = 1000;
            this.voltageChartWithData1.Size = new System.Drawing.Size(525, 274);
            this.voltageChartWithData1.TabIndex = 1;
            // 
            // tabPage7
            // 
            this.tabPage7.Controls.Add(this.currentDisplay);
            this.tabPage7.Location = new System.Drawing.Point(4, 22);
            this.tabPage7.Name = "tabPage7";
            this.tabPage7.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage7.Size = new System.Drawing.Size(531, 280);
            this.tabPage7.TabIndex = 2;
            this.tabPage7.Text = "Current";
            this.tabPage7.UseVisualStyleBackColor = true;
            // 
            // currentDisplay
            // 
            this.currentDisplay.Dock = System.Windows.Forms.DockStyle.Fill;
            this.currentDisplay.Location = new System.Drawing.Point(3, 3);
            this.currentDisplay.Name = "currentDisplay";
            this.currentDisplay.NumDataPoints = 0;
            this.currentDisplay.redrawInterval = 1000;
            this.currentDisplay.Size = new System.Drawing.Size(525, 274);
            this.currentDisplay.TabIndex = 1;
            // 
            // tabPage8
            // 
            this.tabPage8.Controls.Add(this.temperatureChart);
            this.tabPage8.Location = new System.Drawing.Point(4, 22);
            this.tabPage8.Name = "tabPage8";
            this.tabPage8.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage8.Size = new System.Drawing.Size(531, 280);
            this.tabPage8.TabIndex = 1;
            this.tabPage8.Text = "Temperature";
            this.tabPage8.UseVisualStyleBackColor = true;
            // 
            // temperatureChart
            // 
            this.temperatureChart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.temperatureChart.Location = new System.Drawing.Point(3, 3);
            this.temperatureChart.Name = "temperatureChart";
            this.temperatureChart.NumDataPoints = 0;
            this.temperatureChart.redrawInterval = 1000;
            this.temperatureChart.Size = new System.Drawing.Size(525, 274);
            this.temperatureChart.TabIndex = 1;
            // 
            // tabPage9
            // 
            this.tabPage9.Controls.Add(this.tableLayoutPanel1);
            this.tabPage9.Location = new System.Drawing.Point(4, 22);
            this.tabPage9.Name = "tabPage9";
            this.tabPage9.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage9.Size = new System.Drawing.Size(531, 280);
            this.tabPage9.TabIndex = 3;
            this.tabPage9.Text = "Status";
            this.tabPage9.UseVisualStyleBackColor = true;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33334F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33334F));
            this.tableLayoutPanel1.Controls.Add(this.VoltageStatusTextBox, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.label2, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.label3, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.temperatureStatusTextBox, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.currentStatusTextBox, 2, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 3);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.Size = new System.Drawing.Size(525, 274);
            this.tableLayoutPanel1.TabIndex = 1;
            // 
            // VoltageStatusTextBox
            // 
            this.VoltageStatusTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.VoltageStatusTextBox.Location = new System.Drawing.Point(3, 23);
            this.VoltageStatusTextBox.Multiline = true;
            this.VoltageStatusTextBox.Name = "VoltageStatusTextBox";
            this.VoltageStatusTextBox.ReadOnly = true;
            this.VoltageStatusTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.VoltageStatusTextBox.Size = new System.Drawing.Size(168, 280);
            this.VoltageStatusTextBox.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(168, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "Voltage Status";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Location = new System.Drawing.Point(177, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(169, 20);
            this.label2.TabIndex = 2;
            this.label2.Text = "Temperature Status";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Location = new System.Drawing.Point(352, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(170, 20);
            this.label3.TabIndex = 3;
            this.label3.Text = "Current Status";
            // 
            // temperatureStatusTextBox
            // 
            this.temperatureStatusTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.temperatureStatusTextBox.Location = new System.Drawing.Point(177, 23);
            this.temperatureStatusTextBox.Multiline = true;
            this.temperatureStatusTextBox.Name = "temperatureStatusTextBox";
            this.temperatureStatusTextBox.ReadOnly = true;
            this.temperatureStatusTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.temperatureStatusTextBox.Size = new System.Drawing.Size(169, 280);
            this.temperatureStatusTextBox.TabIndex = 4;
            // 
            // currentStatusTextBox
            // 
            this.currentStatusTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.currentStatusTextBox.Location = new System.Drawing.Point(352, 23);
            this.currentStatusTextBox.Multiline = true;
            this.currentStatusTextBox.Name = "currentStatusTextBox";
            this.currentStatusTextBox.ReadOnly = true;
            this.currentStatusTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.currentStatusTextBox.Size = new System.Drawing.Size(170, 280);
            this.currentStatusTextBox.TabIndex = 5;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.tabControl1);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(545, 312);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "MPPT";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(3, 3);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(539, 306);
            this.tabControl1.TabIndex = 2;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.tableLayoutPanel2);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(531, 280);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Status";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.tableLayoutPanel2.ColumnCount = 3;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel2.Controls.Add(this.UNDV1, 2, 4);
            this.tableLayoutPanel2.Controls.Add(this.UNDV0, 1, 4);
            this.tableLayoutPanel2.Controls.Add(this.NOC1, 2, 3);
            this.tableLayoutPanel2.Controls.Add(this.NOC0, 1, 3);
            this.tableLayoutPanel2.Controls.Add(this.OVT1, 2, 2);
            this.tableLayoutPanel2.Controls.Add(this.OVT0, 1, 2);
            this.tableLayoutPanel2.Controls.Add(this.label4, 0, 1);
            this.tableLayoutPanel2.Controls.Add(this.label5, 0, 2);
            this.tableLayoutPanel2.Controls.Add(this.label6, 0, 3);
            this.tableLayoutPanel2.Controls.Add(this.label7, 0, 4);
            this.tableLayoutPanel2.Controls.Add(this.BVLR0, 1, 1);
            this.tableLayoutPanel2.Controls.Add(this.BVLR1, 2, 1);
            this.tableLayoutPanel2.Controls.Add(this.label8, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.label9, 1, 0);
            this.tableLayoutPanel2.Controls.Add(this.label10, 2, 0);
            this.tableLayoutPanel2.Location = new System.Drawing.Point(6, 6);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 5;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel2.Size = new System.Drawing.Size(159, 107);
            this.tableLayoutPanel2.TabIndex = 2;
            // 
            // UNDV1
            // 
            this.UNDV1.AutoSize = true;
            this.UNDV1.Enabled = false;
            this.UNDV1.Location = new System.Drawing.Point(140, 87);
            this.UNDV1.Name = "UNDV1";
            this.UNDV1.Size = new System.Drawing.Size(15, 14);
            this.UNDV1.TabIndex = 14;
            this.UNDV1.UseVisualStyleBackColor = true;
            // 
            // UNDV0
            // 
            this.UNDV0.AutoSize = true;
            this.UNDV0.Enabled = false;
            this.UNDV0.Location = new System.Drawing.Point(118, 87);
            this.UNDV0.Name = "UNDV0";
            this.UNDV0.Size = new System.Drawing.Size(15, 14);
            this.UNDV0.TabIndex = 13;
            this.UNDV0.UseVisualStyleBackColor = true;
            // 
            // NOC1
            // 
            this.NOC1.AutoSize = true;
            this.NOC1.Enabled = false;
            this.NOC1.Location = new System.Drawing.Point(140, 66);
            this.NOC1.Name = "NOC1";
            this.NOC1.Size = new System.Drawing.Size(15, 14);
            this.NOC1.TabIndex = 12;
            this.NOC1.UseVisualStyleBackColor = true;
            // 
            // NOC0
            // 
            this.NOC0.AutoSize = true;
            this.NOC0.Enabled = false;
            this.NOC0.Location = new System.Drawing.Point(118, 66);
            this.NOC0.Name = "NOC0";
            this.NOC0.Size = new System.Drawing.Size(15, 14);
            this.NOC0.TabIndex = 11;
            this.NOC0.UseVisualStyleBackColor = true;
            // 
            // OVT1
            // 
            this.OVT1.AutoSize = true;
            this.OVT1.Enabled = false;
            this.OVT1.Location = new System.Drawing.Point(140, 45);
            this.OVT1.Name = "OVT1";
            this.OVT1.Size = new System.Drawing.Size(15, 14);
            this.OVT1.TabIndex = 10;
            this.OVT1.UseVisualStyleBackColor = true;
            // 
            // OVT0
            // 
            this.OVT0.AutoSize = true;
            this.OVT0.Enabled = false;
            this.OVT0.Location = new System.Drawing.Point(118, 45);
            this.OVT0.Name = "OVT0";
            this.OVT0.Size = new System.Drawing.Size(15, 14);
            this.OVT0.TabIndex = 9;
            this.OVT0.UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(4, 15);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(83, 26);
            this.label4.TabIndex = 0;
            this.label4.Text = "Battery Voltage Level Reached:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(4, 42);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(93, 13);
            this.label5.TabIndex = 1;
            this.label5.Text = "Over Temperature";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(4, 63);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(58, 13);
            this.label6.TabIndex = 2;
            this.label6.Text = "No Charge";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(4, 84);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(75, 13);
            this.label7.TabIndex = 3;
            this.label7.Text = "Under Voltage";
            // 
            // BVLR0
            // 
            this.BVLR0.AutoSize = true;
            this.BVLR0.Enabled = false;
            this.BVLR0.Location = new System.Drawing.Point(118, 18);
            this.BVLR0.Name = "BVLR0";
            this.BVLR0.Size = new System.Drawing.Size(15, 14);
            this.BVLR0.TabIndex = 4;
            this.BVLR0.UseVisualStyleBackColor = true;
            // 
            // BVLR1
            // 
            this.BVLR1.AutoSize = true;
            this.BVLR1.Enabled = false;
            this.BVLR1.Location = new System.Drawing.Point(140, 18);
            this.BVLR1.Name = "BVLR1";
            this.BVLR1.Size = new System.Drawing.Size(15, 14);
            this.BVLR1.TabIndex = 5;
            this.BVLR1.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(4, 1);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(40, 13);
            this.label8.TabIndex = 15;
            this.label8.Text = "MPPT:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(118, 1);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(13, 13);
            this.label9.TabIndex = 16;
            this.label9.Text = "0";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(140, 1);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(13, 13);
            this.label10.TabIndex = 17;
            this.label10.Text = "1";
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.MPPTVoltage);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(531, 280);
            this.tabPage3.TabIndex = 1;
            this.tabPage3.Text = "Voltage";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // MPPTVoltage
            // 
            this.MPPTVoltage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MPPTVoltage.Location = new System.Drawing.Point(3, 3);
            this.MPPTVoltage.Name = "MPPTVoltage";
            this.MPPTVoltage.NumDataPoints = 0;
            this.MPPTVoltage.redrawInterval = 1000;
            this.MPPTVoltage.Size = new System.Drawing.Size(525, 274);
            this.MPPTVoltage.TabIndex = 3;
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.MPPTCurrent);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(531, 280);
            this.tabPage4.TabIndex = 2;
            this.tabPage4.Text = "Current";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // MPPTCurrent
            // 
            this.MPPTCurrent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MPPTCurrent.Location = new System.Drawing.Point(3, 3);
            this.MPPTCurrent.Name = "MPPTCurrent";
            this.MPPTCurrent.NumDataPoints = 0;
            this.MPPTCurrent.redrawInterval = 1000;
            this.MPPTCurrent.Size = new System.Drawing.Size(525, 274);
            this.MPPTCurrent.TabIndex = 1;
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.MPPTTempChart);
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage5.Size = new System.Drawing.Size(531, 280);
            this.tabPage5.TabIndex = 3;
            this.tabPage5.Text = "Temperature";
            this.tabPage5.UseVisualStyleBackColor = true;
            // 
            // MPPTTempChart
            // 
            this.MPPTTempChart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MPPTTempChart.Location = new System.Drawing.Point(3, 3);
            this.MPPTTempChart.Name = "MPPTTempChart";
            this.MPPTTempChart.NumDataPoints = 0;
            this.MPPTTempChart.redrawInterval = 1000;
            this.MPPTTempChart.Size = new System.Drawing.Size(525, 274);
            this.MPPTTempChart.TabIndex = 0;
            // 
            // mainTabs
            // 
            this.mainTabs.Controls.Add(this.tabPage2);
            this.mainTabs.Controls.Add(this.tabPage6);
            this.mainTabs.Controls.Add(this.motorControllerTab);
            this.mainTabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainTabs.Location = new System.Drawing.Point(0, 24);
            this.mainTabs.Name = "mainTabs";
            this.mainTabs.SelectedIndex = 0;
            this.mainTabs.ShowToolTips = true;
            this.mainTabs.Size = new System.Drawing.Size(553, 338);
            this.mainTabs.TabIndex = 1;
            // 
            // motorControllerTab
            // 
            this.motorControllerTab.Controls.Add(this.tabControl2);
            this.motorControllerTab.Location = new System.Drawing.Point(4, 22);
            this.motorControllerTab.Name = "motorControllerTab";
            this.motorControllerTab.Padding = new System.Windows.Forms.Padding(3);
            this.motorControllerTab.Size = new System.Drawing.Size(545, 312);
            this.motorControllerTab.TabIndex = 6;
            this.motorControllerTab.Text = "Kelly";
            this.motorControllerTab.ToolTipText = "I <3 Kelly";
            this.motorControllerTab.UseVisualStyleBackColor = true;
            // 
            // tabControl2
            // 
            this.tabControl2.Controls.Add(this.tabPage10);
            this.tabControl2.Controls.Add(this.tabPage11);
            this.tabControl2.Controls.Add(this.tabPage12);
            this.tabControl2.Controls.Add(this.tabPage13);
            this.tabControl2.Controls.Add(this.tabPage14);
            this.tabControl2.Controls.Add(this.tabPage15);
            this.tabControl2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl2.Location = new System.Drawing.Point(3, 3);
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedIndex = 0;
            this.tabControl2.Size = new System.Drawing.Size(539, 306);
            this.tabControl2.TabIndex = 0;
            // 
            // tabPage10
            // 
            this.tabPage10.Controls.Add(this.tableLayoutPanel4);
            this.tabPage10.Location = new System.Drawing.Point(4, 22);
            this.tabPage10.Name = "tabPage10";
            this.tabPage10.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage10.Size = new System.Drawing.Size(531, 280);
            this.tabPage10.TabIndex = 0;
            this.tabPage10.Text = "Pedals";
            this.tabPage10.UseVisualStyleBackColor = true;
            // 
            // tableLayoutPanel4
            // 
            this.tableLayoutPanel4.ColumnCount = 1;
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel4.Controls.Add(this.KellyThrottleChart, 0, 1);
            this.tableLayoutPanel4.Controls.Add(this.KellyBrakeChart, 0, 3);
            this.tableLayoutPanel4.Controls.Add(this.label12, 0, 0);
            this.tableLayoutPanel4.Controls.Add(this.label13, 0, 2);
            this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel4.Location = new System.Drawing.Point(3, 3);
            this.tableLayoutPanel4.Name = "tableLayoutPanel4";
            this.tableLayoutPanel4.RowCount = 4;
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel4.Size = new System.Drawing.Size(525, 274);
            this.tableLayoutPanel4.TabIndex = 0;
            // 
            // KellyThrottleChart
            // 
            this.KellyThrottleChart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KellyThrottleChart.Location = new System.Drawing.Point(3, 23);
            this.KellyThrottleChart.Name = "KellyThrottleChart";
            this.KellyThrottleChart.NumDataPoints = 4;
            this.KellyThrottleChart.redrawInterval = 1000;
            this.KellyThrottleChart.Size = new System.Drawing.Size(519, 111);
            this.KellyThrottleChart.TabIndex = 0;
            // 
            // KellyBrakeChart
            // 
            this.KellyBrakeChart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KellyBrakeChart.Location = new System.Drawing.Point(3, 160);
            this.KellyBrakeChart.Name = "KellyBrakeChart";
            this.KellyBrakeChart.NumDataPoints = 3;
            this.KellyBrakeChart.redrawInterval = 1000;
            this.KellyBrakeChart.Size = new System.Drawing.Size(519, 111);
            this.KellyBrakeChart.TabIndex = 2;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(3, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(43, 13);
            this.label12.TabIndex = 3;
            this.label12.Text = "Throttle";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(3, 137);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(35, 13);
            this.label13.TabIndex = 4;
            this.label13.Text = "Brake";
            // 
            // tabPage11
            // 
            this.tabPage11.Controls.Add(this.KellyCurrent);
            this.tabPage11.Location = new System.Drawing.Point(4, 22);
            this.tabPage11.Name = "tabPage11";
            this.tabPage11.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage11.Size = new System.Drawing.Size(531, 280);
            this.tabPage11.TabIndex = 1;
            this.tabPage11.Text = "Current";
            this.tabPage11.UseVisualStyleBackColor = true;
            // 
            // KellyCurrent
            // 
            this.KellyCurrent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KellyCurrent.Location = new System.Drawing.Point(3, 3);
            this.KellyCurrent.Name = "KellyCurrent";
            this.KellyCurrent.NumDataPoints = 4;
            this.KellyCurrent.redrawInterval = 1000;
            this.KellyCurrent.Size = new System.Drawing.Size(525, 274);
            this.KellyCurrent.TabIndex = 0;
            // 
            // tabPage12
            // 
            this.tabPage12.Controls.Add(this.KellyHighVoltage);
            this.tabPage12.Location = new System.Drawing.Point(4, 22);
            this.tabPage12.Name = "tabPage12";
            this.tabPage12.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage12.Size = new System.Drawing.Size(531, 280);
            this.tabPage12.TabIndex = 2;
            this.tabPage12.Text = "High Voltage";
            this.tabPage12.UseVisualStyleBackColor = true;
            // 
            // KellyHighVoltage
            // 
            this.KellyHighVoltage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KellyHighVoltage.Location = new System.Drawing.Point(3, 3);
            this.KellyHighVoltage.Name = "KellyHighVoltage";
            this.KellyHighVoltage.NumDataPoints = 5;
            this.KellyHighVoltage.redrawInterval = 1000;
            this.KellyHighVoltage.Size = new System.Drawing.Size(525, 274);
            this.KellyHighVoltage.TabIndex = 0;
            // 
            // tabPage13
            // 
            this.tabPage13.Controls.Add(this.KellySpeed);
            this.tabPage13.Location = new System.Drawing.Point(4, 22);
            this.tabPage13.Name = "tabPage13";
            this.tabPage13.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage13.Size = new System.Drawing.Size(531, 280);
            this.tabPage13.TabIndex = 3;
            this.tabPage13.Text = "Speed";
            this.tabPage13.UseVisualStyleBackColor = true;
            // 
            // KellySpeed
            // 
            this.KellySpeed.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KellySpeed.Location = new System.Drawing.Point(3, 3);
            this.KellySpeed.Name = "KellySpeed";
            this.KellySpeed.NumDataPoints = 2;
            this.KellySpeed.redrawInterval = 1000;
            this.KellySpeed.Size = new System.Drawing.Size(525, 274);
            this.KellySpeed.TabIndex = 0;
            // 
            // tabPage14
            // 
            this.tabPage14.Controls.Add(this.KellyTemperatures);
            this.tabPage14.Location = new System.Drawing.Point(4, 22);
            this.tabPage14.Name = "tabPage14";
            this.tabPage14.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage14.Size = new System.Drawing.Size(531, 280);
            this.tabPage14.TabIndex = 4;
            this.tabPage14.Text = "Temperatures";
            this.tabPage14.UseVisualStyleBackColor = true;
            // 
            // KellyTemperatures
            // 
            this.KellyTemperatures.Dock = System.Windows.Forms.DockStyle.Fill;
            this.KellyTemperatures.Location = new System.Drawing.Point(3, 3);
            this.KellyTemperatures.Name = "KellyTemperatures";
            this.KellyTemperatures.NumDataPoints = 4;
            this.KellyTemperatures.redrawInterval = 1000;
            this.KellyTemperatures.Size = new System.Drawing.Size(525, 274);
            this.KellyTemperatures.TabIndex = 0;
            // 
            // tabPage15
            // 
            this.tabPage15.Location = new System.Drawing.Point(4, 22);
            this.tabPage15.Name = "tabPage15";
            this.tabPage15.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage15.Size = new System.Drawing.Size(531, 280);
            this.tabPage15.TabIndex = 5;
            this.tabPage15.Text = "Errors";
            this.tabPage15.UseVisualStyleBackColor = true;
            // 
            // PitDisplay
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(553, 362);
            this.Controls.Add(this.mainTabs);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "PitDisplay";
            this.Text = "Sunstang Telemetry";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.tabPage6.ResumeLayout(false);
            this.BPSVoltageTab.ResumeLayout(false);
            this.BPSTabPages.ResumeLayout(false);
            this.tabPage7.ResumeLayout(false);
            this.tabPage8.ResumeLayout(false);
            this.tabPage9.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.tableLayoutPanel2.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.mainTabs.ResumeLayout(false);
            this.motorControllerTab.ResumeLayout(false);
            this.tabControl2.ResumeLayout(false);
            this.tabPage10.ResumeLayout(false);
            this.tableLayoutPanel4.ResumeLayout(false);
            this.tableLayoutPanel4.PerformLayout();
            this.tabPage11.ResumeLayout(false);
            this.tabPage12.ResumeLayout(false);
            this.tabPage13.ResumeLayout(false);
            this.tabPage14.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem xbeeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem dataSetToolStripMenuItem;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.TabControl BPSVoltageTab;
        private System.Windows.Forms.TabPage BPSTabPages;
        private ChartWithData voltageChartWithData1;
        private System.Windows.Forms.TabPage tabPage8;
        private ChartWithData temperatureChart;
        private System.Windows.Forms.TabPage tabPage7;
        private ChartWithData currentDisplay;
        private System.Windows.Forms.TabPage tabPage9;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TextBox VoltageStatusTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox temperatureStatusTextBox;
        private System.Windows.Forms.TextBox currentStatusTextBox;
        private System.Windows.Forms.TabPage tabPage2;
        private ChartWithData MPPTCurrent;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.CheckBox UNDV1;
        private System.Windows.Forms.CheckBox UNDV0;
        private System.Windows.Forms.CheckBox NOC1;
        private System.Windows.Forms.CheckBox NOC0;
        private System.Windows.Forms.CheckBox OVT1;
        private System.Windows.Forms.CheckBox OVT0;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.CheckBox BVLR0;
        private System.Windows.Forms.CheckBox BVLR1;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TabControl mainTabs;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TabPage tabPage5;
        private ChartWithData MPPTTempChart;
        private System.Windows.Forms.TabPage motorControllerTab;
        private System.Windows.Forms.TabControl tabControl2;
        private System.Windows.Forms.TabPage tabPage10;
        private System.Windows.Forms.TabPage tabPage11;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
        private ChartWithData KellyThrottleChart;
        private ChartWithData KellyBrakeChart;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private ChartWithData KellyCurrent;
        private System.Windows.Forms.TabPage tabPage12;
        private ChartWithData KellyHighVoltage;
        private System.Windows.Forms.TabPage tabPage13;
        private ChartWithData KellySpeed;
        private System.Windows.Forms.TabPage tabPage14;
        private ChartWithData KellyTemperatures;
        private System.Windows.Forms.TabPage tabPage15;
        private ChartWithData MPPTVoltage;
    }
}

