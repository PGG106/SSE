namespace Requqantizer;

class Block
{
    public List<float> Values { get; set; }
    public int Divisor { get; set; }

    public Block()
    {
        Values = new List<float>();
        Divisor = 0;
    }
}