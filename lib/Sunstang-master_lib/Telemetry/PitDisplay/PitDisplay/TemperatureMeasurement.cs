using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sunstang
{
    public class TemperatureMeasurement
    {
        public float Temperature { get; set; }
        public DateTime Time { get; set; }

        public TemperatureMeasurement(DateTime d, byte high, byte low)
        {
            Time = d;
            UInt16 raw = (UInt16)((UInt16)(high) << 8 | low);
            Temperature = rawToLogical(raw);
        }

        /*This function is from John*/
        public static float rawToLogical(UInt16 adcVal)
        {
            if (adcVal > 550)
            {
                return adcVal * (-0.051395817f) + 64.6869847f;
            }
            if (adcVal <= 197)
            {
                return adcVal * (-0.080820328f) + 73.01123921f;
            }
            else
            {
                return adcVal * (-0.059458674f) + 68.71374138f;
            }

        }

    }
}
