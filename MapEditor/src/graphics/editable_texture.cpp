#include "editable_texture.hpp"
#include <span>
#include <utility>
#include "image.hpp"
#include "texture.hpp"

namespace graphics {
    editable_texture::editable_texture(image &&img) : texture(img), img_(std::move(img)) {}

    editable_texture::editable_texture(editable_texture &&other) noexcept = default;

    auto editable_texture::operator=(editable_texture &&other) noexcept -> editable_texture & {
        if (this == &other) { return *this; }
        img_ = std::move(other.img_);
        texture::operator=(std::move(other));
        return *this;
    }

    editable_texture::~editable_texture() = default;

    auto editable_texture::edit(const std::span<const unsigned char> data, const image::dimensions position,
        const int width) -> void {
        const auto height = static_cast<int>(data.size() / width / 4);

        glTextureSubImage2D(id(), 0, position.at(0), position.at(1), width,
            height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

        img_.edit(data, position, width);
    }

    auto editable_texture::img() const -> const image & { return img_; }
} // namespace graphics
