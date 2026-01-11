#version 460 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D tex;
uniform float opacity;

void main() {
    frag_color = vec4(texture(tex, tex_coord).rgb, opacity);
}