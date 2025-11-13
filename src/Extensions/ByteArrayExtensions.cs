using System;
using System.Text;

public static class ByteArrayExtensions
{
    public static int ReadInt32(this byte[] bytes, ref int offset)
    {
        return BitConverter.ToInt32(bytes, (offset += 4) - 4);
    }

    public static string ReadLine(this byte[] bytes, ref int offset)
    {
        return ReadUntil(bytes, '\n', ref offset);
    }

    public static string ReadUntil(this byte[] bytes, char pattern, ref int offset)
    {
        byte bytePattern = (byte)pattern;

        for (int i = offset; i < bytes.Length; i++)
        {
            if (bytes[i] == bytePattern)
            {
                string text = Encoding.UTF8.GetString(bytes, offset, i - offset);
                offset = i + 1;

                return text;
            }
        }

        throw new Exception("Pattern not found");
    }
}