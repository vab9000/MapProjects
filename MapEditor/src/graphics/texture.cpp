#include "texture.hpp"
#include "image.hpp"

namespace graphics {
    texture::texture(const image &img) : texture_id_{} {
        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        const auto img_dims = img.dims();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img_dims.at(0), img_dims.at(1), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, img.data().data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    texture::texture(texture &&other) noexcept : texture_id_{other.texture_id_} { other.texture_id_ = 0; }

    auto texture::operator=(texture &&other) noexcept -> texture & {
        if (this == &other) { return *this; }
        glDeleteTextures(1, &texture_id_);
        texture_id_ = other.texture_id_;
        other.texture_id_ = 0;
        return *this;
    }

    texture::~texture() {
        glDeleteTextures(1, &texture_id_);
    }

    auto texture::id() const -> GLuint { return texture_id_; }
} // namespace graphics
