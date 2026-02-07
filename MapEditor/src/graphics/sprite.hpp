#ifndef MAPEDITOR_SPRITE_HPP
#define MAPEDITOR_SPRITE_HPP
#include <array>
#include <memory>
#include <zstring_view.hpp>
#include <glad/glad.h>
#include "image.hpp"
#include "paletted_texture.hpp"
#include "shader.hpp"
#include "texture_base.hpp"

namespace graphics {
    class sprite {
    public:
        using dimensions = std::array<int, 2>;

        struct space {
            dimensions position;
            dimensions size;
        };

        sprite(std::shared_ptr<texture_base> &&texture, std::shared_ptr<shader> &&shader, space space,
            dimensions window_dims);

        sprite(sprite &&other) noexcept;

        sprite(const sprite &other) = delete;

        auto operator=(sprite &&other) noexcept -> sprite &;

        auto operator=(const sprite &other) -> sprite & = delete;

        auto draw(dimensions window_dims) -> void;

        [[nodiscard]] auto position() const -> dimensions;

        [[nodiscard]] auto size() const -> dimensions;

        template<typename T>
        auto set_uniform(zstring_view name, T value) -> void;

        auto set_position(dimensions pos) -> void;

        auto set_size(dimensions size) -> void;

        ~sprite();

        static auto create_paletted(std::shared_ptr<paletted_texture> &&tex, space space,
            dimensions window_dims) -> sprite;

        static auto create_from_image(const image &img, space space, dimensions window_dims) -> sprite;

    private:
        GLuint vao_;
        GLuint vbo_;
        GLuint ebo_;
        dimensions window_dims_;
        space space_;
        std::shared_ptr<shader> shader_;
        std::shared_ptr<texture_base> texture_;

        auto resize_buffers() const -> void;
    };

    template<typename T>
    auto sprite::set_uniform(const zstring_view name, const T value) -> void {
        const int location = glGetUniformLocation(shader_->program(), name.str());
        if constexpr (std::is_same_v<T, int>) { glProgramUniform1i(shader_->program(), location, value); }
        else if constexpr (std::is_same_v<T, unsigned int>) {
            glProgramUniform1ui(shader_->program(), location, value);
        }
        else if constexpr (std::is_same_v<T, float>) { glProgramUniform1f(shader_->program(), location, value); }
        else if constexpr (std::is_same_v<T, double>) { glProgramUniform1d(shader_->program(), location, value); }
        else if constexpr (std::is_same_v<T, std::array<unsigned int, 2>>) {
            glProgramUniform2ui(shader_->program(), location, value.at(0), value.at(1));
        }
        else if constexpr (std::is_same_v<T, std::array<float, 2>>) {
            glProgramUniform2f(shader_->program(), location, value.at(0), value.at(1));
        }
        else if constexpr (std::is_same_v<T, std::array<float, 3>>) {
            glProgramUniform3f(shader_->program(), location, value.at(0), value.at(1), value.at(2));
        }
        else if constexpr (std::is_same_v<T, std::array<float, 4>>) {
            glProgramUniform4f(shader_->program(), location, value.at(0), value.at(1), value.at(2), value.at(3));
        }
        else { static_assert(false, "Unsupported uniform type"); }
    }
} // namespace graphics
#endif
