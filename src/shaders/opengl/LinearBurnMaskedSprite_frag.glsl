in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;

void main() {
    vec4 spriteColor = texture(ColorMap, uv);
    vec4 maskColor = texture(Mask, maskUV);

    color.rgb = clamp(spriteColor.rgb + maskColor.rgb - vec3(1.0), 0.0, 1.0);

    color.rgb = mix(spriteColor.rgb, color.rgb, maskColor.a);
    color.a = spriteColor.a * tint.a;
}
