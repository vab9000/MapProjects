#ifndef MAPEDITOR_IMAGE_HPP
#define MAPEDITOR_IMAGE_HPP
#include <array>
#include <span>
#include <vector>
#include <zstring_view.hpp>

namespace graphics {
    class image {
    public:
        using dimensions = std::array<int, 2>;

    private:
        std::vector<unsigned char> data_;
        dimensions dims_;

    public:
        explicit image(dimensions dims);

        explicit image(zstring_view path);

        explicit image(std::span<const unsigned char> data, int width);

        image();

        image(image &&other) noexcept;

        image(const image &other);

        ~image() = default;

        auto operator =(image &&other) noexcept -> image &;

        auto operator =(const image &other) -> image &;

        [[nodiscard]] auto at(dimensions pixel) const -> std::span<const unsigned char, 4>;

        auto at(dimensions pixel) -> std::span<unsigned char, 4>;

        [[nodiscard]] auto data() const -> std::span<const unsigned char>;

        [[nodiscard]] auto dims() const -> dimensions;

        auto edit(std::span<const unsigned char> data, dimensions position, int width) -> void;
    };
} // namespace graphics
#endif
