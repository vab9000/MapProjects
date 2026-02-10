#ifndef MAPEDITOR_TEXTURE_BASE_HPP
#define MAPEDITOR_TEXTURE_BASE_HPP
#include <glad/glad.h>

namespace graphics {
    class texture_base {
    public:
        explicit texture_base(GLuint tex_id);

        texture_base(texture_base &&other) noexcept;

        texture_base(const texture_base &other) = delete;

        auto operator=(texture_base &&other) noexcept -> texture_base &;

        auto operator=(const texture_base &other) -> texture_base & = delete;

        [[nodiscard]] auto id() const -> GLuint;

        ~texture_base();

    protected:
        auto id_ref() -> GLuint &;

    private:
        GLuint id_{};
    };
} // namespace graphics

#endif
