#ifndef MAPEDITOR_TEXTURE_HPP
#define MAPEDITOR_TEXTURE_HPP
#include "basic_texture.hpp"
#include "editable_texture.hpp"
#include "image.hpp"

namespace graphics {
    class image_texture : public editable_texture<basic_texture_type::rgba8> {
    public:
        explicit image_texture(const image &img);
    };
} // namespace graphics
#endif
