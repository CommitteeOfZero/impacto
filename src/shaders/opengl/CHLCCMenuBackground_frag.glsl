uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform float Alpha;

in vec2 uv;
in vec2 maskUV;

out vec4 outColor;

float remapMiddle(float value, float newMiddle) {
    if (value < 0.5) {
        return mix(0.0, newMiddle, 2.0 * value);
    } else {
        return mix(newMiddle, 1.0, 2.0 * value - 1.0);
    }
}

void main() {
    vec4 color = texture(ColorMap, uv);
    vec4 mask = texture(Mask, maskUV);
    outColor = vec4(
        remapMiddle(color.r, mask.r),
        remapMiddle(color.g, mask.g),
        remapMiddle(color.b, mask.b),
        mask.a * Alpha
    );
}