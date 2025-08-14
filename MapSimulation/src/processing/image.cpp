#include "image.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

namespace processing {
    image::image() : width_(0U), height_(0U) {}

    image::image(const std::string &path) {
        int width{};
        int height{};
        uint8_t *arr = stbi_load(path.c_str(), &width, &height, nullptr, 3);
        width_ = static_cast<unsigned int>(width);
        height_ = static_cast<unsigned int>(height);
        if (arr == nullptr) { throw std::runtime_error("Failed to load image: " + path); }
        data_ = std::vector(arr, arr + 3UZ * width_ * height_);
    }

    [[nodiscard]] auto image::color(const unsigned int i, const unsigned int j) const -> unsigned int {
        auto pixel = 0U;
        pixel += data_[3UZ * (i + j * width_)];
        pixel += static_cast<unsigned int>(data_[3UZ * (i + j * width_) + 1UZ]) << 8;
        pixel += static_cast<unsigned int>(data_[3UZ * (i + j * width_) + 2UZ]) << 16;

        return pixel;
    }

    auto image::width() const -> unsigned int { return width_; }

    auto image::height() const -> unsigned int { return height_; }
}
