using System.Linq;
using System.Xml.Linq;


public class ObjectGroup
{
    public class Object
    {
        public string Name { get; set; }

        public string Type { get; set; }

        public int X { get; set; }

        public int Y { get; set; }

        public int Width { get; set; }

        public int Height { get; set; }

        public static Object Parse(XElement node)
        {
            return new Object()
            {
                Name = node.Attribute("name").Value,
                Type = node.Attribute("type").Value,
                X = int.Parse(node.Attribute("x").Value),
                Y = int.Parse(node.Attribute("y").Value),
                Width = int.Parse(node.Attribute("width").Value),
                Height = int.Parse(node.Attribute("height").Value),
            };
        }

        public XElement ToXml()
        {
            return new XElement("object",
                new XAttribute("name", Name),
                new XAttribute("type", Type),
                new XAttribute("x", X),
                new XAttribute("y", Y),
                new XAttribute("width", Width),
                new XAttribute("height", Height)
            );
        }
    }

    public string Name { get; set; }

    public int Width { get; set; }

    public int Height { get; set; }

    public Object[] Objects { get; set; }

    public static ObjectGroup Parse(XElement node)
    {
        return new ObjectGroup()
        {
            Name = node.Attribute("name").Value,
            Width = int.Parse(node.Attribute("width").Value),
            Height = int.Parse(node.Attribute("height").Value),
            Objects = node.Descendants("object").Select(Object.Parse).ToArray(),
        };
    }

    public XElement ToXml()
    {
        var element = new XElement("objectgroup",
            new XAttribute("name", Name),
            new XAttribute("width", Width),
            new XAttribute("height", Height)
        );

        foreach (var obj in Objects)
        {
            element.Add(obj.ToXml());
        }

        return element;
    }
}