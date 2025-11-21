public struct Vector3i
{
    public int rawData;

    public int x
    {
        get => rawData >> 16;
        set => rawData = (rawData & 0x00FFFF) | (value << 16);
    }

    public int y
    {
        get => (rawData >> 8) & 0xFF;
        set => rawData = (rawData & 0xFF00FF) | (value << 8);
    }

    public int z
    {
        get => rawData & 0xFF;
        set => rawData = (rawData & 0xFFFF00) | value;
    }

    public Vector3i(int x, int y, int z)
    {
        rawData = (x << 16) | (y << 8) | z;
    }
}