in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;

float softLight(float spriteVal, float maskVal) {
    float spriteFactor = spriteVal - spriteVal * spriteVal;
    float maskFactor = 2.0 * maskVal - 1.0;

    float val = spriteVal;
    if (maskVal < 0.5) {
        val += spriteFactor * maskFactor;
    } else if (maskVal <= 32.0 / 255.0) {
        val += spriteFactor * maskFactor * (3.0 - 8.0 * spriteVal);
    } else {
        val += (sqrt(spriteVal) - spriteVal) * maskFactor;
    }

    return val;
}

void main() {
    vec4 spriteColor = texture(ColorMap, uv);
    vec4 maskColor = texture(Mask, maskUV);

    color.r = softLight(spriteColor.r, maskColor.r);
    color.g = softLight(spriteColor.g, maskColor.g);
    color.b = softLight(spriteColor.b, maskColor.b);

    color.rgb = mix(spriteColor.rgb, color.rgb, maskColor.a);
    color.a = spriteColor.a * tint.a;
}
