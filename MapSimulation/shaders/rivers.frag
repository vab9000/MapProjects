uniform sampler2D texture;

void main() {
    vec4 center = texture2D(texture, gl_TexCoord[0].xy);

    if (center.r != 0 || center.g != 0) {
        gl_FragColor = gl_Color * vec4(0.0, 0.0, 0.0, 0.0);
    }
    else {
        gl_FragColor = gl_Color * vec4(0.0, 0.0, 1.0, 0.8 * center.b);
    }
}