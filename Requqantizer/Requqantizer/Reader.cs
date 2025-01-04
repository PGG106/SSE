using System.Security.Cryptography.X509Certificates;

namespace Requqantizer
{
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

        public Sections ReadBoth(string pathShorts, string pathFloats)
        {
            var sections = new Sections();
            sections.Shorts = ReadShorts(pathShorts);
            sections.Floats = ReadFloats(pathFloats);
            return sections;
        }
    }
}
