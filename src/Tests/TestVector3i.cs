using NUnit.Framework;

public class TestVector3i
{
    [Test]
    public static void Test()
    {
        // Console.WriteLine($"int:   {int.MaxValue:N0}, maxSize={Math.Sqrt(int.MaxValue / 64):N0}");
        // Console.WriteLine($"uint:  {uint.MaxValue:N0}, maxSize={Math.Sqrt(uint.MaxValue / 64):N0}");
        // Console.WriteLine($"long:  {long.MaxValue:N0}, maxSize={Math.Sqrt(long.MaxValue / 64):N0}");
        // Console.WriteLine($"ulong: {ulong.MaxValue:N0}, maxSize={Math.Sqrt(ulong.MaxValue / 64):N0}");

        var vec3 = new Vector3i(0, 0, 0);

        Assert.That(vec3.x, Is.EqualTo(0));
        Assert.That(vec3.y, Is.EqualTo(0));
        Assert.That(vec3.z, Is.EqualTo(0));

        vec3.x = 255;
        vec3.y = 255;
        vec3.z = 255;

        Assert.That(vec3.x, Is.EqualTo(255));
        Assert.That(vec3.y, Is.EqualTo(255));
        Assert.That(vec3.z, Is.EqualTo(255));

        for (int x = 0; x < 256; x++)
        {
            for (int y = 0; y < 256; y++)
            {
                for (int z = 0; z < 256; z++)
                {
                    var vec = new Vector3i(x, y, z);

                    Assert.That(vec.x, Is.EqualTo(x));
                    Assert.That(vec.y, Is.EqualTo(y));
                    Assert.That(vec.z, Is.EqualTo(z));
                }
            }
        }
    }
}