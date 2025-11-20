using System.IO;

public static class BinaryWriterExtensions
{
    public static int Position(this BinaryWriter writer)
    {
        return (int)writer.BaseStream.Position;
    }
}