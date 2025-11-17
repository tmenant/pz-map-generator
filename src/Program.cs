using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

public class Program
{
    public const string pathB41 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB41";

    public const string pathB42 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42";

    public const string texturePacks = $"{pathB41}/media/texturepacks";

    public static string MapPath(string gamePath) => $"{gamePath}/media/maps/Muldraugh, KY";

    public static void Main(string[] args)
    {
        // ReadAllPackFiles();
        // ReadAllMapFiles(pathB41);
        // ReadAllMapFiles(pathB42);

        ExportBuilding();
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
                var pen = new Pen(building.SimpleColor());

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

    public static void ReadAllPackFiles()
    {
        var packFiles = Directory.GetFiles(texturePacks);

        foreach (var file in packFiles)
        {
            string filename = Path.GetFileName(file);

            try
            {
                PackFile.Read(file);

                Console.WriteLine(filename);
            }
            catch (Exception exc)
            {
                Console.WriteLine($"Error while reading '{filename}' : {exc.Message}");
            }
        }
    }

    public static void ReadAllMapFiles(string gamePath)
    {
        string mapPath = MapPath(gamePath);

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