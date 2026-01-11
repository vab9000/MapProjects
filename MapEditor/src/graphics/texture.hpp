#ifndef MAPEDITOR_IMAGE_SPRITE_HPP
#define MAPEDITOR_IMAGE_SPRITE_HPP
#include <glad/glad.h>
#include "image.hpp"

namespace graphics {
    class texture {
    public:
        explicit texture(const image &img);

        texture(const texture &other) = delete;

        texture(texture &&other) noexcept;

        auto operator=(texture &&other) noexcept -> texture &;

        auto operator=(const texture &other) -> texture & = delete;

        ~texture();

        [[nodiscard]] auto id() const -> GLuint;

    private:
        GLuint texture_id_;
    };
} // namespace graphics
#endif
