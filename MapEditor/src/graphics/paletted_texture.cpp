#include "paletted_texture.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <vector>
#include "editable_texture.hpp"
#include "image.hpp"

namespace graphics {
    paletted_texture::paletted_texture(const std::span<const unsigned int> indices, const int width,
        const std::span<const unsigned char> palette) : editable_texture(image(image::dimensions{
            width, static_cast<int>(indices.size() / width)
        })),
        palette_(image({palette_size, palette_size})) {
        auto index_data = std::vector<unsigned char>(indices.size() * 4);
        for (size_t i = 0; i < indices.size(); ++i) {
            const auto index = indices[i];
            index_data.at(i * 4) = static_cast<unsigned char>(index >> 24);
            index_data.at(i * 4 + 1) = static_cast<unsigned char>(index >> 16);
            index_data.at(i * 4 + 2) = static_cast<unsigned char>(index >> 8);
            index_data.at(i * 4 + 3) = static_cast<unsigned char>(index);
        }
        editable_texture::edit(index_data, {0, 0}, width);
        palette_.edit(palette.subspan(0, palette.size() / palette_size * palette_size), {0, 0}, palette_size);
        const auto last_row = palette.subspan(palette.size() / palette_size * palette_size);
        palette_.edit(last_row, {0, static_cast<int>(palette.size() / palette_size / 4)},
            static_cast<int>(last_row.size() / 4));
    }

    paletted_texture::paletted_texture(paletted_texture &&other) noexcept = default;

    auto paletted_texture::operator=(paletted_texture &&other) noexcept -> paletted_texture & = default;

    paletted_texture::~paletted_texture() = default;

    auto paletted_texture::color(const unsigned int index) const -> std::array<unsigned char, 4> {
        const auto color = palette_.img().at(image::dimensions{
            static_cast<int>(index % palette_size), static_cast<int>(index / palette_size)
        });
        return {color[0], color[1], color[2], color[3]};
    }

    auto paletted_texture::index(const image::dimensions position) const -> unsigned int {
        const auto color = img().at(position);
        return color[0] << 24 | color[1] << 16 | color[2] << 8 | color[3];
    }

    auto paletted_texture::set_color(const unsigned int index, std::array<unsigned char, 4> color) -> void {
        palette_.edit(color, image::dimensions{
            static_cast<int>(index % palette_size), static_cast<int>(index / palette_size)
        }, 1);
    }

    auto paletted_texture::edit(const std::span<const unsigned int> indices, const image::dimensions position,
        const int width) -> void {
        auto index_data = std::vector<unsigned char>(indices.size() * 4);
        for (size_t i = 0; i < indices.size(); ++i) {
            const auto index = indices[i];
            index_data.at(i * 4) = static_cast<unsigned char>(index >> 24);
            index_data.at(i * 4 + 1) = static_cast<unsigned char>(index >> 16);
            index_data.at(i * 4 + 2) = static_cast<unsigned char>(index >> 8);
            index_data.at(i * 4 + 3) = static_cast<unsigned char>(index);
        }
        editable_texture::edit(index_data, position, width);
    }

    auto paletted_texture::palette() const -> const editable_texture & { return palette_; }
} // namespace graphics
