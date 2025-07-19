#pragma once
#include <string>
#include <vector>

namespace processing {
    class image {
        std::vector<uint8_t> data_;
        uint_fast32_t width_;
        uint_fast32_t height_;

    public:
        image();

        explicit image(const std::string &path);

        auto operator=(image &&other) noexcept -> image & = default;

        auto operator=(const image &other) -> image & = delete;

        // Get the color at a specified coordinate
        [[nodiscard]] auto color(uint_fast32_t i, uint_fast32_t j) const -> uint_fast32_t;

        // Get the width
        [[nodiscard]] auto width() const -> uint_fast32_t;

        // Get the height
        [[nodiscard]] auto height() const -> uint_fast32_t;
    };
}
