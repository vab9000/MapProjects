#include "image_texture.hpp"
#include "editable_texture.hpp"
#include "image.hpp"

namespace graphics {
    image_texture::image_texture(const image &img) : editable_texture(img.data(), img.dims()) {}
} // namespace graphics
