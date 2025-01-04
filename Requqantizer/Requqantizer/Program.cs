using System.Diagnostics;

namespace Requqantizer;

internal class Program
{
    static long GetCompressedSize(string path)
    {
        using var process = Process.Start("xz.exe", "-f -k -9 nn.net");
        process.WaitForExit();

        var compressedFileInfo = new FileInfo($"{path}.xz");
        var compressedSize = compressedFileInfo.Length;
        
        return compressedSize;
    }

    static long ProcessSections(Sections sections, string outPath)
    {
        var blocker = new Blocker();
        var serializer = new Serializer();

        var blocks = blocker.BlockSection(sections.Floats[0], Constants.BLOCK_SIZE, false);
        var result = serializer.Serialize(sections, blocks);
        File.WriteAllBytes(outPath, result);

        var size = GetCompressedSize(outPath);
        Console.WriteLine(size);
        return size;
    }

    static void Main(string[] args)
    {
        var dir = $"C:/shared/sse/nets/{Constants.L1_SIZE}";
        var pathShorts = Path.Combine(dir, "quantised.bin");
        var pathFloats = Path.Combine(dir, "raw.bin");
        var outPath = "nn.net";

        var reader = new Reader();
        var permutor = new Permutor();


        var sections = reader.ReadBoth(pathShorts, pathFloats);

        var bestSections = sections;
        var bestSize = ProcessSections(sections, outPath);

        for (var i = 0; i < 500; i++)
        {
            var sections2 = permutor.Permute(sections);
            var size = ProcessSections(sections2, outPath);
            if (size < bestSize)
            {
                bestSize = size;
                bestSections = sections2;
            }
        }

        ProcessSections(bestSections, outPath);
    }
}