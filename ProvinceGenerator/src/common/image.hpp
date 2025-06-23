#pragma once

#include <string>

class image {
    std::string filename_;
    unsigned char *data_;
    int width_;
    int height_;

public:
    class image_color {
        unsigned char *data_;

    public:
        explicit image_color(unsigned char *data);

        explicit operator unsigned int() const;

        [[nodiscard]] unsigned char &r() const;

        [[nodiscard]] unsigned char &g() const;

        [[nodiscard]] unsigned char &b() const;

        image_color &operator=(unsigned int color);
    };

    image(std::string filename, int width, int height);

    explicit image(std::string filename);

    ~image();

    image &operator=(const image &other) = delete;

    image &operator=(image &&other) noexcept;

    image_color operator()(int x, int y) const;

    void write() const;

    [[nodiscard]] int width() const;

    [[nodiscard]] int height() const;
};
