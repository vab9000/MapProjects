#ifndef MAPEDITOR_EDITABLE_TEXTURE_HPP
#define MAPEDITOR_EDITABLE_TEXTURE_HPP
#include <array>
#include <cstddef>
#include <print>
#include <span>
#include <vector>
#include "basic_texture.hpp"

namespace graphics {
    template<basic_texture_type Type>
    class editable_texture : public basic_texture<Type> {
    public:
        using value_type = basic_texture<Type>::value_type;
        static constexpr size_t values_per_pixel = basic_texture<Type>::values_per_pixel;
        static constexpr GLint internal_format = basic_texture<Type>::internal_format;
        static constexpr GLenum format = basic_texture<Type>::format;
        static constexpr GLenum type = basic_texture<Type>::type;

        editable_texture(std::span<const value_type> data, const std::array<GLsizei, 2> dims) : basic_texture<
            Type>(data, dims), data_(data.begin(), data.end()), width_{dims.at(0)} {}

        editable_texture(const editable_texture &other) = delete;

        editable_texture(editable_texture &&other) noexcept = default;

        auto operator=(const editable_texture &other) -> editable_texture & = delete;

        auto operator=(editable_texture &&other) noexcept -> editable_texture & = default;

        auto at(const std::array<GLsizei, 2> position) const -> std::span<const value_type> {
            const auto pixel_index = (position.at(1) * width_ + position.at(0)) * values_per_pixel;
            return std::span<const value_type>(data_.data() + pixel_index, values_per_pixel);
        }

        auto edit(std::span<const value_type> data, std::array<GLsizei, 2> position, int width) -> void {
            const auto height = static_cast<int>(data.size() / values_per_pixel / width);

            glTextureSubImage2D(basic_texture<Type>::id(), 0, position.at(0), position.at(1), width,
                height, format, type, data.data());

            for (size_t i = 0; i < data.size(); ++i) {
                const auto pixel_index = (position.at(1) * width + position.at(0)) * values_per_pixel + i;
                if (pixel_index < data_.size()) { data_.at(pixel_index) = data[i]; }
            }
        }

        auto data() const -> const std::vector<value_type> & { return data_; }

        [[nodiscard]] auto dims() const -> std::array<GLsizei, 2> {
            return {width_, static_cast<GLsizei>(data_.size() / values_per_pixel / width_)};
        }

        ~editable_texture() = default;

    private:
        std::vector<value_type> data_;
        GLsizei width_;
    };
} // namespace graphics

#endif
