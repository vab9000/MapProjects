#include "paletted_texture.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <glad/glad.h>
#include "editable_texture.hpp"
#include "image.hpp"

namespace graphics {
    paletted_texture::paletted_texture(const std::span<const unsigned int> indices, const int width,
        const std::span<const unsigned char> palette) : editable_texture(indices, {width, static_cast<int>(indices.size() / width)}),
        palette_(image({palette_size, palette_size})) {

        palette_.edit(palette.subspan(0, palette.size() / palette_size * palette_size), {0, 0}, palette_size);
        const auto last_row = palette.subspan(palette.size() / palette_size * palette_size);
        palette_.edit(last_row, {0, static_cast<int>(palette.size() / palette_size / 4)},
            static_cast<int>(last_row.size() / 4));
    }

    paletted_texture::paletted_texture(paletted_texture &&other) noexcept = default;

    auto paletted_texture::operator=(paletted_texture &&other) noexcept -> paletted_texture & = default;

    paletted_texture::~paletted_texture() = default;

    auto paletted_texture::color(const unsigned int index) const -> std::array<unsigned char, 4> {
        const auto color = palette_.at({
            static_cast<int>(index % palette_size), static_cast<int>(index / palette_size)
        });
        return {color[0], color[1], color[2], color[3]};
    }

    auto paletted_texture::index(const std::array<GLsizei, 2> position) const -> unsigned int {
        const auto color = at(position);
        return color[0];
    }

    auto paletted_texture::set_color(const unsigned int index, std::array<unsigned char, 4> color) -> void {
        palette_.edit(color, image::dimensions{
            static_cast<int>(index % palette_size), static_cast<int>(index / palette_size)
        }, 1);
    }

    auto paletted_texture::palette() const -> const image_texture & { return palette_; }
} // namespace graphics
