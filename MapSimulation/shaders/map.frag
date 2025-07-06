uniform sampler2D texture;
uniform vec2 size;

void main() {
    vec4 center = texture2D(texture, gl_TexCoord[0].xy);

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            if (dx != 0 && dy != 0) continue;
            vec2 offset = vec2(dx, dy) / size;
            vec2 new_pos = gl_TexCoord[0].xy + offset;
            if (new_pos.x < 0.0 || new_pos.y < 0.0 || new_pos.x >= size.x || new_pos.y >= size.y) continue;
            vec4 neighbor = texture2D(texture, gl_TexCoord[0].xy + offset);
            vec3 c = floor(center.rgb * 255.0 + 0.5);
            vec3 n = floor(neighbor.rgb * 255.0 + 0.5);
            if (any(notEqual(c, n))) {
                gl_FragColor = gl_Color * vec4(0.0, 0.0, 0.0, 1.0);
                return;
            }
        }
    }

    gl_FragColor = gl_Color * center;
}