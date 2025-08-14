#pragma once
#include <string>
#include <vector>

namespace processing {
    class image {
        std::vector<uint8_t> data_;
        unsigned int width_;
        unsigned int height_;

    public:
        image();

        explicit image(const std::string &path);

        auto operator=(image &&other) noexcept -> image & = default;

        auto operator=(const image &other) -> image & = delete;

        // Get the color at a specified coordinate
        [[nodiscard]] auto color(unsigned int i, unsigned int j) const -> unsigned int;

        // Get the width
        [[nodiscard]] auto width() const -> unsigned int;

        // Get the height
        [[nodiscard]] auto height() const -> unsigned int;
    };
}
