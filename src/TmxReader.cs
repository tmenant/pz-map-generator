using System;
using System.Collections.Generic;
using System.Xml.Linq;


public class TmxFile()
{
    private readonly Dictionary<int, TileSet> TileSets = new();

    private readonly Dictionary<string, Layer> Layers = new();

    private readonly Dictionary<string, ObjectGroup> ObjectGroups = new();

    public static TmxFile Read(string path)
    {
        var document = XDocument.Load(path);
        var tmxFile = new TmxFile();

        tmxFile.ReadTilesets(document);
        tmxFile.ReadLayers(document);
        tmxFile.ReadObjectGroups(document);

        return tmxFile;
    }

    private void ReadTilesets(XDocument document)
    {
        var xmlTileSets = document.Descendants("tileset");

        foreach (var node in xmlTileSets)
        {
            var tileset = TileSet.Parse(node);
            this.TileSets[tileset.Firstgid] = tileset;
        }

        Console.WriteLine($"{TileSets.Count} tilesets parsed.");
    }

    public void ReadLayers(XDocument document)
    {
        var xmlLayers = document.Descendants("layer");

        foreach (var node in xmlLayers)
        {
            var layer = Layer.Parse(node);
            this.Layers[layer.Name] = layer;
        }

        Console.WriteLine($"{Layers.Count} layers parsed.");
    }

    public void ReadObjectGroups(XDocument document)
    {
        var xmlObjectGroups = document.Descendants("objectgroup");

        foreach (var node in xmlObjectGroups)
        {
            var objectGroup = ObjectGroup.Parse(node);
            this.ObjectGroups[objectGroup.Name] = objectGroup;
        }

        Console.WriteLine($"{ObjectGroups.Count} objectgroups parsed.");
    }
}