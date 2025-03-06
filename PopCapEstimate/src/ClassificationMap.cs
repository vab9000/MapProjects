using System.Windows.Media;

namespace PopCapEstimate;

public abstract class ClassificationMap(string path) : Map(path)
{
    public readonly Dictionary<Color, string> Classification = new();
    
    public string GetClassification(int x, int y)
    {
        var color = GetColor(x, y);
        return Classification[Color.FromArgb(255, color.Item2, color.Item1, color.Item0)];
    }
}
