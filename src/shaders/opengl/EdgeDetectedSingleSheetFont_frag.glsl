in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D Font;
uniform sampler2D Mask;
uniform bool HasMask;
uniform vec2 PixelOffset;
uniform float IntensityShift;
uniform float AlphaShift;

float getIntensity(float texValue, float valueShift, float difference) {
    float intensity = clamp(0.5 + (texValue - valueShift) / (2.0 * difference), 0.0, 1.0);
    return (3.0 - 2.0 * intensity) * pow(intensity, 2.0);
}

void main() {
    float fontValue = texture(Font, uv).r;
    float maskValue = HasMask ? texture(Mask, maskUV).r : 1.0;
    if (fontValue <= 0.0 || maskValue <= 0.0) {
        color = vec4(0.0);
        return;
    }

    float horizontalVal = texture(Font, uv + vec2(PixelOffset.x, 0.0)).r;
    float verticalVal = texture(Font, uv + vec2(0.0, PixelOffset.y)).r;
    float difference = abs(fontValue - horizontalVal) + abs(fontValue - verticalVal);

    color.rgb = vec3(getIntensity(fontValue, IntensityShift, difference)) * tint.rgb;
    color.a = getIntensity(fontValue, AlphaShift, difference) * tint.a * maskValue;
}
