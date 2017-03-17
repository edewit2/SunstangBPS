using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sunstang
{
    public class Measurement<T>
    {
        public T Value { get; set; }
        public DateTime Time { get; set; }

        public Measurement(DateTime d, T value)
        {
            Time = d;
            Value = value;
        }
    }
}
