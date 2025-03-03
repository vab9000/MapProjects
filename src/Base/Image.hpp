#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <opencv2/opencv.hpp>

typedef cv::Point3_<uint8_t> Pixel;

class Image {
public:
	int width{};
	int height{};
	cv::Mat cvImage;

	Image() = default;

	explicit Image(const std::string &path) {
		cvImage = imread(path, cv::IMREAD_COLOR);
		width = cvImage.cols;
		height = cvImage.rows;
	}

	[[nodiscard]] unsigned int getColor(const unsigned int i, const unsigned int j) const {
		const auto pixel = cvImage.at<Pixel>(static_cast<int>(j), static_cast<int>(i));
		return pixel.x + (pixel.y << 8) + (pixel.z << 16);
	}
};

#endif // IMAGE_HPP
