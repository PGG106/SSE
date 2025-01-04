using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Requqantizer;

internal class Program
{
    static long GetCompressedSize(string path, bool isWindows)
    {
        var xzName = isWindows ? "xz.exe" : "xz";
        using var process = Process.Start(xzName, "-f -k -9 nn.net");
        process.WaitForExit();

        var compressedFileInfo = new FileInfo($"{path}.xz");
        var compressedSize = compressedFileInfo.Length;
        
        return compressedSize;
    }

    static long ProcessSections(Sections sections, string outPath, bool isWindows)
    {
        var blocker = new Blocker();
        var serializer = new Serializer();

        var blocks = blocker.BlockSection(sections.Floats[0], Constants.BLOCK_SIZE, false);
        var result = serializer.Serialize(sections, blocks);
        File.WriteAllBytes(outPath, result);

        var size = GetCompressedSize(outPath, isWindows);
        Console.WriteLine(size);
        return size;
    }

    static void Main(string[] args)
    {
        bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);

        var baseDir = isWindows ? "C:/shared/sse/nets" : "/mnt/c/shared/sse/nets";
        var dir = $"{baseDir}/{Constants.L1_SIZE}";
        var pathShorts = Path.Combine(dir, "quantised.bin");
        var pathFloats = Path.Combine(dir, "raw.bin");
        var outPath = "nn.net";

        var reader = new Reader();
        var permutor = new Permutor();

        var sections = reader.ReadBoth(pathShorts, pathFloats);

        var bestSections = sections;
        var bestSize = ProcessSections(sections, outPath, isWindows);

        for (var i = 0; i < 100; i++)
        {
            var sections2 = permutor.Permute(sections);
            var size = ProcessSections(sections2, outPath, isWindows);
            if (size < bestSize)
            {
                bestSize = size;
                bestSections = sections2;
            }
        }

        ProcessSections(bestSections, outPath, isWindows);
    }
}