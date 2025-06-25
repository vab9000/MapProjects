#include "image.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

image::image() : data_(nullptr), width_(0), height_(0) {
}

image::image(const std::string &path) : data_(nullptr), width_(0), height_(0) {
    data_ = stbi_load(path.c_str(), &width_, &height_, nullptr, 3);
    if (data_ == nullptr) {
        throw std::runtime_error("Failed to load image: " + path);
    }
}

image::~image() {
    if (data_) {
        stbi_image_free(data_);
    }
}

image& image::operator=(image &&other) noexcept {
    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    other.data_ = nullptr;
    return *this;
}

[[nodiscard]] unsigned int image::color(const unsigned int i, const unsigned int j) const {
    unsigned int pixel = 0;
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
