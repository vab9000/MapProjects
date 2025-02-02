#ifndef IMAGE_HPP
#define IMAGE_HPP

class Image {
public:
    unsigned char *data;
    int width;
    int height;
    int channels;

    [[nodiscard]] unsigned int getColor(const unsigned int i, const unsigned int j) const {
        return (static_cast<int>(data[(i + j * width) * 3]) << 16) + (
                   static_cast<int>(data[(i + j * width) * 3 + 1]) << 8) + static_cast<int>(data[
                   (i + j * width) * 3 + 2]);
    }
};

#endif //IMAGE_HPP
