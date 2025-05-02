#pragma once

#include <opencv2/opencv.hpp>

using pixel_t = cv::Point3_<uint8_t>;

class image {
    cv::Mat cv_image_;
    int width_;
    int height_;

public:
    image();

    explicit image(const std::string &path);

    [[nodiscard]] unsigned int get_color(unsigned int i, unsigned int j) const;

    [[nodiscard]] int get_width() const;

    [[nodiscard]] int get_height() const;
};
