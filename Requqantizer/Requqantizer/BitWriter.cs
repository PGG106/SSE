using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Requqantizer
{
    internal class BitWriter : IDisposable
    {
        private readonly BinaryWriter _writer;
        private byte _currentByte;
        private int _currentBits;

        public BitWriter(MemoryStream stream)
        {
            _writer = new BinaryWriter(stream);
            _currentByte = 0;
            _currentBits = 0;
        }
        public BitWriter(BinaryWriter writer)
        {
            _writer = writer;
        }

        public void WriteBit(bool value)
        {
            WriteBit(value ? 1 : 0);
        }

        public void WriteBit(int value)
        {
            if (value != 0 && value != 1)
            {
                throw new ArgumentException("Must be 1 or 0", nameof(value));
            }

            _currentByte <<= 1;
            _currentByte |= (byte)(value);
            _currentBits++;
            if (_currentBits == 8)
            {
                FlushCurrentByte();
            }
        }

        public void WriteIntShittyAttempt(int val)
        {
            if (val < 0)
            {
                WriteBit(1);
            }
            var abs = Math.Abs(val);
            var copy = abs;
            var needsBits = 0;
            while (copy != 0)
            {
                copy /= 2;
                needsBits++;
            }

            for (int i = 0; i < 4; i++)
            {
                var bit = val & 1;
                val >>= 1;
                WriteBit(bit);
            }

            while (abs != 0)
            {
                var bit = val & 1;
                val >>= 1;
                WriteBit(bit);
            }
        }

        public void FlushCurrentByte()
        {
            _writer.Write(_currentByte);
            _currentByte = 0;
            _currentBits = 0;
        }

        public void Dispose()
        {
            _writer.Dispose();
        }
    }
}
