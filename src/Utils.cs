using System.Diagnostics;
using System.Drawing;
using System.Text;

public static class Utils
{
    public static Stopwatch StartTimer()
    {
        var stopwatch = new Stopwatch();
        stopwatch.Start();

        return stopwatch;
    }

    public static byte[] Encode(string text, Encoding encoding = null)
    {
        if (encoding == null)
            encoding = Encoding.UTF8;

        return encoding.GetBytes(text);
    }

    public static Color SimpleColor(int value)
    {
        int r = (value >> 16) & 0xFF;
        int g = (value >> 8) & 0xFF;
        int b = value & 0xFF;

        return Color.FromArgb(r, g, b);
    }
}