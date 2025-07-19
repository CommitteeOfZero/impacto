in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;

void main() {
    vec4 spriteColor = texture(ColorMap, uv);
    vec4 maskColor = texture(Mask, maskUV);

    // color.r = (maskColor.r == 0.0) ? 1.0 :
    //     clamp(1.0 - (1.0 - spriteColor.r) / maskColor.r, 0.0, 1.0),
    color.rgb = mix(
        clamp(vec3(1.0) - (vec3(1.0) - spriteColor.rgb) / maskColor.rgb, 0.0, 1.0),
        vec3(1.0),
        vec3(maskColor.r <= 0.0, maskColor.g <= 0.0, maskColor.b <= 0.0)
    );

    color.rgb = mix(spriteColor.rgb, color.rgb, maskColor.a);
    color.a = spriteColor.a * tint.a;
}
