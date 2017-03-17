namespace BatteryTest
{
    partial class SerialChooser
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SerialChooser));
            this.label1 = new System.Windows.Forms.Label();
            this.SerialPortCombo = new System.Windows.Forms.ComboBox();
            this.OkayButton = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.BaudRateCombo = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Serial Port: ";
            // 
            // SerialPortCombo
            // 
            this.SerialPortCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.SerialPortCombo.FormattingEnabled = true;
            this.SerialPortCombo.Location = new System.Drawing.Point(76, 10);
            this.SerialPortCombo.Name = "SerialPortCombo";
            this.SerialPortCombo.Size = new System.Drawing.Size(121, 21);
            this.SerialPortCombo.TabIndex = 1;
            // 
            // OkayButton
            // 
            this.OkayButton.Location = new System.Drawing.Point(122, 69);
            this.OkayButton.Name = "OkayButton";
            this.OkayButton.Size = new System.Drawing.Size(75, 23);
            this.OkayButton.TabIndex = 3;
            this.OkayButton.Text = "&Okay!!!";
            this.OkayButton.UseVisualStyleBackColor = true;
            this.OkayButton.Click += new System.EventHandler(this.OkayButton_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Baud Rate";
            // 
            // BaudRateCombo
            // 
            this.BaudRateCombo.FormattingEnabled = true;
            this.BaudRateCombo.Items.AddRange(new object[] {
            "300",
            "600",
            "1200",
            "2400",
            "4800",
            "9600",
            "14400",
            "19200",
            "28800",
            "38400",
            "57600",
            "115200"});
            this.BaudRateCombo.Location = new System.Drawing.Point(76, 37);
            this.BaudRateCombo.Name = "BaudRateCombo";
            this.BaudRateCombo.Size = new System.Drawing.Size(121, 21);
            this.BaudRateCombo.TabIndex = 2;
            // 
            // SerialChooser
            // 
            this.AcceptButton = this.OkayButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(215, 104);
            this.Controls.Add(this.BaudRateCombo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.OkayButton);
            this.Controls.Add(this.SerialPortCombo);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SerialChooser";
            this.Text = "Choose a Serial Port";
            this.Load += new System.EventHandler(this.SerialChooser_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox SerialPortCombo;
        private System.Windows.Forms.Button OkayButton;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox BaudRateCombo;
    }
}