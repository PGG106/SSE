using static System.Collections.Specialized.BitVector32;
using System.Reflection.PortableExecutable;

namespace Requqantizer
{
    static class Constants
    {
        public const int NUM_INPUTS = 768;
        public const int L1_SIZE = 80;
        public const int OUTPUT_BUCKETS = 1;

        public const int FT_QUANT = 255;
        public const int L1_QUANT = 64;

        public const int blockSize = L1_SIZE * 4;

        public static int[] SectionSizes = new[]
        {
            Constants.NUM_INPUTS* Constants.L1_SIZE,
            Constants.L1_SIZE,
            Constants.L1_SIZE* 2 * Constants.OUTPUT_BUCKETS,
            Constants.OUTPUT_BUCKETS
        };
}

    internal class Reader
    {
        public List<List<short>> ReadShorts(string path)
        {
            var bytes = File.ReadAllBytes(path);
            Console.WriteLine($"Original file size: {bytes.Length} bytes");
            
            var unpaddedSize = Constants.SectionSizes.Sum() * 2;
            Console.WriteLine($"Unpadded file size: {unpaddedSize} bytes");

            var sections = new List<List<short>>();
            var reader = new BinaryReader(new MemoryStream(bytes));
            for (var sectionIndex = 0; sectionIndex < Constants.SectionSizes.Length; sectionIndex++)
            {
                var sectionSize = Constants.SectionSizes[sectionIndex];
                Console.Write($"Section {sectionIndex}: Count: {sectionSize}: ");

                var data = new List<short>();
                for (var i = 0; i < sectionSize; i++)
                {
                    var value = reader.ReadInt16();
                    data.Add(value);
                }
                sections.Add(data);
                Console.WriteLine($"Min: {data.Min()} Max: {data.Max()}");
            }
            return sections;
        }

        public List<List<float>> ReadFloats(string path)
        {
            var bytes = File.ReadAllBytes(path);
            Console.WriteLine($"Original file size: {bytes.Length} bytes");

            var unpaddedSize = Constants.SectionSizes.Sum() * 2;
            Console.WriteLine($"Unpadded file size: {unpaddedSize} bytes");

            var sections = new List<List<float>>();
            var reader = new BinaryReader(new MemoryStream(bytes));
            for (var sectionIndex = 0; sectionIndex < Constants.SectionSizes.Length; sectionIndex++)
            {
                var sectionSize = Constants.SectionSizes[sectionIndex];
                Console.Write($"Section {sectionIndex}: Count: {sectionSize}: ");

                var data = new List<float>();
                for (var i = 0; i < sectionSize; i++)
                {
                    var value = reader.ReadSingle();
                    data.Add(value);
                }
                sections.Add(data);
                Console.WriteLine($"Min: {data.Min()} Max: {data.Max()}");
            }
            return sections;
        }
    }
}
