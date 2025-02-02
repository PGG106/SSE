using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Requqantizer
{
    internal class Int7Writer : IDisposable
    {
        private readonly BinaryWriter _writer;
        private byte _currentByte;
        private int _currentBits;

        public Int7Writer(MemoryStream stream)
        {
            _writer = new BinaryWriter(stream);
            _currentByte = 0;
            _currentBits = 0;
        }

        public Int7Writer(BinaryWriter writer)
        {
            _writer = writer;
        }

        public void WriteInt(int value)
        {
            var negative = value < 0;
            var negativeInt = negative ? 1 : 0;
            var abs = Math.Abs(value);
            var mask = 0b00111111;
            var masked = abs & mask;
            var remaining = abs >> 6;
            var hasSecondByte = remaining != 0;
            var hasSecondByteInt = hasSecondByte ? 1 : 0;
            byte firstByte = (byte)(masked | (negativeInt << 6) | (hasSecondByteInt << 7));
            _writer.Write(firstByte);
            if (hasSecondByte)
            {
                _writer.Write((byte)remaining);
            }
            var a = 123;
        }

        public void Dispose()
        {
            _writer.Dispose();
        }
    }
}
