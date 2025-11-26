using System;
using System.Drawing;
using System.IO;
using System.Linq;


public class Program
{
    public const string pathB41 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB41";

    public const string pathB42 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42";

    public static string GetTexturesDir(string gamePath) => $"{gamePath}/media/texturepacks";

    public static string GetMapDir(string gamePath) => $"{gamePath}/media/maps/Muldraugh, KY";

    public static void Main(string[] args)
    {
        string mapPath = GetMapDir(pathB42);

        var totalTimer = Utils.StartTimer();
        var filesCount = 0;

        foreach (var file in Directory.GetFiles(mapPath))
        {
            if (Path.GetFileName(file).EndsWith(".lotheader"))
            {
                // Console.WriteLine(file);
                var header = LotheaderFile.Read(file);
                filesCount++;
            }
        }

        Console.WriteLine($"{filesCount} read in {totalTimer.ElapsedMilliseconds / 1000:F3}s (average = {totalTimer.ElapsedMilliseconds / (float)filesCount:F3}ms / file)");

    }

    public static void test(string[] args)
    {
        var header = LotheaderFile.Read("ignore/B41/27_38.lotheader");
        var lotpack = LotpackFile.Read("ignore/B41/world_27_38.lotpack", header);

        var building = header.Buildings[0];
        var rooms = building.RoomIds.Select(id => header.Rooms[id]);

        var xmin = 300;
        var ymin = 300;
        var xmax = 0;
        var ymax = 0;

        foreach (var room in rooms)
        {
            foreach (var rect in room.Rectangles)
            {
                xmin = Math.Min(xmin, rect.X);
                ymin = Math.Min(ymin, rect.Y);

                xmax = Math.Max(xmax, rect.X + rect.Width);
                ymax = Math.Max(ymax, rect.Y + rect.Height);
            }
        }

        Console.WriteLine($"[{xmin}, {ymin}] [{xmax}, {ymax}]");
    }

    public static void TestMain()
    {
        TestReadWriteAllMapFiles(pathB41);
        TestReadWriteAllMapFiles(pathB42);

        TestReadAllPackFiles(pathB41);
        TestReadAllPackFiles(pathB42);

        ReadAllTileFiles(pathB41);
        ReadAllTileFiles(pathB42);
    }

    public static void TestLotpack()
    {
        var header = LotheaderFile.Read("ignore/B42/27_38.lotheader");
        var lotpack = LotpackFile.Read("ignore/B42/world_27_38.lotpack", header);

        Console.WriteLine($"BlockSizeInSquare: {header.BlockSizeInSquare}");

        var maxTiles = 0;

        for (int x = 0; x < 300; x++)
        {
            for (int y = 0; y < 300; y++)
            {
                var tile = lotpack.GetSquareData(x, y, 0);

                maxTiles = Math.Max(maxTiles, tile.Tiles.Length);

                // Console.WriteLine($"[{x}, {y}] " + string.Join(" ", tile.Tiles));
            }
        }

        Console.WriteLine($"maxTiles: {maxTiles}");
    }

    public static void ReadAllTileFiles(string gamePath)
    {
        var tilesPath = $"{gamePath}/media";

        foreach (var file in Directory.GetFiles(tilesPath))
        {
            if (!file.EndsWith(".tiles"))
                continue;

            try
            {
                TilesFile.Read(file).ToJsonFile($"ignore/tilesdef/{Path.GetFileName(file)}.json");
            }
            catch (Exception exc)
            {
                Console.WriteLine($"{exc.Message}: '{file}'");
                continue;
            }

            Console.WriteLine(file);
        }
    }

    public static void TestReadWriteAllMapFiles(string gamePath)
    {
        string mapPath = GetMapDir(gamePath);

        var totalTimer = Utils.StartTimer();
        var filesCount = 0;

        for (int x = 0; x < 128; x++)
        {
            for (int y = 0; y < 128; y++)
            {
                filesCount += TestReadWriteMapfile(mapPath, x, y);
            }
        }

        Console.WriteLine($"{filesCount} read in {totalTimer.ElapsedMilliseconds / 1000:F3}s (average = {totalTimer.ElapsedMilliseconds / filesCount}ms / file)");

    }

