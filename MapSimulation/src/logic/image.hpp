#pragma once

#include <string>

class image {
    uint8_t *data_;
    int width_;
    int height_;

public:
    image();

    explicit image(const std::string &path);

    ~image();

    image &operator=(image &&other) noexcept;

    image &operator=(const image &other) = delete;

    [[nodiscard]] uint_fast32_t color(uint_fast32_t i, uint_fast32_t j) const;

    [[nodiscard]] int width() const;

    [[nodiscard]] int height() const;
};
