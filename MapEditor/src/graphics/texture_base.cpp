#include "texture_base.hpp"

namespace graphics {
    texture_base::texture_base(const GLuint tex_id) : id_{tex_id} {}

    texture_base::texture_base(texture_base &&other) noexcept : id_{other.id_} { other.id_ = 0; }

    auto texture_base::operator=(texture_base &&other) noexcept -> texture_base & {
        if (this == &other) { return *this; }
        id_ = other.id_;
        other.id_ = 0;
        return *this;
    }

    [[nodiscard]] auto texture_base::id() const -> GLuint { return id_; }

    texture_base::~texture_base() = default;

    auto texture_base::id_ref() -> GLuint & { return id_; }
} // namespace graphics
