using System.Xml.Linq;

public class TileSet
{
    public class SourceImage
    {
        public string Source { get; set; }

        public int Width { get; set; }

        public int Height { get; set; }

        public static SourceImage Parse(XElement node)
        {
            return new SourceImage()
            {
                Source = node.Attribute("source").Value,
                Width = int.Parse(node.Attribute("width").Value),
                Height = int.Parse(node.Attribute("height").Value),
            };
        }
    }

    public int Firstgid { get; set; }

    public string Name { get; set; }

    public int Tilewidth { get; set; }

    public int Tileheight { get; set; }

    public SourceImage ImageSource { get; set; }

    public static TileSet Parse(XElement node)
    {
        return new TileSet()
        {
            Firstgid = int.Parse(node.Attribute("firstgid").Value),
            Name = node.Attribute("name").Value,
            Tilewidth = int.Parse(node.Attribute("tilewidth").Value),
            Tileheight = int.Parse(node.Attribute("tileheight").Value),
            ImageSource = SourceImage.Parse(node.Element("image")),
        };
    }
}