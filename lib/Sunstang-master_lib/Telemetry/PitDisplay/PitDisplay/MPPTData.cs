using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sunstang
{
    public class MPPTData
    {
        public const int numMPPT =2;
        LinkedList<Measurement<bool>>[] batteryVoltageLevelReached;
        LinkedList<Measurement<bool>>[] overTemperature;
        LinkedList<Measurement<bool>>[] noCharge;
        LinkedList<Measurement<bool>>[] underVoltage;

        LinkedList<Measurement<int>>[] voltageIn;  //TODO: figure out the actual conversion "Number of amperes or volts per LSB are client specfic"
        LinkedList<Measurement<int>>[] currentIn;
        LinkedList<Measurement<int>>[] voltageOut;
        LinkedList<Measurement<int>>[] temperature;

        public event EventHandler MPPTUpdated;

        public LinkedList<Measurement<bool>>[] BatteryVoltageLevelReaced
        {
            get { return batteryVoltageLevelReached; }
        }

        public LinkedList<Measurement<bool>>[] OverTemperature
        {
            get { return overTemperature;}
        }

        public LinkedList<Measurement<bool>>[] NoCharge
        {
            get { return noCharge; }
        }

        public LinkedList<Measurement<bool>>[] UnderVoltage
        {
            get { return underVoltage; }
        }

        public LinkedList<Measurement<int>>[] VoltageIn
        {
            get { return voltageIn; }
        }

        public LinkedList<Measurement<int>>[] CurrentIn
        {
            get { return currentIn; }
        }

        public LinkedList<Measurement<int>>[] VoltageOut
        {
            get { return voltageOut; }
        }

        public LinkedList<Measurement<int>>[] Temperature
        {
            get { return temperature; }
        }

        public MPPTData()
        {
            batteryVoltageLevelReached = new LinkedList<Measurement<bool>>[numMPPT];
            overTemperature = new LinkedList<Measurement<bool>>[numMPPT];
            noCharge = new LinkedList<Measurement<bool>>[numMPPT];
            underVoltage = new LinkedList<Measurement<bool>>[numMPPT];
            voltageIn = new LinkedList<Measurement<int>>[numMPPT];
            voltageOut = new LinkedList<Measurement<int>>[numMPPT];
            temperature = new LinkedList<Measurement<int>>[numMPPT];
            currentIn = new LinkedList<Measurement<int>>[numMPPT];

            for (int i = 0; i < numMPPT; i++)
            {
                batteryVoltageLevelReached[i] = new LinkedList<Measurement<bool>>();
                overTemperature[i] = new LinkedList<Measurement<bool>>();
                noCharge[i] = new LinkedList<Measurement<bool>>();
                underVoltage[i] = new LinkedList<Measurement<bool>>();
                voltageIn[i] = new LinkedList<Measurement<int>>();
                voltageOut[i] = new LinkedList<Measurement<int>>();
                temperature[i] = new LinkedList<Measurement<int>>();
                currentIn[i] = new LinkedList<Measurement<int>>();
            }
        }
        public bool parse(canFrame data)
        {
            if (data.ID != 1905 && data.ID != 1906 || data.Length != 7) //make sure the frame is actually what we expect
            {
                return false;
            }

            int mpptID;

            if(data.ID == 1905)
            {
                mpptID = 0;
            }
            else
            {
                mpptID = 1;
            }

            Measurement<bool> bvlr = new Measurement<bool>(data.Time, (data.Data[0] & 0x80) != 0);
            Measurement<bool> overtemp = new Measurement<bool>(data.Time, (data.Data[0] & 0x40) != 0);
            Measurement<bool> underVolt = new Measurement<bool>(data.Time, (data.Data[0] & 0x10) != 0);
            Measurement<bool> nocharge = new Measurement<bool>(data.Time, (data.Data[0] & 0x20) != 0);

            Measurement<int> vIn =  new Measurement<int>(data.Time, (int)data.Data[0] & 0x3 << 10 | data.Data[1]);
            Measurement<int> iIn =  new Measurement<int>(data.Time, (int)data.Data[2] & 0x3 << 10 | data.Data[3]);
            Measurement<int> vOut =  new Measurement<int>(data.Time, (int)data.Data[4] & 0x3 << 10 | data.Data[5]);
            Measurement<int> temp =  new Measurement<int>(data.Time, data.Data[6]);

            batteryVoltageLevelReached[mpptID].AddFirst(bvlr);
            overTemperature[mpptID].AddFirst(overtemp);
            noCharge[mpptID].AddFirst(nocharge);
            underVoltage[mpptID].AddFirst(underVolt);

            voltageIn[mpptID].AddFirst(vIn);
            currentIn[mpptID].AddFirst(iIn);
            voltageOut[mpptID].AddFirst(vOut);
            temperature[mpptID].AddFirst(temp);

            if (MPPTUpdated != null)
            {
                MPPTEventArgs e = new MPPTEventArgs();
                e.BatteryLevelReached = bvlr;
                e.OverTemperature = overtemp;
                e.UnderVoltage = underVolt;
                e.NoCharge = nocharge;
                e.VoltageIn = vIn;
                e.CurrentIn = iIn;
                e.VoltageOut = vOut;
                e.Temperature = temp;
                e.MPPT = mpptID;
                MPPTUpdated(this, e);
            }

            return true;
        }
        public class MPPTEventArgs : EventArgs
        {
            public Measurement<bool> BatteryLevelReached {get; set;}
            public Measurement<bool> OverTemperature {get; set;}
            public Measurement<bool>  UnderVoltage {get; set;}
            public Measurement<bool>  NoCharge  {get; set;}

            public Measurement<int> VoltageIn {get; set;}
            public Measurement<int> CurrentIn {get; set;}
            public Measurement<int> VoltageOut {get; set;}
            public Measurement<int> Temperature { get; set; }
            public int MPPT { get; set; }
        }
    }
}
