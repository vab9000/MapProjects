#version 460 core

uniform sampler2D tex;
uniform vec2 size;
uniform sampler2D province_colors;
uniform int selected_index;
uniform bool draw_outline;
uniform float dim;

in vec2 tex_coord;
in vec4 color;

layout (location = 0) out vec4 frag_color;

int index_from_pixel(vec4 pixel) {
    return int(floor((pixel.r * 255.0 * 256.0 * 256.0 * 256.0 + pixel.g * 255.0 * 256.0 * 256.0 + pixel.b * 255.0 * 256.0 + pixel.a * 255.0)));
}

vec4 color_for_province(vec4 pixel) {
    int index = index_from_pixel(pixel);
    float index_float = mod(float(index), dim) / (dim - 1);
    vec4 province_color = texture(province_colors, vec2(index_float, floor(float(index) / dim) / (dim - 1)));
    return province_color;
}

void main() {
    vec4 center = texture(tex, tex_coord.xy);
    int index = index_from_pixel(center);

    vec4 province_color = color_for_province(center);

    bool outline = false;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            if (dx != 0 && dy != 0) continue;
            vec2 offset = vec2(dx, dy) / size / 2.0;
            vec2 new_pos = tex_coord.xy + offset;
            if (new_pos.x < 0.0) {
                new_pos.x = 1.0;
            }
            else if (new_pos.x > 1.0) {
                new_pos.x = 0.0;
            }
            if (new_pos.y < 0.0 || new_pos.y >= 1.0) {
                outline = true;
                break;
            }
            vec4 neighbor = texture(tex, new_pos);
            vec4 neighbor_color = color_for_province(neighbor);

            if (index == selected_index) {
                if (center != neighbor) {
                    outline = true;
                    break;
                }
            }
            else if (province_color != neighbor_color) {
                outline = true;
                break;
            }
        }
        if (outline) break;
    }

    if (index == selected_index && int(tex_coord.x * size.x + tex_coord.y * size.y) % 2 == 0) {
        frag_color = color * province_color.brga;
        return;
    }
    if (draw_outline && outline) {
        frag_color = color * vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    frag_color = color * province_color;
}
