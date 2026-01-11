#ifndef MAPEDITOR_PALETTED_TEXTURE_HPP
#define MAPEDITOR_PALETTED_TEXTURE_HPP
#include <array>
#include <span>
#include "editable_texture.hpp"
#include "image.hpp"

namespace graphics {
    class paletted_texture : public editable_texture {
    public:
        static constexpr int palette_size = 16 * 256;

        paletted_texture(std::span<const unsigned int> indices, int width, std::span<const unsigned char> palette);

        paletted_texture(paletted_texture &&other) noexcept;

        paletted_texture(const paletted_texture &other) = delete;

        auto operator=(paletted_texture &&other) noexcept -> paletted_texture &;

        auto operator=(const paletted_texture &other) -> paletted_texture & = delete;

        ~paletted_texture();

        [[nodiscard]] auto color(unsigned int index) const -> std::array<unsigned char, 4>;

        [[nodiscard]] auto index(image::dimensions position) const -> unsigned int;

        auto set_color(unsigned int index, std::array<unsigned char, 4> color) -> void;

        auto edit(std::span<const unsigned int> indices, image::dimensions position, int width) -> void;

        [[nodiscard]] auto palette() const -> const editable_texture &;

    private:
        editable_texture palette_;
    };
} // namespace graphics

#endif
