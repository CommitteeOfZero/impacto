uniform float Alpha;

flat in int ucolorMap;
in vec2 uv;
flat in int umask;
in vec2 maskUV;

out vec4 outColor;

uniform sampler2D[15] Textures;

float remapMiddle(float value, float newMiddle) {
    if (value < 0.5) {
        return mix(0.0, newMiddle, 2.0 * value);
    } else {
        return mix(newMiddle, 1.0, 2.0 * value - 1.0);
    }
}

void main() {
    vec4 color = texture(Textures[colorMap], uv);
    vec4 maskColor = texture(Textures[mask], maskUV);
    outColor = vec4(
        remapMiddle(color.r, maskColor.r),
        remapMiddle(color.g, maskColor.g),
        remapMiddle(color.b, maskColor.b),
        maskColor.a * Alpha
    );
}