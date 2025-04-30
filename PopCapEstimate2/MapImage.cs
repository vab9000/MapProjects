using System.Collections;
using OpenCvSharp;

namespace PopCapEstimate2;

public class MapImage(string filepath) : IEnumerable<KeyValuePair<int, int>>
{
    private readonly Mat _image = Cv2.ImRead(filepath);

    public Vec3b this[int x, int y] => _image.At<Vec3b>(y, x);
    
    public IEnumerator<KeyValuePair<int, int>> GetEnumerator()
    {
        for (var y = 0; y < _image.Rows; y++)
        {
            for (var x = 0; x < _image.Cols; x++)
            {
                yield return new KeyValuePair<int, int>(x, y);
            }
        }
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
        return GetEnumerator();
    }
}