using System.Linq;
using System.Xml.Linq;

public class Layer
{
    public string Name { get; set; }

    public int Width { get; set; }

    public int Height { get; set; }

    public int[] Datas { get; set; }

    public static Layer Parse(XElement node)
    {
        return new Layer()
        {
            Name = node.Attribute("name").Value,
            Width = int.Parse(node.Attribute("width").Value),
            Height = int.Parse(node.Attribute("height").Value),
            Datas = node.Value.Split(",").Select(int.Parse).ToArray(),
        };
    }

    public XElement ToXml()
    {
        var csvDatas = "\n" + string.Join(",", Datas.Select(value => value.ToString()));

        return new XElement("layer",
            new XAttribute("name", Name),
            new XAttribute("width", Width),
            new XAttribute("height", Height),
            new XElement("data", new XAttribute("encoding", "csv"), csvDatas)
        );
    }
}