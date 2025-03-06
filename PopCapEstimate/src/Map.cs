using OpenCvSharp;

namespace PopCapEstimate;

public abstract class Map(string path) : IDisposable
{
    public readonly Mat Image = Cv2.ImRead(path);

    protected Vec3b GetColor(int x, int y) => Image.At<Vec3b>(y, x);

    public void Dispose()
    {
        GC.SuppressFinalize(this);
        Image.Dispose();
    }
}
