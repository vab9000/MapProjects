using System.IO;
using System.Windows.Media;

namespace PopCapEstimate;

public class VegetationMap : ClassificationMap
{
    private static VegetationMap? _instance;

    public static VegetationMap GetInstance()
    {
        return _instance ??= new VegetationMap();
    }

    private VegetationMap() : base("assets/images/vegetation.png")
    {
        var classCsv = File.ReadAllLines("assets/classifications/vegetation.csv");
        foreach (var line in classCsv[1..])
        {
            var split = line.Split(',');
            Classification.Add(Color.FromArgb(255, byte.Parse(split[5]), byte.Parse(split[6]), byte.Parse(split[7])),
                split[3]);
        }
    }
}