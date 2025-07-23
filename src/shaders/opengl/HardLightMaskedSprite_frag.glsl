in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;

void main() {
    vec4 spriteColor = texture(ColorMap, uv);
    vec4 maskColor = texture(Mask, maskUV);

    // color.r = (maskColor.r < 0.5) ?
    //     2.0 * spriteColor.r * maskColor.r :
    //     1.0 - 2.0 * (1.0 - spriteColor.r) * (1.0 - maskColor.r);
    color.rgb = mix(
        2.0 * spriteColor.rgb *  maskColor.rgb,
        vec3(1.0) - 2.0 * (vec3(1.0) - spriteColor.rgb) * (vec3(1.0) - maskColor.rgb),
        vec3(maskColor.r >= 0.5, maskColor.g >= 0.5, maskColor.b >= 0.5)
    );

    color.rgb = mix(spriteColor.rgb, color.rgb, maskColor.a);
    color.a = spriteColor.a * tint.a;
}
