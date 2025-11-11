using System.Xml.Linq;

public class TileSet
{
    public class Image
    {
        public string Source { get; set; }

        public int Width { get; set; }

        public int Height { get; set; }

        public static Image Parse(XElement node)
        {
            return new Image()
            {
                Source = node.Attribute("source").Value,
                Width = int.Parse(node.Attribute("width").Value),
                Height = int.Parse(node.Attribute("height").Value),
            };
        }

        public XElement ToXml()
        {
            return new XElement("image",
                new XAttribute("source", Source),
                new XAttribute("width", Width),
                new XAttribute("height", Height)
            );
        }
    }

    public int Firstgid { get; set; }

    public string Name { get; set; }

    public int Tilewidth { get; set; }

    public int Tileheight { get; set; }

    public Image ImageSource { get; set; }

    public static TileSet Parse(XElement node)
    {
        return new TileSet()
        {
            Firstgid = int.Parse(node.Attribute("firstgid").Value),
            Name = node.Attribute("name").Value,
            Tilewidth = int.Parse(node.Attribute("tilewidth").Value),
            Tileheight = int.Parse(node.Attribute("tileheight").Value),
            ImageSource = Image.Parse(node.Element("image")),
        };
    }

    public XElement ToXml()
    {
        return new XElement("tileset",
            new XAttribute("firstgid", Firstgid),
            new XAttribute("name", Name),
            new XAttribute("tilewidth", Tilewidth),
            new XAttribute("tileheight", Tileheight),
            ImageSource.ToXml()
        );
    }
}