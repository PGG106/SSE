using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Requqantizer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var dir = $"C:/shared/sse/nets/{Constants.L1_SIZE}";
            var path = Path.Combine(dir, "quantised.bin");
            var path2 = Path.Combine(dir, "raw.bin");

            var sections = new Reader().ReadShorts(path);
            var sections2 = new Reader().ReadFloats(path2);

            var section = sections[0];
            var section2 = sections2[0];
            
            var blockCount = section.Count / Constants.blockSize;
            Console.WriteLine($"Block size: {Constants.blockSize}");
            Console.WriteLine($"Block count: {blockCount}");
            var queue = new Queue<short>(section);
            var queue2 = new Queue<float>(section2);
            var divisors = new List<int>();
            var blocks = new List<List<short>>();
            var blocks2 = new List<List<float>>();
            for (var blockIndex = 0; blockIndex < blockCount; blockIndex++)
            {
                var block = new List<short>();
                var block2 = new List<float>();
                for (var i = 0; i < Constants.blockSize; i++)
                {
                    var value = queue.Dequeue();
                    block.Add(value);

                    var value2 = queue2.Dequeue() * Constants.FT_QUANT;
                    block2.Add(value2);
                }
                blocks.Add(block);
                blocks2.Add(block2);

                int divisor;

                for (divisor = 1; ; divisor++)
                {
                    var min = Math.Round(block2.Min() / (double)divisor);
                    var max = Math.Round(block2.Max() / (double)divisor);

                    if (min == 0 && max == 0)
                    {
                        divisor = 0;
                        break;
                    }

                    if (max <= sbyte.MaxValue && min >= sbyte.MinValue)
                    {
                        break;
                    }
                }
                divisors.Add(divisor);

                Console.WriteLine($"Block {blockIndex}: Divisor: {divisor} Min: {block.Min()} Max: {block.Max()}");
            }

            if (queue.Count > 0)
            {
                throw new Exception("Didnt read all from queue");
            }

            var result = new List<byte>();
            for (var blockIndex = 0; blockIndex < blocks.Count; blockIndex++)
            {
                var block = blocks2[blockIndex];
                var divisor = divisors[blockIndex];

                result.Add((byte)divisor);
                if (divisor == 0)
                {
                    continue;
                }

                foreach (var value in block)
                {
                    var quantized = (byte)(sbyte)Math.Round(value / (double)divisor);
                    result.Add(quantized);
                }
            }

            foreach (var value in sections[1])
            {
                //result.Add((byte)(sbyte)value);

                //var sht = (short)Math.Round(value * FT_QUANT);
                //result.AddRange(BitConverter.GetBytes(sht));

                result.AddRange(BitConverter.GetBytes(value));
            }
            foreach (var value in sections[2])
            {
                //var sht = (short)Math.Round(value * L1_QUANT);
                //result.AddRange(BitConverter.GetBytes(sht));
                result.Add((byte)(sbyte)value);
            }
            foreach (var value in sections[3])
            {
                //var sht = (short)Math.Round(value * FT_QUANT * L1_QUANT);
                //result.AddRange(BitConverter.GetBytes(sht));

                result.AddRange(BitConverter.GetBytes(value));
            }

            File.WriteAllBytes("nn.net", result.ToArray());
        }
    }
}
