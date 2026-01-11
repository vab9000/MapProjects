#version 460 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D palette;
uniform uvec2 dim;

int index_from_pixel(vec4 pixel) {
    return int(floor((pixel.r * 255.0 * 256.0 * 256.0 * 256.0 + pixel.g * 255.0 * 256.0 * 256.0 + pixel.b * 255.0 * 256.0 + pixel.a * 255.0)));
}

vec4 color_from_palette(vec4 pixel) {
    int index = index_from_pixel(pixel);
    return texture(palette, vec2(float(index % int(dim.x)) / float(dim.x - 1), float(index / int(dim.x)) / float(dim.y - 1)));
}

void main() {
    frag_color = vec4(color_from_palette(texture(tex, tex_coord.xy)).rgb, 1.0);
}