#include "image.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include <stdexcept>
#include <stb_image/stb_image.h>

namespace processing {
    image::image() : width_(0U), height_(0U) {}

    image::image(const std::string &path) {
        int width{};
        int height{};
        unsigned char *arr = stbi_load(path.c_str(), &width, &height, nullptr, 3);
        width_ = static_cast<unsigned int>(width);
        height_ = static_cast<unsigned int>(height);
        if (arr == nullptr) { throw std::runtime_error("Failed to load image: " + path); }
        data_ = std::vector(arr, arr + 3UZ * width_ * height_);
    }

    [[nodiscard]] auto image::color(const unsigned int i, const unsigned int j) const -> unsigned int {
        auto pixel = 0U;
        pixel += static_cast<unsigned int>(data_[3UZ * (i + j * width_)]) << 16U;
        pixel += static_cast<unsigned int>(data_[3UZ * (i + j * width_) + 1UZ]) << 8U;
        pixel += static_cast<unsigned int>(data_[3UZ * (i + j * width_) + 2UZ]);

        return pixel;
    }

    auto image::width() const -> unsigned int { return width_; }

    auto image::height() const -> unsigned int { return height_; }

    auto image::data() const -> const unsigned char * { return data_.data(); }
}
