namespace PopCapEstimate2;

public struct Pair<T1, T2>(T1 first, T2 second)
{
    public T1 First { get; set; } = first;
    public T2 Second { get; set; } = second;
}