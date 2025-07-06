#pragma once

#include <string>
#include <vector>

class image {
    std::vector<uint8_t> data_;
    int width_;
    int height_;

public:
    image();

    explicit image(const std::string &path);

    image &operator=(image &&other) noexcept = default;

    image &operator=(const image &other) = delete;

    // Get the color at a specified coordinate
    [[nodiscard]] uint_fast32_t color(uint_fast32_t i, uint_fast32_t j) const;

    // Get the width
    [[nodiscard]] int width() const;

    // Get the height
    [[nodiscard]] int height() const;
};
