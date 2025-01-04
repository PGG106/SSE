namespace Requqantizer;

class NodeData
{
    public List<float> L1Weights { get; set; }
}

internal class Permutor
{
    public Sections Permute(Sections sections)
    {
        var rng = new Random(0);
        var nodeIndices = Enumerable.Range(0, Constants.L1_SIZE).ToArray();
        rng.Shuffle(nodeIndices);

        var newSections = new Sections();
        newSections.Shorts = sections.Shorts.Select(x => x.ToList()).ToList();
        newSections.Floats = sections.Floats.Select(x => x.ToList()).ToList();

        for (var oldNodeIndex = 0; oldNodeIndex < Constants.L1_SIZE; oldNodeIndex++)
        {
            var newNodeIndex = nodeIndices[oldNodeIndex];

            // Input weights
            for (var inputIndex = 0; inputIndex < Constants.NUM_INPUTS; inputIndex++)
            {
                var oldInputOffset = oldNodeIndex * Constants.NUM_INPUTS + inputIndex;
                var newInputOffset = newNodeIndex * Constants.NUM_INPUTS + inputIndex;

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