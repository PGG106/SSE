namespace Requqantizer;

internal class Program
{
    static void Main(string[] args)
    {
        var dir = $"C:/shared/sse/nets/{Constants.L1_SIZE}";
        var pathShorts = Path.Combine(dir, "quantised.bin");
        var pathFloats = Path.Combine(dir, "raw.bin");

        var reader = new Reader();
        var sections = reader.ReadBoth(pathShorts, pathFloats);

        var permutor = new Permutor();
        sections = permutor.Permute(sections);
        
        var blocker = new Blocker();
        var blocks = blocker.BlockSection(sections.Floats[0], Constants.BLOCK_SIZE);

        var serializer = new Serializer();
        var result = serializer.Serialize(sections, blocks);

        File.WriteAllBytes("nn.net", result);
    }
}