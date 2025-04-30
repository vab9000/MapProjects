using System.Collections.Concurrent;
using OpenCvSharp;
using PopCapEstimate2;

var baseMap = new MapImage("images/base.png");
var densityMap = new MapImage("images/density.png");
// var elevationMap = new MapImage("images/elevation.png");
var koppenMap = new MapImage("images/koppen.png");
var soilMap = new MapImage("images/soil.png");
var vegetationMap = new MapImage("images/vegetation.png");

Dictionary<Vec3b, string> koppenTable = new();
{
    var koppenCsv = File.ReadAllLines("classifications/koppen.csv");

    foreach (var line in koppenCsv[1..])
    {
        var parts = line.Split(',');
        var color = new Vec3b(byte.Parse(parts[3]), byte.Parse(parts[2]), byte.Parse(parts[1]));
        var classification = string.Intern(parts[0]);
        koppenTable[color] = classification;
    }
}

Dictionary<Vec3b, string> soilTable = new();
{
    var soilCsv = File.ReadAllLines("classifications/soil.csv");
    foreach (var line in soilCsv[1..])
    {
        var parts = line.Split(',');
        var color = new Vec3b(byte.Parse(parts[3]), byte.Parse(parts[2]), byte.Parse(parts[1]));
        var classification = string.Intern(parts[0]);
        soilTable[color] = classification;
    }
}

Dictionary<Vec3b, string> vegetationTable = new();
{
    var vegetationCsv = File.ReadAllLines("classifications/vegetation.csv");
    foreach (var line in vegetationCsv[1..])
    {
        var parts = line.Split(',');
        var color = new Vec3b(byte.Parse(parts[3]), byte.Parse(parts[2]), byte.Parse(parts[1]));
        var classification = string.Intern(parts[0]);
        vegetationTable[color] = classification;
    }
}

var table = new ConcurrentDictionary<KeyCombination, Pair<int, int>>();

Parallel.ForEach(baseMap, (pair) =>
{
    var (x, y) = pair;
    
    var baseColor = baseMap[x, y];

    if (!baseColor.Equals(new Vec3b(0, 0, 0)))
    {
        return;
    }

    var densityColor = densityMap[x, y];
    // var elevationColor = elevationMap[x, y];
    var koppenColor = koppenMap[x, y];
    var soilColor = soilMap[x, y];
    var vegetationColor = vegetationMap[x, y];

    var koppenKey = koppenTable[koppenColor];
    if (koppenKey.Equals("None"))
    {
        return;
    }

    var soilKey = soilTable[soilColor];
    if (soilKey.Equals("None"))
    {
        return;
    }

    var vegetationKey = vegetationTable[vegetationColor];
    if (vegetationKey.Equals("None"))
    {
        return;
    }

    var key = new KeyCombination(koppenKey, soilKey, vegetationKey);
    if (!table.TryGetValue(key, out var value))
    {
        value = new Pair<int, int>(0, 0);
        table[key] = value;
    }

    value.First++;
    value.Second += (densityColor.Item0 + densityColor.Item1 + densityColor.Item2);
});

var output = new List<string> { "Koppen,Soil,Vegetation,Count,Average" };
foreach (var (key, value) in table)
{
    var average = (double)value.Second / value.First;
    output.Add($"{key},{value.First},{average}");
}

File.WriteAllLines("output.csv", output);
