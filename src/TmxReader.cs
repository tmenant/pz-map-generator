using System;
using System.Collections.Generic;
using System.IO;
using System.Xml.Linq;


public class TmxReader(string path)
{
    private readonly XDocument Document = XDocument.Load(path);

    private readonly Dictionary<int, TileSet> TileSets = new();

    private readonly Dictionary<string, Layer> Layers = new();

    private readonly Dictionary<string, ObjectGroup> ObjectGroups = new();

    public TmxReader Read()
    {
        ReadTilesets();

        return this;
    }

    private void ReadTilesets()
    {
        var xmlTileSets = Document.Descendants("tileset");

        foreach (var node in xmlTileSets)
        {
            var tileset = TileSet.Parse(node);

            this.TileSets[tileset.Firstgid] = tileset;
        }

        Console.WriteLine($"{TileSets.Count} tilesets parsed.");
    }
}