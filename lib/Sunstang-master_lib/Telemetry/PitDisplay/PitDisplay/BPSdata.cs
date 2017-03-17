using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sunstang
{
    public class BPSdata
    {

        public const int NUM_VOLT = 32;
        public const int NUM_TEMP = 96;

        LinkedList<Measurement<float>>[] voltage;  //array index is cell number, linked list holds data

        public LinkedList<Measurement<float>>[] Voltage
        {
            get { return voltage; }
        }

        LinkedList<Measurement<float>> current;

        public LinkedList<Measurement<float>> Current
        {
            get { return current; }
        }
        LinkedList<TemperatureMeasurement>[] temp;

        public LinkedList<TemperatureMeasurement>[] Temperature
        {
            get { return temp; }
        }

        LinkedList<VoltageStatus> voltageStatus;

        public LinkedList<VoltageStatus> VoltageStatus
        {
            get { return voltageStatus; }
        }

        LinkedList<byte> temperatureStatus;
        public LinkedList<byte> TemperatureStatus
        {
            get { return temperatureStatus; }
        }

        LinkedList<byte> currentStatus;
        public LinkedList<byte> CurrentStatus
        {
            get { return currentStatus; }
        }

        public event EventHandler voltageAdded;
        public event EventHandler CurrentAdded;
        public event EventHandler TemperatureAdded;
        public event EventHandler voltageStatusAdded;
        public event EventHandler temperatureStatusAdded;
        public event EventHandler currentStatusAdded;

        public BPSdata()
        {
            voltage = new LinkedList<Measurement<float>>[NUM_VOLT];

            for (int i = 0; i < voltage.Length; i++)
            {
                voltage[i] = new LinkedList<Measurement<float>>();
            }

            current = new LinkedList<Measurement<float>>();

            temp = new LinkedList<TemperatureMeasurement>[NUM_TEMP];

            for (int i = 0; i < temp.Length; i++)
            {
                temp[i] = new LinkedList<TemperatureMeasurement>();
            }

            voltageStatus = new LinkedList<VoltageStatus>();
            temperatureStatus = new LinkedList<byte>();
            currentStatus = new LinkedList<byte>();
        }

        public bool parse(canFrame frame)
        {
            if (frame.ID < 100 || frame.ID > 138)
            {
                return false;
            }

            if (frame.ID >= 100 && frame.ID <= 107)
            { //voltage
                int loc = frame.ID - 100;
                for (int i = 0; i < 4; i++)
                {
                    int cellId = loc*4 + i;



                    Measurement<float> m = new Measurement<float>(frame.Time, voltageConveraion(frame.Data[i * 2], frame.Data[i * 2 + 1]));
                    voltage[cellId].AddFirst(m);
                    if (voltageAdded != null)
                    {
                        VoltageEventArgs e = new VoltageEventArgs();
                        e.Voltage = m;
                        e.Cell = cellId;
                        voltageAdded(this, e);
                    }
                }
            }

            if (frame.ID == 108)
            {
                Measurement<float> mesurement = new Measurement<float>(frame.Time, currentConversion(frame.Data[0], frame.Data[1]));
                current.AddFirst(mesurement);
               
                if (CurrentAdded != null)
                {
                    CurrentEventArgs e = new CurrentEventArgs();
                    e.Current = mesurement;

                    CurrentAdded(this, e);

                }
            }

            if(frame.ID >= 109 && frame.ID <=132)
            {
                int loc = frame.ID - 109;
                for (int i = 0; i < 4; i++)
                {
                    int cellId = loc * 4 + i;
                    TemperatureMeasurement measurement = new TemperatureMeasurement(frame.Time, frame.Data[i * 2], frame.Data[i * 2 + 1]);
                    temp[loc * 4 + i].AddFirst(measurement);

                    if (TemperatureAdded != null)
                    {
                        TemperatureEventArgs e = new TemperatureEventArgs();
                        e.Cell = cellId;
                        e.Temperature = measurement;

                        TemperatureAdded(this, e);
                    }
                }
            }

            if (frame.ID == 133)
            {
                voltageStatus.AddFirst(new VoltageStatus(frame.Data[0]));

                if (voltageStatusAdded != null)
                {
                    EventArgs e = new EventArgs();
                    voltageStatusAdded(this,  e);
                }
            }

            if (frame.ID == 135)
            {
                temperatureStatus.AddFirst(frame.Data[0]);

                if (temperatureStatusAdded != null)
                {
                    EventArgs e = new EventArgs();
                    temperatureStatusAdded(this, e);
                }
            }

            if (frame.ID == 137)
            {
                currentStatus.AddFirst(frame.Data[0]);

                if (currentStatusAdded != null)
                {
                    EventArgs e = new EventArgs();
                    currentStatusAdded(this, e);
                }
            }

            return true;
        }

        public static float voltageConveraion(byte high, byte low)
        {
            UInt16 raw = (UInt16)((UInt16)(high) << 8 | low);
            return raw * 0.0001f;
        }

        public static float currentConversion(byte high, byte low)
        {
            UInt16 raw = (UInt16)((UInt16)(high) << 8 | low);
            return raw * 180f / 1023 - 150;
        }

    }

    public class VoltageEventArgs : EventArgs
    {
        public Measurement<float> Voltage { get; set; }
        public int Cell {get; set;}
    }

    public class CurrentEventArgs : EventArgs
    {
        public Measurement<float> Current { get; set; }
    }

    public class TemperatureEventArgs : EventArgs
    {
        public TemperatureMeasurement Temperature { get; set; }
        public int Cell { get; set; }
    }

}
