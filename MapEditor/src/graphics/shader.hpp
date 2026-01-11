#ifndef MAPEDITOR_SHADER_HPP
#define MAPEDITOR_SHADER_HPP
#include <zstring_view.hpp>
#include <glad/glad.h>

namespace graphics {
    class shader {
    public:
        shader(zstring_view vertex_path, zstring_view fragment_path);

        shader(const shader &other) = delete;

        shader(shader &&other) noexcept;

        auto operator=(const shader &other) -> shader & = delete;

        auto operator=(shader &&other) noexcept -> shader &;

        ~shader();

        [[nodiscard]] auto program() const -> GLuint;

    private:
        GLuint program_;
    };
} // namespace graphics

#endif
