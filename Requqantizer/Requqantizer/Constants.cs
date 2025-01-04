namespace Requqantizer;

static class Constants
{
    public const int NUM_INPUTS = 768;
    public const int L1_SIZE = 80;
    public const int OUTPUT_BUCKETS = 8;

    public const int FT_QUANT = 255;
    public const int L1_QUANT = 64;

    public const int BLOCK_SIZE = L1_SIZE * 4;

    public static int[] SectionSizes = new[]
    {
        Constants.NUM_INPUTS* Constants.L1_SIZE,
        Constants.L1_SIZE,
        Constants.L1_SIZE* 2 * Constants.OUTPUT_BUCKETS,
        Constants.OUTPUT_BUCKETS
    };
}