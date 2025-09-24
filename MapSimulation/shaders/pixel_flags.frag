#version 460

uniform sampler2D border_mask;
uniform bool draw_crossings;
uniform bool draw_bridges;
uniform vec2 tex_size;

in vec2 tex_coord;
in vec4 color;

layout (location = 0) out vec4 frag_color;

void main() {
    vec4 pixel = texture(border_mask, tex_coord);
    float border_r = pixel.r;
    int int_r = int(border_r * 255.0);
    float border_g = pixel.g;
    int int_g = int(border_g * 255.0);
    float border_b = pixel.b;
    int int_b = int(border_b * 255.0);
    float border_a = pixel.a;
    int int_a = int(border_a * 255.0);
    // Impassable crossings
    if (draw_crossings && (int_r & 0x1) != 0) {
        float dash = mod(tex_size.x * tex_coord.x + tex_size.y * tex_coord.y, 2.0);
        if (dash < 1.0) {
            frag_color = color * vec4(1.0, 0.0, 0.0, 1.0);
        } else {
            frag_color = color * vec4(1.0, 1.0, 0.0, 1.0);
        }
        return;
    }
    // Bridges
    if (draw_bridges && (int_r & 0x2) != 0) {
        frag_color = color * vec4(0.5, 0.25, 0.0, 1.0);
        return;
    }
    // Water Shading
    if ((int_b & 0x1) != 0) {
        frag_color = color * vec4(0.0, 0.0, 0.0, 0.3);
        return;
    }
    frag_color = color * vec4(0.0, 0.0, 0.0, 0.0);
}
