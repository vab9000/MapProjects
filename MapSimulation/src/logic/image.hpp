#pragma once

#include <string>
#include <vector>

class image {
    std::vector<uint8_t> data_;
    uint_fast32_t width_;
    uint_fast32_t height_;

public:
    image();

    explicit image(const std::string &path);

    image &operator=(image &&other) noexcept = default;

    image &operator=(const image &other) = delete;

    // Get the color at a specified coordinate
    [[nodiscard]] uint_fast32_t color(uint_fast32_t i, uint_fast32_t j) const;

    // Get the width
    [[nodiscard]] uint_fast32_t width() const;

    // Get the height
    [[nodiscard]] uint_fast32_t height() const;
};
