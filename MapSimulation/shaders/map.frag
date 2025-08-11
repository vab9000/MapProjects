uniform sampler2D texture;
uniform vec2 size;
uniform sampler2D provinceColors;
uniform int selectedIndex;
uniform bool drawOutline;
uniform float dim;

void main() {
    vec4 center = texture2D(texture, gl_TexCoord[0].xy);
    int index = int(floor((center.r * 255.0 * 256.0 * 256.0 * 256.0 + center.g * 255.0 * 256.0 * 256.0 + center.b * 255.0 * 256.0 + center.a * 255.0)));

    float indexFloat = mod(float(index), dim) / (dim - 1);
    vec4 provinceColor = texture2D(provinceColors, vec2(indexFloat, floor(index / dim) / (dim - 1)));

    bool outline = false;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            if (dx != 0 && dy != 0) continue;
            vec2 offset = vec2(dx, dy) / size / 2.0;
            vec2 new_pos = gl_TexCoord[0].xy + offset;
            if (new_pos.x < 0.0 || new_pos.y < 0.0 || new_pos.x >= size.x || new_pos.y >= size.y) {
                outline = true;
                break;
            }
            vec4 neighbor = texture2D(texture, new_pos);

            if (center != neighbor) {
                outline = true;
                break;
            }
        }
        if (outline) break;
    }

    if (outline) {
        if (index == selectedIndex) {
            gl_FragColor = gl_Color * vec4(abs(1.0 - provinceColor.rgb), 1.0);
            return;
        }
        if (drawOutline) {
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
    }

    gl_FragColor = gl_Color * provinceColor;
}
