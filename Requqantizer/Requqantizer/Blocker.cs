namespace Requqantizer;

internal class Blocker
{
    public List<Block> BlockSection(List<float> section, int blockSize, bool printDetails = true)
    {
        var blockCount = section.Count / blockSize;
        if (printDetails)
        {
            Console.WriteLine($"Block size: {blockSize}");
            Console.WriteLine($"Block count: {blockCount}");
        }

        var queue = new Queue<float>(section);
        var blocks = new List<Block>();
        for (var blockIndex = 0; blockIndex < blockCount; blockIndex++)
        {
            var block = new Block();
            for (var i = 0; i < blockSize; i++)
            {
                var value = queue.Dequeue() * Constants.FT_QUANT;
                block.Values.Add(value);
            }
            blocks.Add(block);

            int divisor;
            for (divisor = 1; ; divisor++)
            {
                var min = Math.Round(block.Values.Min() / (double)divisor);
                var max = Math.Round(block.Values.Max() / (double)divisor);

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

            block.Divisor = divisor;

            if (printDetails)
            {
                Console.WriteLine($"Block {blockIndex}: Divisor: {divisor} Min: {block.Values.Min()} Max: {block.Values.Max()}");
            }
        }

        if (queue.Count > 0)
        {
            throw new Exception("Didnt read all from queue");
        }

        if (printDetails)
        {
            var divisorBins = Enumerable.Range(0, 5).ToArray();
            var divisorSum = 0f;
            foreach (var block in blocks)
            {
                divisorBins[block.Divisor]++;
                divisorSum += block.Divisor;
            }

            var avgDivisor = divisorSum / blockCount;
            Console.WriteLine($"Avg divisor: {avgDivisor}");

            for (int i = 0; i < 5; i++)
            {
                var bin = divisorBins[i];
                var percentage = bin * 100f / blockCount;
                Console.WriteLine($"Div {i}: {bin} ({percentage:0.0}%)");
            }
        }

        return blocks;
    }
}