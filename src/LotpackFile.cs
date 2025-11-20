using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;


public class LotpackFile
{
    private static readonly byte[] magic = Encoding.UTF8.GetBytes("LOTP");

    public int Version { get; set; }

    public LotheaderFile Header { get; set; }

    public SquareData[][,,] ChunkDatas { get; set; }

    public static LotpackFile Read(string path, LotheaderFile header)
    {
        var bytes = File.ReadAllBytes(path);
        var lotpack = new LotpackFile();
        var position = 0;

        lotpack.Header = header;
        lotpack.ReadVersion(bytes, ref position);
        lotpack.ReadChunks(bytes, ref position);

        if (position != bytes.Length)
        {
            throw new Exception($"End of file not reached: {position:N0} / {bytes.Length:N0} ('{path}')");
        }

        return lotpack;
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
            Version = 0;
        }

        if (Version != Header.Version)
        {
            throw new Exception($"Inconsistant version: '{Version}', header: '{Header.Version}'");
        }
    }

    private void ReadChunks(byte[] bytes, ref int position)
    {
        var chunksCount = bytes.ReadInt32(ref position);
        var offsetTable = position;

        ChunkDatas = new SquareData[chunksCount][,,];

        for (int i = 0; i < chunksCount; i++)
        {
            position = bytes.ReadInt32(offsetTable + i * 8);
            ChunkDatas[i] = ReadSquareDatas(bytes, Header, ref position);
        }
    }

    private SquareData[,,] ReadSquareDatas(byte[] bytes, LotheaderFile header, ref int position)
    {
        var chunkSize = header.BlockSizeInSquare;
        var squarePerLayer = chunkSize * chunkSize;
        var minLayer = header.MinLayer;
        var maxLayer = header.MaxLayer;
        var skip = 0;

        var squareDatas = new SquareData[chunkSize, chunkSize, maxLayer - minLayer];

        for (int z = 0; z < maxLayer - minLayer; z++)
        {
            if (skip >= squarePerLayer)
            {
                skip -= squarePerLayer;
                continue;
            }

            for (int x = 0; x < chunkSize; x++)
            {
                if (skip >= chunkSize)
                {
                    skip -= chunkSize;
                    continue;
                }

                for (int y = 0; y < chunkSize; y++)
                {
                    if (skip > 0)
                    {
                        skip -= 1;
                        continue;
                    }

                    var count = bytes.ReadInt32(ref position);

                    if (count == -1)
                    {
                        skip = bytes.ReadInt32(ref position);

                        if (skip > 0)
                        {
                            skip -= 1;
                            continue;
                        }
                    }
                    if (count > 1)
                    {
                        squareDatas[x, y, z] = SquareData.FromLotpack(bytes, count - 1, ref position);
                    }
                }
            }
        }

        return squareDatas;
    }

    public void Save(string path)
    {
        using (var stream = new FileStream(path, FileMode.OpenOrCreate))
        {
            Save(stream);
        }
    }

    public void Save(Stream stream)
    {
        using var writer = new BinaryWriter(stream);

        WriteVersion(writer);
        WriteChunks(writer);
    }

    private void WriteVersion(BinaryWriter fileWriter)
    {
        if (Version != 0)
        {
            fileWriter.Write(magic);
            fileWriter.Write(Version);
        }
    }

    private void WriteChunks(BinaryWriter fileWriter)
    {
        fileWriter.Write(ChunkDatas.Length);

        using var buffer = new MemoryStream();
        using var buffWriter = new BinaryWriter(buffer);

        var tableOffset = fileWriter.Position() + ChunkDatas.Length * 8;

        for (int i = 0; i < ChunkDatas.Length; i++)
        {
            var sequencePosition = buffWriter.Position() + tableOffset;
            var chunkSequence = EncodeChunkSequence(ChunkDatas[i]);

            fileWriter.Write(sequencePosition);
            fileWriter.Write(0); // zero-padding

            buffWriter.Write(chunkSequence);
        }

        fileWriter.Write(buffer.ToArray());
    }

    private byte[] EncodeChunkSequence(SquareData[,,] squareDatas)
    {
        var squareSequence = new List<SquareData>();

        for (int z = 0; z < Header.MaxLayer - Header.MinLayer; z++)
        {
            for (int x = 0; x < Header.BlockSizeInSquare; x++)
            {
                for (int y = 0; y < Header.BlockSizeInSquare; y++)
                {
                    squareSequence.Add(squareDatas[x, y, z]);
                }
            }
        }

        return CompressSquareSequence(squareSequence);
    }

    private byte[] CompressSquareSequence(List<SquareData> squareSequence)
    {
        var sequence = new List<int>();

        int length = squareSequence.Count;

        for (int i = 0; i < squareSequence.Count; i++)
        {
            var squareData = squareSequence[i];

            if (squareData == null)
            {
                int skipCount = 0;
                int startZero = i;

                while (i < length && squareSequence[i] == null)
                {
                    skipCount++;
                    i++;
                }

                sequence.Add(-1);
                sequence.Add(skipCount);

                i--;
            }
            else
            {
                int count = squareSequence[i].Tiles.Length;
                int roomId = squareSequence[i].RoomId;

                sequence.Add(count + 1);
                sequence.Add(roomId);

                foreach (var tileID in squareData.Tiles)
                {
                    sequence.Add(tileID);
                }
            }
        }

        return sequence.SelectMany(BitConverter.GetBytes).ToArray();
    }

    public static void DebugSequence(byte[] bytes, string path)
    {
        Console.WriteLine(bytes.Length);

        var position = 0;

        if (bytes[0..4].SequenceEqual(magic))
        {
            position += 4;
        }

        var chunksCount = bytes.ReadInt32(ref position);
        var positions = new int[chunksCount];

        using var streamWriter = new StreamWriter(path);

        for (int i = 0; i < chunksCount; i++)
        {
            positions[i] = bytes.ReadInt32(ref position);
            position += 4;
        }

        for (int i = 0; i < chunksCount; i++)
        {
            var start = positions[i];
            var end = bytes.Length;
            var index = 0;

            if (i < chunksCount - 1)
            {
                end = positions[i + 1];
            }

            var sequence = bytes[start..end];
            var values = new int[sequence.Length / 4];

            for (int j = 0; j < sequence.Length; j += 4)
            {
                values[index++] = sequence.ReadInt32(j);
            }

            streamWriter.WriteLine(start.ToString().PadRight(7) + ": " + string.Join(" ", values.Select(v => v.ToString().PadLeft(6))));
        }
    }
}

public class SquareData
{
    public int[] Tiles { get; set; }

    public int RoomId { get; set; }

    public static SquareData FromLotpack(byte[] bytes, int count, ref int position)
    {
        var squareData = new SquareData()
        {
            Tiles = new int[count],
            RoomId = bytes.ReadInt32(ref position),
        };

        for (int i = 0; i < count; i++)
        {
            squareData.Tiles[i] = bytes.ReadInt32(ref position);
        }

        return squareData;
    }
}
