using System.IO;
using System.Text.Json;

public static class ObjectExtensions
{
    public static void ToJsonFile(this object obj, string path)
    {
        string directory = Path.GetDirectoryName(path);

        Directory.CreateDirectory(directory);

        string json = JsonSerializer.Serialize(obj, new JsonSerializerOptions()
        {
            WriteIndented = true,
        });

        File.WriteAllText(path, json);
    }
}