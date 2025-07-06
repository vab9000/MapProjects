#include "image.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

image::image() : width_(0), height_(0) {
}

image::image(const std::string &path) : width_(0), height_(0) {
    uint8_t* arr = stbi_load(path.c_str(), &width_, &height_, nullptr, 3);
    if (arr == nullptr) {
        throw std::runtime_error("Failed to load image: " + path);
    }
    data_ = std::vector(arr, arr + (width_ * height_ * 3));
}

image& image::operator=(image &&other) noexcept {
    data_ = std::move(other.data_);
    width_ = other.width_;
    height_ = other.height_;
    return *this;
}

[[nodiscard]] uint_fast32_t image::color(const uint_fast32_t i, const uint_fast32_t j) const {
    uint_fast32_t pixel = 0;
    pixel += data_[(i + j * width_) * 3];
    pixel += data_[(i + j * width_) * 3 + 1] << 8;
    pixel += data_[(i + j * width_) * 3 + 2] << 16;

    return pixel;
}

int image::width() const {
    return width_;
}

int image::height() const {
    return height_;
}
