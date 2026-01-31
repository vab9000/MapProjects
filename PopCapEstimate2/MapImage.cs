global using Pixel = (byte, byte, byte);

using System.Collections;
using StbImageSharp;

namespace PopCapEstimate2;

public class MapImage : IEnumerable<Pair<int, int>>
{
    private readonly ImageResult _image;

    public Pixel this[int x, int y]
    {
        get
        {
            var index = (_image.Width * y + x * 4);
            return (_image.Data[index], _image.Data[index + 1], _image.Data[index + 2]);
        }
        set
        {
            var index = (_image.Width * y + x * 4);
            _image.Data[index] = value.Item1;
            _image.Data[index + 1] = value.Item2;
            _image.Data[index + 2] = value.Item3;
        }
    }

    public int Width => _image.Width;

    public int Height => _image.Height;

    public MapImage(string filepath)
    {
        _image = ImageResult.FromStream(new FileStream(filepath, FileMode.Open), ColorComponents.RedGreenBlueAlpha);
    }

    public MapImage(int width, int height)
    {
        _image = ImageResult.FromMemory(new byte[width * height * 4], ColorComponents.RedGreenBlueAlpha);
    }

    public IEnumerator<Pair<int, int>> GetEnumerator()
    {
        for (var y = 0; y < _image.Height; y++)
        {
            for (var x = 0; x < _image.Width; x++)
            {
                yield return new Pair<int, int>(x, y);
            }
        }
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
        return GetEnumerator();
    }
}