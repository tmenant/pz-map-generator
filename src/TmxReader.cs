using System.Collections.Generic;
using System.Xml;


public class TmxReader(string _path)
{
    public string Path { get; set; } = _path;

    public readonly Dictionary<int, TileSet> TileSets = new();

    public readonly Dictionary<string, Layer> Layers = new();

    public readonly Dictionary<string, ObjectGroup> ObjectGroups = new();

    public TmxReader Read()
    {
        var document = new XmlDocument();
        document.Load("ignore/Challenge1/tmx/0_0.tmx");

        return this;
    }
}