#ifndef MAPEDITOR_TEXTURE_BASE_HPP
#define MAPEDITOR_TEXTURE_BASE_HPP

namespace graphics {
    class texture_base {
    public:
        explicit texture_base(const GLuint tex_id) : id_{tex_id} {}

        texture_base(texture_base &&other) noexcept : id_{other.id_} { other.id_ = 0; }

        texture_base(const texture_base &other) = delete;

        auto operator=(texture_base &&other) noexcept -> texture_base & {
            if (this == &other) { return *this; }
            id_ = other.id_;
            other.id_ = 0;
            return *this;
        }

        auto operator=(const texture_base &other) -> texture_base & = delete;

        [[nodiscard]] auto id() const -> GLuint { return id_; }

        ~texture_base() = default;

    protected:
        auto id_ref() -> GLuint & { return id_; }

    private:
        GLuint id_{};
    };
} // namespace graphics

#endif
