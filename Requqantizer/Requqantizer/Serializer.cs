using static System.Reflection.Metadata.BlobBuilder;

namespace Requqantizer;

internal class Serializer
{
    public byte[] Serialize(Sections sections, List<Block> blocks)
    {
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

        foreach (var value in sections.Shorts[1])
        {
            //result.Add((byte)(sbyte)value);

            //var sht = (short)Math.Round(value * FT_QUANT);
            //result.AddRange(BitConverter.GetBytes(sht));

            result.AddRange(BitConverter.GetBytes(value));
        }

        //var transposedL1Weights = new short[Constants.L1_SIZE * 2 * Constants.OUTPUT_BUCKETS];
        //for (var weight = 0; weight < 2 * Constants.L1_SIZE; ++weight)
        //{
        //    for (var bucket = 0; bucket < Constants.OUTPUT_BUCKETS; ++bucket)
        //    {
        //        var srcIdx = weight * Constants.OUTPUT_BUCKETS + bucket;
        //        var dstIdx = bucket * 2 * Constants.L1_SIZE + weight;
        //        transposedL1Weights[dstIdx] = sections[2][srcIdx];
        //    }
        //}
        //sections[2] = transposedL1Weights.ToList();

        foreach (var value in sections.Shorts[2])
        {
            //var sht = (short)Math.Round(value * L1_QUANT);
            //result.AddRange(BitConverter.GetBytes(sht));
            result.Add((byte)(sbyte)value);
        }
        foreach (var value in sections.Shorts[3])
        {
            //var sht = (short)Math.Round(value * FT_QUANT * L1_QUANT);
            //result.AddRange(BitConverter.GetBytes(sht));

            result.AddRange(BitConverter.GetBytes(value));
        }

        var bytes = result.ToArray();
        return bytes;
    }
}