    public static int TestReadWriteMapfile(string directory, int x, int y)
    {
        string headerPath = $"{directory}/{x}_{y}.lotheader";
        string lotpackPath = $"{directory}/world_{x}_{y}.lotpack";

        if (!Path.Exists(headerPath))
            return 0;

        Console.WriteLine(Path.GetFileName(headerPath));

        var headerBytes = File.ReadAllBytes(headerPath);
        var lotpackBytes = File.ReadAllBytes(lotpackPath);

        var header = LotheaderFile.Read(headerBytes);
        var lotpack = LotpackFile.Read(lotpackPath, header);

        TestSaveLotheader(header, headerBytes);
        TestSaveLotpack(lotpack, lotpackBytes);

        return 1;
    }

    public static void TestSaveLotheader(LotheaderFile header, byte[] bytes)
    {
        var buffer = new MemoryStream();

        header.Save(buffer);

        var bytesAfter = buffer.ToArray();

        if (bytes.Length != bytesAfter.Length)
            throw new Exception($"Different length: {bytes.Length} / {bytesAfter.Length}");

        var md5_before = Utils.HashMd5(bytes);
        var md5_after = Utils.HashMd5(buffer.ToArray());

        if (md5_before != md5_after)
            throw new Exception($"Different hash: {md5_before} / {md5_after}");
    }

    public static void TestSaveLotpack(LotpackFile lotpack, byte[] bytes)
    {
        var buffer = new MemoryStream();

        lotpack.Save(buffer);

        var bytesAfter = buffer.ToArray();

        if (bytes.Length != bytesAfter.Length)
            throw new Exception($"Different length: {bytes.Length:N0} / {bytesAfter.Length:N0}");

        var md5_before = Utils.HashMd5(bytes);
        var md5_after = Utils.HashMd5(buffer.ToArray());

        if (md5_before != md5_after)
            throw new Exception($"Different hash: {md5_before} / {md5_after}");
    }

    public static void ExportBuilding()
    {
        var header = LotheaderFile.Read($"ignore/B41Map/27_38.lotheader");
        var pack = LotpackFile.Read("ignore/B41Map/world_27_38.lotpack", header);

        // var building = header.Buildings[37];
        // var room = header.Rooms[131];

        using (var drawing = new Drawing("ignore/building.png", 300, 300))
        {
            drawing.Clear(Color.Black);

            foreach (var building in header.Buildings)
            {
                var pen = new Pen(building.GetColor());

                foreach (var roomId in building.RoomIds)
                {
                    var room = header.Rooms[roomId];

                    foreach (var rect in room.Rectangles)
                    {
                        drawing.DrawRectangle(pen, rect);
                    }
                }
            }
        }
    }

    public static void TestReadAllPackFiles(string gamePath)
    {
        var packFiles = Directory.GetFiles(GetTexturesDir(gamePath));
        var pngDirectory = "ignore/tiles";

        Directory.CreateDirectory(pngDirectory);

        foreach (var file in packFiles)
        {
            if (!file.EndsWith(".pack"))
                continue;

            var filename = Path.GetFileName(file);
            var packFile = PackFile.Read(file);
            var texturesCount = 0;

            foreach (var page in packFile.Pages)
            {
                page.SavePng($"{pngDirectory}/{page.Name}.png");

                texturesCount += page.Textures.Length;
            }

            Console.WriteLine($"{filename}: version {packFile.Version}, {packFile.Pages.Length} pages, {texturesCount} textures");
        }
    }

    public static void TestReadAllMapFiles(string gamePath)
    {
        string mapPath = GetMapDir(gamePath);

        var totalTimer = Utils.StartTimer();
        var filesCount = 0;

        for (int x = 0; x < 99; x++)
        {
            for (int y = 0; y < 99; y++)
            {
                string headerPath = $"{mapPath}/{x}_{y}.lotheader";
                string lotpackPath = $"{mapPath}/world_{x}_{y}.lotpack";

                if (!Path.Exists(headerPath))
                    continue;

                Console.WriteLine(Path.GetFileName(headerPath));

                var header = LotheaderFile.Read(headerPath);
                var lotpack = LotpackFile.Read(lotpackPath, header);

                filesCount++;
            }
        }

        Console.WriteLine($"{filesCount} read in {totalTimer.ElapsedMilliseconds / 1000:F3}s (average = {totalTimer.ElapsedMilliseconds / filesCount}ms / file)");
    }

    public static void ReadTmxFile()
    {
        var txmFile = TmxFile.Read("ignore/Challenge1/tmx/0_0.tmx");

        txmFile.GetGID("0_FloorOverlay", 152, 163);

        // txmFile.Clear(128, 177, 130, 182);
        // txmFile.Clear(148, 151, 162, 161);
        // txmFile.Save("ignore/Challenge1/tmx/0_0.parsed.tmx");
    }
}