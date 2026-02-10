#version 460 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform usampler2D tex;
layout (binding = 1) uniform sampler2D palette;
uniform uvec2 dim;
uniform uint selected_idx;

vec4 color_from_palette(uint index) {
    return texture(palette, vec2(float(index % int(dim.x)) / float(dim.x - 1), float(index / int(dim.x)) / float(dim.y - 1)));
}

void main() {
    uint center = texture(tex, tex_coord).r;

    vec2 dtx = vec2(abs(dFdx(tex_coord.x)), 0.0);
    vec2 dty = vec2(0.0, abs(dFdy(tex_coord.y)));

    bool edge = false;

    vec2 offsets[4] = vec2[](-dtx, dtx, -dty, dty);

    for (int i = 0; i < 4; ++i) {
        vec2 uv = tex_coord + offsets[i];

        uv = clamp(uv, vec2(0.0), vec2(0.999999));

        uint neighbor = texture(tex, uv).r;

        if (neighbor != center) {
            edge = true;
            break;
        }
    }

    vec3 color = color_from_palette(center).rgb;
    if (edge) {
        if (selected_idx == center) {
            color = vec3(1.0, 1.0, 1.0);
        }
        else {
            color = vec3(0.0);
        }
    }

    frag_color = vec4(color, 1.0);
}