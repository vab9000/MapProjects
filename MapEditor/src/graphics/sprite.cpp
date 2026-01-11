#include "sprite.hpp"
#include <array>
#include <memory>
#include <utility>
#include <zstring_view.hpp>
#include "image.hpp"
#include "paletted_texture.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace graphics {
    sprite::sprite(std::shared_ptr<texture> &&texture, std::shared_ptr<shader> &&shader, const space space,
        const dimensions window_dims) : vao_{}, vbo_{}, ebo_{},
        window_dims_{window_dims}, space_{space}, shader_{std::move(shader)}, texture_{std::move(texture)} {
        const std::array bounds = {
            static_cast<float>(space_.position.at(0)) / static_cast<float>(window_dims.at(0)) * 2.0F - 1.0F,
            1.0F - static_cast<float>(space_.position.at(1)) / static_cast<float>(window_dims.at(1)) * 2.0F,
            static_cast<float>(space_.position.at(0) + space_.size.at(0)) / static_cast<float>(window_dims.at(0)) * 2.0F
            -
            1.0F,
            1.0F - static_cast<float>(space_.position.at(1) + space_.size.at(1)) / static_cast<float>(window_dims.at(1))
            *
            2.0F,
        };

        const std::array vertices = {
            bounds.at(0), bounds.at(1), 0.0F, 0.0F,
            bounds.at(2), bounds.at(1), 1.0F, 0.0F,
            bounds.at(2), bounds.at(3), 1.0F, 1.0F,
            bounds.at(0), bounds.at(3), 0.0F, 1.0F
        };

        constexpr std::array<GLuint, 6> indices = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    sprite::sprite(sprite &&other) noexcept : vao_{other.vao_}, vbo_{other.vbo_}, ebo_{other.ebo_},
        window_dims_{other.window_dims_}, space_{other.space_},
        shader_{std::move(other.shader_)}, texture_{std::move(other.texture_)} {
        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
    }

    auto sprite::operator=(sprite &&other) noexcept -> sprite & {
        if (this == &other) { return *this; }

        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        window_dims_ = other.window_dims_;
        space_ = other.space_;
        shader_ = std::move(other.shader_);
        texture_ = std::move(other.texture_);
        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;

        return *this;
    }

    auto sprite::draw(const dimensions window_dims) -> void {
        if (window_dims != window_dims_) {
            window_dims_ = window_dims;
            resize_buffers();
        }
        glUseProgram(shader_->program());
        glBindTextureUnit(0, texture_->id());

        glBindVertexArray(vao_);
        constexpr auto num_vertices = 6;
        glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    auto sprite::position() const -> dimensions { return space_.position; }

    auto sprite::size() const -> dimensions { return space_.size; }

    auto sprite::set_position(const dimensions pos) -> void {
        space_.position = pos;
        resize_buffers();
    }

    auto sprite::set_size(const dimensions size) -> void {
        space_.size = size;
        resize_buffers();
    }

    sprite::~sprite() {
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
        glDeleteVertexArrays(1, &vao_);
    }

    auto sprite::resize_buffers() const -> void {
        const std::array bounds = {
            static_cast<float>(space_.position.at(0)) / static_cast<float>(window_dims_.at(0)) * 2.0F - 1.0F,
            1.0F - static_cast<float>(space_.position.at(1)) / static_cast<float>(window_dims_.at(1)) * 2.0F,
            static_cast<float>(space_.position.at(0) + space_.size.at(0)) / static_cast<float>(window_dims_.at(0)) *
            2.0F -
            1.0F,
            1.0F - static_cast<float>(space_.position.at(1) + space_.size.at(1)) / static_cast<float>(window_dims_.
                at(1)) *
            2.0F,
        };

        const std::array vertices = {
            bounds.at(0), bounds.at(1), 0.0F, 0.0F,
            bounds.at(2), bounds.at(1), 1.0F, 0.0F,
            bounds.at(2), bounds.at(3), 1.0F, 1.0F,
            bounds.at(0), bounds.at(3), 0.0F, 1.0F
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
    }

    static auto get_paletted_shader() -> std::shared_ptr<shader> {
        constexpr auto paletted_vert_path = "shaders/default.vert"_zsv;
        constexpr auto paletted_frag_path = "shaders/paletted.frag"_zsv;

        auto paletted_shader = std::make_shared<shader>(paletted_vert_path, paletted_frag_path);
        return paletted_shader;
    }

    auto sprite::create_paletted(std::shared_ptr<paletted_texture> &&tex, const space space,
        const dimensions window_dims) -> sprite {
        const std::shared_ptr paletted_texture = tex;
        auto spr = sprite{std::static_pointer_cast<texture>(std::move(tex)), get_paletted_shader(), space, window_dims};
        spr.set_uniform("dim"_zsv, std::array{
            static_cast<unsigned int>(paletted_texture::palette_size),
            static_cast<unsigned int>(paletted_texture::palette_size)
        });
        glUseProgram(spr.shader_->program());
        glBindTextureUnit(1, paletted_texture->palette().id());
        return spr;
    }

    static auto get_image_shader() -> std::shared_ptr<shader> {
        constexpr auto image_vert_path = "shaders/default.vert"_zsv;
        constexpr auto image_frag_path = "shaders/default.frag"_zsv;

        auto new_shader = std::make_shared<shader>(image_vert_path, image_frag_path);
        return new_shader;
    }

    auto sprite::create_from_image(const image &img, const space space, const dimensions window_dims) -> sprite {
        auto spr = sprite{std::make_shared<texture>(img), get_image_shader(), space, window_dims};
        spr.set_uniform("opacity"_zsv, 1.0F);
        return spr;
    }
} // namespace graphics
