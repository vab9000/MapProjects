#ifndef MAPEDITOR_BASIC_TEXTURE_HPP
#define MAPEDITOR_BASIC_TEXTURE_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <glad/glad.h>
#include "texture_base.hpp"

namespace graphics {
    enum class basic_texture_type {
        rgba8, mono_uint,
    };

    template<basic_texture_type Type>
    class basic_texture : public texture_base {
    public:
        using value_type = std::conditional_t<Type == basic_texture_type::rgba8, uint8_t, uint32_t>;
        static constexpr size_t values_per_pixel = Type == basic_texture_type::rgba8 ? 4 : 1;
        static constexpr GLint internal_format = Type == basic_texture_type::rgba8 ? GL_RGBA8 : GL_R32UI;
        static constexpr GLenum format = Type == basic_texture_type::rgba8 ? GL_RGBA : GL_RED_INTEGER;
        static constexpr GLenum type = Type == basic_texture_type::rgba8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_INT;

        basic_texture(std::span<const value_type> data, const std::array<GLsizei, 2> dims) : texture_base{0} {
            if (data.size() != values_per_pixel * dims.at(0) * dims.at(1)) {
                throw std::invalid_argument("Data size does not match dimensions.");
            }

            glGenTextures(1, &id_ref());
            glBindTexture(GL_TEXTURE_2D, id());

            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, dims.at(0), dims.at(1), 0,
                format, type, data.data());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        basic_texture(basic_texture &&other) noexcept = default;

        basic_texture(const basic_texture &other) = delete;

        auto operator=(basic_texture &&other) noexcept -> basic_texture & = default;

        auto operator=(const basic_texture &other) -> basic_texture & = delete;

        auto edit(std::span<const value_type> data, const std::array<GLsizei, 2> position, GLsizei width) -> void {
            const auto height = static_cast<int>(data.size() / width / 4);

            glTextureSubImage2D(id(), 0, position.at(0), position.at(1), width,
                height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        }

        ~basic_texture() {
            glDeleteTextures(1, &id_ref());
        }
    };
} // namespace graphics

#endif
