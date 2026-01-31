namespace PopCapEstimate2;

public struct KeyCombination() : IEquatable<KeyCombination>
{
    public static Dictionary<Pixel, string> KoppenTable { get; } = new();
    public static Dictionary<Pixel, string> SoilTable { get; } = new();
    public static Dictionary<Pixel, string> VegetationTable { get; } = new();

    public Pixel? KoppenKey = null;
    public Pixel? SoilKey = null;
    public Pixel? VegetationKey = null;

    public override int GetHashCode()
    {
        return HashCode.Combine(KoppenKey, SoilKey, VegetationKey);
    }

    public override string ToString()
    {
        return
            $"{KoppenTable[KoppenKey.GetValueOrDefault()]},{SoilTable[SoilKey.GetValueOrDefault()]},{VegetationTable[VegetationKey.GetValueOrDefault()]}";
    }

    public bool Equals(KeyCombination other)
    {
        return KoppenKey == other.KoppenKey && SoilKey == other.SoilKey && VegetationKey == other.VegetationKey;
    }
}