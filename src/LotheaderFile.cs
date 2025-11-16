using System;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;


public class LotheaderFile
{
    private static readonly byte[] magic = Encoding.UTF8.GetBytes("LOTH");

    public int CellSizeInBlock { get; set; }

    public int BlockSizeInSquare { get; set; }

    public int MinLayer { get; set; }

    public int MaxLayer { get; set; }

    public int Version { get; set; }

    public int Width { get; set; }

    public int Height { get; set; }

    public string[] TileNames { get; set; }

    public Room[] Rooms { get; set; }

    public Building[] Buildings { get; set; }

    public byte[] ZombieSpawns { get; set; }

    public static LotheaderFile Read(string path)
    {
        var header = new LotheaderFile();
        var bytes = File.ReadAllBytes(path);
        var position = 0;

        header.ReadVersion(bytes, ref position);
        header.ReadTileDefs(bytes, ref position);
        header.UpdateLimitationInfos();

        if (header.Version == 0) // B41, skip 0x00
            position++;

        header.ReadSize(bytes, ref position);
        header.ReadLayers(bytes, ref position);
        header.ReadRooms(bytes, ref position);
        header.ReadBuildings(bytes, ref position);
        header.ReadZombieSpawns(bytes, ref position);

        header.ToJsonFile("ignore/header.json");

        if (position != bytes.Length)
        {
            throw new Exception($"End of file not reached: {position:N0} / {bytes.Length:N0}");
        }

        return header;
    }

    private void UpdateLimitationInfos()
    {
        if (Version == 0) // B41
        {
            CellSizeInBlock = 30;
            BlockSizeInSquare = 10;
            MinLayer = 0;
            MaxLayer = 8;
        }
        else
        {
            CellSizeInBlock = 32;
            BlockSizeInSquare = 8;
            MinLayer = -32;
            MaxLayer = 32;
        }
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
            Version = bytes.ReadInt32(ref position);
        }
    }

    private void ReadTileDefs(byte[] bytes, ref int position)
    {
        int tilesCount = bytes.ReadInt32(ref position);

        TileNames = new string[tilesCount];

        for (int i = 0; i < tilesCount; i++)
        {
            TileNames[i] = bytes.ReadLine(ref position);
        }
    }

    private void ReadSize(byte[] bytes, ref int position)
    {
        Width = bytes.ReadInt32(ref position);
        Height = bytes.ReadInt32(ref position);
    }

    private void ReadLayers(byte[] bytes, ref int position)
    {
        if (Version == 0) // B41
        {
            MinLayer = 0;
            MaxLayer = Math.Min(MaxLayer, bytes.ReadInt32(ref position));
        }
        else // B42
        {
            MinLayer = Math.Max(MinLayer, bytes.ReadInt32(ref position));
            MaxLayer = Math.Min(MaxLayer, bytes.ReadInt32(ref position) + 1);
        }
    }

    private void ReadRooms(byte[] bytes, ref int position)
    {
        var roomsCount = bytes.ReadInt32(ref position);

        Rooms = new Room[roomsCount];

        for (int i = 0; i < roomsCount; i++)
        {
            Rooms[i] = Room.Read(i, bytes, ref position);
        }
    }

    private void ReadBuildings(byte[] bytes, ref int position)
    {
        var buildingsCount = bytes.ReadInt32(ref position);

        Buildings = new Building[buildingsCount];

        for (int i = 0; i < buildingsCount; i++)
        {
            Buildings[i] = Building.Read(i, bytes, ref position);
        }
    }

    private void ReadZombieSpawns(byte[] bytes, ref int position)
    {
        ZombieSpawns = new byte[CellSizeInBlock * CellSizeInBlock];

        for (int x = 0; x < CellSizeInBlock; x++)
        {
            for (int y = 0; y < CellSizeInBlock; y++)
            {
                ZombieSpawns[y + x * CellSizeInBlock] = bytes[position++];
            }
        }
    }

    public void ToJsonFile(string path)
    {
        string json = JsonSerializer.Serialize(this, new JsonSerializerOptions()
        {
            WriteIndented = true,
        });

        File.WriteAllText(path, json);
    }
}


public class Building
{
    public int Id { get; set; }

    public int[] RoomIds { get; set; }

    public static Building Read(int id, byte[] bytes, ref int position)
    {
        var building = new Building();
        var roomsCount = bytes.ReadInt32(ref position);

        building.Id = id;
        building.RoomIds = new int[roomsCount];

        for (int i = 0; i < roomsCount; i++)
        {
            building.RoomIds[i] = bytes.ReadInt32(ref position);
        }

        return building;
    }

    public Color SimpleColor()
    {
        return Utils.SimpleColor(GetHashCode());
    }
}


public class Room
{
    public int Id { get; set; }

    public string Name { get; set; }

    public int Layer { get; set; }

    public int Area { get; set; }

    public Rectangle[] Rectangles { get; set; }

    public RoomObject[] RoomObjects { get; set; }

    public static Room Read(int id, byte[] bytes, ref int position)
    {
        var room = new Room();

        room.Id = id;
        room.Name = bytes.ReadLine(ref position);
        room.Layer = bytes.ReadInt32(ref position);

        room.ReadRectangles(bytes, ref position);
        room.ReadMeta(bytes, ref position);

        return room;
    }

    private void ReadRectangles(byte[] bytes, ref int position)
    {
        var rectanglesCount = bytes.ReadInt32(ref position);

        Rectangles = new Rectangle[rectanglesCount];

        for (int i = 0; i < rectanglesCount; i++)
        {
            var x = bytes.ReadInt32(ref position);
            var y = bytes.ReadInt32(ref position);
            var width = bytes.ReadInt32(ref position);
            var height = bytes.ReadInt32(ref position);

            Rectangles[i] = new Rectangle(x, y, width, height);

            Area += width * height;
        }
    }

    private void ReadMeta(byte[] bytes, ref int position)
    {
        var metaCount = bytes.ReadInt32(ref position);

        RoomObjects = new RoomObject[metaCount];

        for (int i = 0; i < metaCount; i++)
        {
            RoomObjects[i] = new RoomObject()
            {
                Type = bytes.ReadInt32(ref position),
                X = bytes.ReadInt32(ref position),
                Y = bytes.ReadInt32(ref position),
            };
        }
    }

    public override string ToString()
    {
        return JsonSerializer.Serialize(this);
    }
}


public struct RoomObject
{
    public int Type { get; set; }

    public int X { get; set; }

    public int Y { get; set; }
}


public struct Rectangle
{
    public int X { get; set; }

    public int Y { get; set; }

    public int Width { get; set; }

    public int Height { get; set; }

    public Rectangle(int x, int y, int w, int h)
    {
        X = x;
        Y = y;
        Width = w;
        Height = h;
    }
}