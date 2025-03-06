namespace PopCapEstimate;

public class DensityMap : Map
{
    private static DensityMap? _instance;
    
    public static DensityMap GetInstance()
    {
        return _instance ??= new DensityMap();
    }
    
    private DensityMap() : base("assets/images/density.png")
    {
    }
    
    public int GetDensity(int x, int y)
    {
        var color = GetColor(x, y);
        return color.Item0 + color.Item1 + color.Item2;
    }
}