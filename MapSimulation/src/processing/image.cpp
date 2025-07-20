#include "image.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

namespace processing {
    image::image() : width_(0), height_(0) {}

    image::image(const std::string &path) {
        int width, height;
        uint8_t *arr = stbi_load(path.c_str(), &width, &height, nullptr, 3);
        width_ = width;
        height_ = height;
        if (arr == nullptr) { throw std::runtime_error("Failed to load image: " + path); }
        data_ = std::vector(arr, arr + 3ULL * width_ * height_);
    }

    [[nodiscard]] auto image::color(const uint_fast32_t i, const uint_fast32_t j) const -> uint_fast32_t {
        uint_fast32_t pixel = 0;
        pixel += data_[3ULL * (i + j * width_)];
        pixel += data_[3ULL * (i + j * width_) + 1] << 8;
        pixel += data_[3ULL * (i + j * width_) + 2] << 16;

        return pixel;
    }

    auto image::width() const -> uint_fast32_t { return width_; }

    auto image::height() const -> uint_fast32_t { return height_; }
}
