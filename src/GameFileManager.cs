using System.IO;
using System.Linq;

public class GameFileManager(string gamePath)
{
    private const string pathB41 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB41";

    private const string pathB42 = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42";

    public static readonly GameFileManager B41 = new GameFileManager(pathB41);

    public static readonly GameFileManager B42 = new GameFileManager(pathB42);

    public string Media => $"{gamePath}/media";

    public string TexturePacksDir => $"{Media}/texturepacks";

    public string MapsDir => $"{Media}/maps";

    public string[] TilesDefinitionFiles => Directory.GetFiles(Media)
        .Where(path => path.EndsWith(".tiles"))
        .ToArray();
}