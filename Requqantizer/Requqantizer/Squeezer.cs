using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Requqantizer
{
    internal class Squeezer
    {
        public sbyte Squeeze(int val)
        {
            const int threshold = 48;
            var abs = Math.Abs(val);
            if (abs < threshold)
            {
                return (sbyte)val;
            }

            var over = abs - threshold;
            var inc = over / 6;
            var result = threshold + inc;
            if (val < 0)
            {
                result = -result;
            }

            return (sbyte)result;
        }
    }
}
