#include "image.hpp"

image::image() : cv_image_(cv::Mat()), width_(0), height_(0) {
}

image::image(const std::string &path) : cv_image_(cv::imread(path)), width_(cv_image_.cols),
                                          height_(cv_image_.rows) {
}

[[nodiscard]] unsigned int image::get_color(const unsigned int i, const unsigned int j) const {
    const auto pixel = cv_image_.at<pixel_t>(static_cast<int>(j), static_cast<int>(i));

    return static_cast<unsigned int>(pixel.x) << 16 | static_cast<unsigned int>(pixel.y) << 8 |
           static_cast<unsigned int>(pixel.z);
}

int image::get_width() const {
    return width_;
}

int image::get_height() const {
    return height_;
}
