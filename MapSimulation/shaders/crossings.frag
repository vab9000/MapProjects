uniform sampler2D borderMask;

void main() {
    // Impassable crossings
    float borderR = texture2D(borderMask, gl_TexCoord[0].xy).r;
    if (borderR > 0.5) {
        float dashLength = 4.0;
        float dash = mod(gl_FragCoord.x + gl_FragCoord.y, dashLength * 2.0);
        if (dash < dashLength) {
            gl_FragColor = gl_Color * vec4(1.0, 0.0, 0.0, 1.0);
        } else {
            gl_FragColor = gl_Color * vec4(1.0, 1.0, 0.0, 1.0);
        }
        return;
    }
    gl_FragColor = gl_Color * vec4(0.0, 0.0, 0.0, 0.0);
}