namespace Requqantizer;

internal class Permutor
{
    private readonly Random _rng;

    public Permutor(Random? rng = null)
    {
        _rng = rng ?? new Random(0);
    }

    private int[] GetNodeIndices()
    {
        var nodeIndices = Enumerable.Range(0, Constants.L1_SIZE).ToArray();
        _rng.Shuffle(nodeIndices);
        return nodeIndices;
    }

    public Sections Permute(Sections sections)
    {
        var nodeIndices = GetNodeIndices();

        var newSections = new Sections();
        newSections.Shorts = sections.Shorts.Select(x => x.ToList()).ToList();
        newSections.Floats = sections.Floats.Select(x => x.ToList()).ToList();

        for (var oldNodeIndex = 0; oldNodeIndex < Constants.L1_SIZE; oldNodeIndex++)
        {
            var newNodeIndex = nodeIndices[oldNodeIndex];

            // Input weights
            for (var inputIndex = 0; inputIndex < Constants.NUM_INPUTS; inputIndex++)
            {
                var oldInputOffset = inputIndex * Constants.L1_SIZE + oldNodeIndex;
                var newInputOffset = inputIndex * Constants.L1_SIZE + newNodeIndex;

                newSections.Shorts[0][newInputOffset] = sections.Shorts[0][oldInputOffset];
                newSections.Floats[0][newInputOffset] = sections.Floats[0][oldInputOffset];
            }

            // Input biases
            newSections.Shorts[1][newNodeIndex] = sections.Shorts[1][newNodeIndex];
            newSections.Floats[1][newNodeIndex] = sections.Floats[1][newNodeIndex];
        }

        return newSections;
    }
}