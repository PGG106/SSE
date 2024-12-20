namespace Requqantizer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            //var ms = new MemoryStream();
            //var w = new Int7Writer(ms);
            //w.WriteInt(1);
            //w.WriteInt(12);
            //w.WriteInt(123);
            //w.WriteInt(1234);
            //var a = ms.ToArray();
            //var r = new Int7Reader();
            //r.Read(a);
            //return;

            var dir = $"C:/shared/sse/nets/{Constants.L1_SIZE}";
            var path = Path.Combine(dir, "quantised.bin");
            var path2 = Path.Combine(dir, "raw.bin");

            var reader = new Reader();
            var sections = reader.ReadShorts(path);
            var sections2 = reader.ReadFloats(path2);

            var bins = Enumerable.Repeat(0, 16).ToArray();
            foreach (var section in sections)
            {
                for (var i = 0; i < section.Count; i++)
                {
                    var val = section[i];
                    var copy = val;
                    var needsBits = 0;
                    while (val != 0)
                    {
                        val /= 2;
                        needsBits++;
                        bins[needsBits]++;
                    }
                }
            }

            for (var i = 0; i < bins.Length; i++)
            {
                var bin = bins[i];
                Console.WriteLine($"{i}: {bin}");
            }

            var blocker = new Blocker();
            var blocks = blocker.BlockSection(sections2[0]);

            var result = new List<byte>();

            //var ms = new MemoryStream();
            //var int7Writer = new Int7Writer(ms);
            //foreach (var value in sections[0])
            //{
            //    int7Writer.WriteInt(value);
            //}
            //var arr = ms.ToArray();
            //foreach (var val in arr)
            //{
            //    result.Add(val);
            //}

            for (var blockIndex = 0; blockIndex < blocks.Count; blockIndex++)
            {
                var block = blocks[blockIndex].Values;
                var divisor = blocks[blockIndex].Divisor;

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
