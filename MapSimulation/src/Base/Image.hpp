#pragma once

#include <opencv2/opencv.hpp>

using pixel_t = cv::Point3_<uint8_t>;

class image {
public:
    int width{};
    int height{};
    cv::Mat cv_image;

    image() = default;

    explicit image(const std::string &path) {
        cv_image = imread(path, cv::IMREAD_COLOR);
        width = cv_image.cols;
        height = cv_image.rows;
    }

    [[nodiscard]] unsigned int get_color(const unsigned int i, const unsigned int j) const {
        const auto pixel = cv_image.at<pixel_t>(static_cast<int>(j), static_cast<int>(i));
        return pixel.x + (pixel.y << 8) + (pixel.z << 16);
    }
};
