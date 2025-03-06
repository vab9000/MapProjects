using System.Collections.Concurrent;
using System.ComponentModel;
using System.Globalization;
using System.IO;

namespace PopCapEstimate;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow
{
    private readonly BackgroundWorker _worker = new();
    
    private readonly ClimateMap _climateMap = ClimateMap.GetInstance();
    private readonly DensityMap _densityMap = DensityMap.GetInstance();
    private readonly VegetationMap _vegetationMap = VegetationMap.GetInstance();
    // private readonly LandMap _landmap = LandMap.GetInstance();

    private readonly List<List<string>> _table = [];

    public MainWindow()
    {
        InitializeComponent();

        Lst.ItemsSource = _table;
        
        _worker.RunWorkerAsync();

        UpdateData();
    }

    private void DisposeMaps()
    {
        _climateMap.Dispose();
        _densityMap.Dispose();
        _vegetationMap.Dispose();
    }

    private void UpdateData()
    {
        ConcurrentDictionary<string, Pair<int, long>> data = [];
        Dictionary<string, int> classificationCount = [];
        Dictionary<string, Dictionary<string, string>> classificationCombinations = [];

        InitClassifications(data, classificationCount, classificationCombinations);

        ProcessPixels(data, classificationCombinations);

        DisposeMaps();

        PopulateTable(data, classificationCount);

        WriteDataToCsv(data);
    }

    private void ProcessPixels(ConcurrentDictionary<string, Pair<int, long>> data, Dictionary<string, Dictionary<string, string>> classificationCombinations)
    {
        Parallel.For(0, _densityMap.Image.Width, new ParallelOptions { MaxDegreeOfParallelism = 12 }, (x) =>
        {
            for (var y = 0; y < _densityMap.Image.Height; y++)
            {
                var climate = _climateMap.GetClassification(x, y);
                var vegetation = _vegetationMap.GetClassification(x, y);

                var density = _densityMap.GetDensity(x, y);
                
                var pair = data[classificationCombinations[climate][vegetation]];

                pair.First++;
                pair.Second += density;
            }

            Console.WriteLine(x + " / " + _densityMap.Image.Width);
        });
    }

    private void InitClassifications(ConcurrentDictionary<string, Pair<int, long>> data, Dictionary<string, int> classificationCount, Dictionary<string, Dictionary<string, string>> classificationCombinations)
    {
        foreach (var climate in _climateMap.Classification.Values)
        {
            classificationCombinations.Add(climate, new Dictionary<string, string>());
            var classificationDict = classificationCombinations[climate];
            foreach (var vegetation in _vegetationMap.Classification.Values)
            {
                var classification = climate + "," + vegetation;
                data.TryAdd(classification, new Pair<int, long>(0, 0));
                _table.Add([climate, vegetation, "-1"]);
                classificationCount.Add(classification, classificationCount.Count);
                classificationDict.Add(vegetation, classification);
            }
        }
    }

    private void PopulateTable(ConcurrentDictionary<string, Pair<int, long>> data, Dictionary<string, int> classificationCount)
    {
        foreach (var (classification, count) in classificationCount)
        {
            var densityPair = data[classification];
            if (densityPair.First == 0) continue;
            var average = densityPair.Second / densityPair.First;
            _table[count][2] = average.ToString(CultureInfo.InvariantCulture);
        }
    }

    private static void WriteDataToCsv(ConcurrentDictionary<string, Pair<int, long>> data)
    {
        using var writer = new StreamWriter("output/climate-vegetation-density.csv");
        writer.WriteLine("Climate,Vegetation,Density,Total");
        foreach (var (key, value) in data)
        {
            var splitKey = key.Split(",");
            var average = value.First == 0 ? -1 : value.Second / value.First;
            writer.WriteLine($"{splitKey[0]},{splitKey[1]},{average.ToString(CultureInfo.InvariantCulture)},{value.First}");
        }
    }
    
    private class Pair<T1, T2>(T1 first, T2 second)
    {
        public T1 First = first;
        public T2 Second = second;
    }
}