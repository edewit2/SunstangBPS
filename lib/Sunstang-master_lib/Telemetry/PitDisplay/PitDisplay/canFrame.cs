using System;

namespace Sunstang
{

    public class canFrame
    {
        private int id;
        public int ID
        {
            get { return id; }
            set { id = value; }
        }

        byte[] data;
        public byte[] Data
        {
            get { return data; }
            set { data = value; }
        }

        public int Length
        {
            get { return data.Length; }
        }

        DateTime time;
        public DateTime Time
        {
            get { return time; }
            set { time = value; }
        }

        public canFrame(int id, byte[] data, DateTime dt)
        {
            this.id = id;
            this.data = data;
            this.time = dt;
        }

        public canFrame(String input)
        {
                String[] parsedInput = input.Split(',');
                time = new DateTime(1970, 1, 1, 0, 0, 0, 0, System.DateTimeKind.Utc);
                time = time.AddSeconds(Convert.ToDouble(parsedInput[0])).ToLocalTime();

                id = Convert.ToInt16(parsedInput[1], 16);
                int length = parsedInput[2].Length / 2;
                data = new byte[length];

                for (int i = 0; i < length; i++)
                {
                    data[i] = (byte)((GetHexVal(parsedInput[2][i * 2]) << 4) + (GetHexVal(parsedInput[2][i * 2 + 1])));
                }
        }
        private static int GetHexVal(char hex)
        {
            int val = (int)hex;
            //For uppercase A-F letters:
            return val - (val < 58 ? 48 : 55);
        }
    }
}
