#ifndef MAPEDITOR_EDITABLE_TEXTURE_HPP
#define MAPEDITOR_EDITABLE_TEXTURE_HPP
#include <span>
#include "image.hpp"
#include "texture.hpp"

namespace graphics {
    class editable_texture : public texture {
    public:
        explicit editable_texture(image &&img);

        editable_texture(const editable_texture &other) = delete;

        editable_texture(editable_texture &&other) noexcept;

        auto operator=(const editable_texture &other) -> editable_texture & = delete;

        auto operator=(editable_texture &&other) noexcept -> editable_texture &;

        ~editable_texture();

        auto edit(std::span<const unsigned char> data, image::dimensions position, int width) -> void;

        [[nodiscard]] auto img() const -> const image &;

    private:
        image img_;
    };
} // namespace graphics

#endif
