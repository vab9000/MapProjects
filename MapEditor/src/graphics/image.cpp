#include "image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace graphics {
    image::image() : dims_{0, 0} {}

    auto image::operator=(image &&other) noexcept -> image & {
        if (this == &other) { return *this; }
        data_ = std::move(other.data_);
        dims_ = other.dims_;
        other.dims_ = {0U, 0U};
        return *this;
    }

    auto image::operator=(const image &other) -> image & = default;

    auto image::at(const dimensions pixel) const -> std::span<const unsigned char, 4> {
        return std::span<const unsigned char, 4>(&data_.at(4UZ * (pixel.at(0) + pixel.at(1) * dims_.at(0))), 4);
    }

    auto image::at(const dimensions pixel) -> std::span<unsigned char, 4> {
        return std::span<unsigned char, 4>(&data_.at(4UZ * (pixel.at(0) + pixel.at(1) * dims_.at(0))), 4);
    }

    auto image::data() const -> std::span<const unsigned char> { return {data_.data(), data_.size()}; }

    auto image::dims() const -> dimensions { return dims_; }

    auto image::edit(const std::span<const unsigned char> data, const dimensions position, const int width) -> void {
        const auto height = data.size() / width / 4;
        for (size_t row = 0; row < height; ++row) {
            std::copy_n(&data[row * width * 4], width * 4,
                &data_.at(4UZ * (position.at(0) + (position.at(1) + row) * dims_.at(0))));
        }
    }

    image::image(image &&other) noexcept : data_(std::move(other.data_)), dims_{other.dims_} { other.dims_ = {0U, 0U}; }

    image::image(const image &other) = default;

    image::image(const dimensions dims) : data_(4UZ * dims.at(0) * dims.at(1),
        std::numeric_limits<unsigned char>::max()), dims_(dims) {}

    image::image(const zstring_view path) : dims_{} {
        unsigned char *data = stbi_load(path.str(), &dims_.at(0), &dims_.at(1), nullptr, 4);
        data_.assign(data, &data[4UZ * dims_.at(0) * dims_.at(1)]);
    }

    image::image(std::span<const unsigned char> data, const int width) : data_(data.begin(), data.end()),
        dims_{width, static_cast<int>(data.size() / width / 4)} {}
} // namespace graphics
