#pragma once

#include <string>

class image {
    unsigned char *data_;
    int width_;
    int height_;

public:
    image();

    explicit image(const std::string &path);

    ~image();

    image &operator=(image &&other) noexcept;

    image &operator=(const image &other) = delete;

    [[nodiscard]] unsigned int color(unsigned int i, unsigned int j) const;

    [[nodiscard]] int width() const;

    [[nodiscard]] int height() const;
};
