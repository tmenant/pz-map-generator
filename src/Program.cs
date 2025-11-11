public class Program
{
    public static void Main(string[] args)
    {
        var txmFile = TmxFile.Read("ignore/Challenge1/tmx/0_0.tmx");

        txmFile.Save("ignore/tmx.xml");
    }
}