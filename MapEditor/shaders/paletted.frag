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
    ivec2 texSize = textureSize(tex, 0);

    vec4 center_px = texture(tex, tex_coord);
    int center = index_from_pixel(center_px);

    // Texture-space step equivalent to 1 screen pixel
    vec2 dtx = vec2(abs(dFdx(tex_coord.x)), 0.0);
    vec2 dty = vec2(0.0, abs(dFdy(tex_coord.y)));

    bool edge = false;

    vec2 offsets[4] = vec2[](
    -dtx,
    dtx,
    -dty,
    dty
    );

    for (int i = 0; i < 4; ++i) {
        vec2 uv = tex_coord + offsets[i];

        // Clamp to valid range
        uv = clamp(uv, vec2(0.0), vec2(0.999999));

        int neighbor = index_from_pixel(texture(tex, uv));

        if (neighbor != center) {
            edge = true;
            break;
        }
    }

    vec3 color = color_from_palette(center_px).rgb;
    if (edge)
    color = vec3(0.0);

    frag_color = vec4(color, 1.0);
}