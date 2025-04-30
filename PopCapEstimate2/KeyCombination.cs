namespace PopCapEstimate2;

public readonly struct KeyCombination(string koppenKey, string soilKey, string vegetationKey) : IEquatable<KeyCombination>
{
    private readonly string _koppenKey = koppenKey;
    private readonly string _soilKey = soilKey;
    private readonly string _vegetationKey = vegetationKey;

    public override int GetHashCode()
    {
        return HashCode.Combine(_koppenKey, _soilKey, _vegetationKey);
    }

    public override bool Equals(object? obj)
    {
        if (obj is not KeyCombination other)
        {
            return false;
        }

        return _koppenKey.Equals(other._koppenKey) && _soilKey.Equals(other._soilKey) &&
               _vegetationKey.Equals(other._vegetationKey);
    }
    
    public override string ToString()
    {
        return $"{_koppenKey},{_soilKey},{_vegetationKey}";
    }

    public bool Equals(KeyCombination other)
    {
        return _koppenKey == other._koppenKey && _soilKey == other._soilKey && _vegetationKey == other._vegetationKey;
    }

    public static bool operator ==(KeyCombination left, KeyCombination right)
    {
        return left.Equals(right);
    }

    public static bool operator !=(KeyCombination left, KeyCombination right)
    {
        return !(left == right);
    }
}