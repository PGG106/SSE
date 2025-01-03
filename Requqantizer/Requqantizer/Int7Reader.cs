using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Requqantizer
{
    internal class Int7Reader
    {
        public void Read(byte[] data)
        {
            var index = 0;
            for (var i = 0; i < 4; i++)
            {
                var firstByte = data[index++];
                var val = firstByte & 0x3F;
                var negative = (firstByte & 0x40) != 0;
                var hasSecondByte = (firstByte & 0x80) != 0;
                if (hasSecondByte)
                {
                    var secondByte = data[index++];
                    val |= secondByte << 6;
                }

                if (negative)
                {
                    val = -val;
                }

                var a = 123;
            }
        }
    }
}
