using System.Windows.Media;
using System.IO;

namespace PopCapEstimate;

public class ClimateMap : ClassificationMap
{
    private static ClimateMap? _instance;

    public static ClimateMap GetInstance()
    {
        return _instance ??= new ClimateMap();
    }

    private ClimateMap() : base("assets/images/koppen.png")
    {
        var classCsv = File.ReadAllLines("assets/classifications/koppen.csv");
        foreach (var line in classCsv[1..])
        {
            var split = line.Split(',');
            Classification.Add(Color.FromArgb(255, byte.Parse(split[1]), byte.Parse(split[2]), byte.Parse(split[3])),
                split[0]);
        }
    }
}