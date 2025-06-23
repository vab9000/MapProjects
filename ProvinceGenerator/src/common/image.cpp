#include "image.hpp"

#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>

image::image_color::image_color(unsigned char *data) : data_(data) {
}

image::image_color::operator unsigned int() const {
    return (data_[0] << 16) | (data_[1] << 8) | data_[2];
}

unsigned char &image::image_color::r() const {
    return data_[0];
}

unsigned char &image::image_color::g() const {
    return data_[1];
}

unsigned char &image::image_color::b() const {
    return data_[2];
}

image::image_color & image::image_color::operator=(const unsigned int color) {
    r() = (color >> 16) & 0xFF;
    g() = (color >> 8) & 0xFF;
    b() = color & 0xFF;
    return *this;
}

image::image(std::string filename, const int width, const int height) : filename_(std::move(filename)),
                                                                        width_(width),
                                                                        height_(height) {
    data_ = new unsigned char[width * height * 3];
}


image::image(std::string filename) : filename_(std::move(filename)) {
    int width, height;
    int channels;
    data_ = stbi_load(filename_.c_str(), &width, &height, &channels, 3);
    if (!data_) {
        throw std::runtime_error("Failed to load image: " + filename);
    }
    width_ = width;
    height_ = height;
}

image::~image() {
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
}

image &image::operator=(image &&other) noexcept {
    filename_ = std::move(other.filename_);
    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    other.data_ = nullptr;

    return *this;
}

image::image_color image::operator()(const int x, const int y) const {
    return image_color{&data_[(y * width_ + x) * 3]};
}

void image::write() const {
    stbi_write_png(filename_.c_str(), width_, height_, 3, data_, width_ * 3);
}

int image::width() const {
    return width_;
}

int image::height() const {
    return height_;
}
