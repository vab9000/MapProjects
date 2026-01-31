using PopCapEstimate2;

var baseMap = new MapImage("images/base.png");

var keyCombinations = new KeyCombination[baseMap.Width, baseMap.Height];

{
    var koppenMap = new MapImage("images/koppen.png");
    foreach (var pixel in koppenMap)
    {
        ref var key = ref keyCombinations[pixel.First, pixel.Second];
        key.KoppenKey = koppenMap[pixel.First, pixel.Second];
    }
}
{
    var soilMap = new MapImage("images/soil.png");
    foreach (var pixel in soilMap)
    {
        ref var key = ref keyCombinations[pixel.First, pixel.Second];
        key.SoilKey = soilMap[pixel.First, pixel.Second];
    }
}
{
    var vegetationMap = new MapImage("images/vegetation.png");
    foreach (var pixel in vegetationMap)
    {
        ref var key = ref keyCombinations[pixel.First, pixel.Second];
        key.VegetationKey = vegetationMap[pixel.First, pixel.Second];
    }
}

foreach (var line in File.ReadAllLines("classifications/koppen.csv")[1..])
{
    var parts = line.Split(',');
    var color = (byte.Parse(parts[3]), byte.Parse(parts[2]), byte.Parse(parts[1]));
    var classification = parts[0];
    KeyCombination.KoppenTable[color] = classification;
}

foreach (var line in File.ReadAllLines("classifications/soil.csv")[1..])
{
    var parts = line.Split(',');
    var color = (byte.Parse(parts[3]), byte.Parse(parts[2]), byte.Parse(parts[1]));
    var classification = parts[0];
    KeyCombination.SoilTable[color] = classification;
}

foreach (var line in File.ReadAllLines("classifications/vegetation.csv")[1..])
{
    var parts = line.Split(',');
    var color = (byte.Parse(parts[3]), byte.Parse(parts[2]), byte.Parse(parts[1]));
    var classification = parts[0];
    KeyCombination.VegetationTable[color] = classification;
}

var nullVal = ((byte)0, (byte)0, (byte)0);

Dictionary<KeyCombination, List<int>> table = new();
var densityMap = new MapImage("images/density.png");

foreach (var pair in baseMap)
{
    var (x, y) = (pair.First, pair.Second);

    if (!baseMap[x, y].Equals(nullVal))
    {
        continue;
    }

    var key = keyCombinations[x, y];

    if (key.KoppenKey.Equals(nullVal))
    {
        continue;
    }

    if (!table.TryGetValue(key, out var value))
    {
        value = [];
        table[key] = value;
    }

    var densityColor = densityMap[x, y];

    value.Add(densityColor.Item1 + densityColor.Item2 + densityColor.Item3);
}

var output = new List<string> { "Koppen,Soil,Vegetation,Count,Average,Deviation" };
foreach (var (key, value) in table)
{
    var average = value.Average();
    var deviation = Math.Sqrt(value.Average(v => Math.Pow(v - average, 2)));
    output.Add($"{key},{value.Count},{average},{deviation}");
}

File.WriteAllLines("output.csv", output);