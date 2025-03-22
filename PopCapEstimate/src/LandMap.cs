namespace PopCapEstimate;

public class LandMap : Map
{
    private static LandMap? _instance;
    
    public static LandMap GetInstance()
    {
        return _instance ??= new LandMap();
    }
    
    private LandMap() : base("assets/images/base.png")
    {
    }
}