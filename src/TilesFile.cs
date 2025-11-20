using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

public class TilesFile
{
    private static readonly byte[] magic = Encoding.UTF8.GetBytes("tdef");

    public int Version { get; set; }

    public TileSheet[] TileSheets { get; set; }

    public static TilesFile Read(string path)
    {
        var tilesdef = new TilesFile();
        var bytes = File.ReadAllBytes(path);
        var position = 0;

        tilesdef.ReadVersion(bytes, ref position);
        tilesdef.ReadTileSheets(bytes, ref position);

        if (position != bytes.Length)
        {
            throw new Exception($"End of file not reached: {position:N0} / {bytes.Length:N0} ('{path}')");
        }

        return tilesdef;
    }

    private void ReadVersion(byte[] bytes, ref int position)
    {
        if (bytes[0..4].SequenceEqual(magic))
        {
            position += 4;
            Version = bytes.ReadInt32(ref position);
        }
        else
        {
            throw new Exception("Tile file not supported");
        }
    }

    private void ReadTileSheets(byte[] bytes, ref int position)
    {
        var tilesheetCount = bytes.ReadInt32(ref position);

        TileSheets = new TileSheet[tilesheetCount];

        for (int i = 0; i < tilesheetCount; i++)
        {
            TileSheets[i] = TileSheet.Read(bytes, ref position);
        }
    }
}

public class TileSheet
{
    public string Name { get; set; }

    public string ImageName { get; set; }

    public int TileWidth { get; set; }

    public int TileHeight { get; set; }

    public int Number { get; set; }

    public int TilesCount { get; set; }

    public TileDefinition[] TileDefinitions { get; set; }

    public static TileSheet Read(byte[] bytes, ref int position)
    {
        var tilesheet = new TileSheet();

        tilesheet.Name = bytes.ReadLine(ref position).ReplaceLineEndings("");
        tilesheet.ImageName = bytes.ReadLine(ref position).ReplaceLineEndings("");
        tilesheet.TileWidth = bytes.ReadInt32(ref position);
        tilesheet.TileHeight = bytes.ReadInt32(ref position);
        tilesheet.Number = bytes.ReadInt32(ref position);
        tilesheet.TilesCount = bytes.ReadInt32(ref position);

        tilesheet.TileDefinitions = new TileDefinition[tilesheet.TilesCount];

        for (int i = 0; i < tilesheet.TilesCount; i++)
        {
            tilesheet.TileDefinitions[i] = TileDefinition.Read(bytes, tilesheet, i, ref position);
        }

        return tilesheet;
    }

    public int GenerateSpriteID(int index)
    {
        return -1;
    }
}

public class TileDefinition
{
    public string Name { get; set; }

    public int SpriteID { get; set; }

    public Dictionary<string, string> Properties { get; set; }

    public static TileDefinition Read(byte[] bytes, TileSheet tileSheet, int tileIndex, ref int position)
    {
        var tileDef = new TileDefinition();

        tileDef.SpriteID = tileSheet.GenerateSpriteID(tileIndex);
        tileDef.Name = $"{tileSheet.Name}_{tileIndex}";
        tileDef.ReadProperties(bytes, ref position);

        return tileDef;
    }

    private void ReadProperties(byte[] bytes, ref int position)
    {
        Properties = new Dictionary<string, string>();

        var propCount = bytes.ReadInt32(ref position);

        for (int i = 0; i < propCount; i++)
        {
            var name = bytes.ReadLine(ref position).ReplaceLineEndings("");
            var value = bytes.ReadLine(ref position).ReplaceLineEndings("");

            Properties[name] = value;
        }
    }
}