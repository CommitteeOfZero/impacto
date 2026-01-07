in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform bool IsInverted;

void main() {
    color = tint;

    vec4 textureColor = texture(ColorMap, uv);
    color.rgb *= textureColor.rgb * vec3(textureColor.a);

    vec4 maskColor = texture(Mask, maskUV);
    vec3 invisibleColor = vec3(IsInverted ? 1.0 : 0.0);
    if (maskColor.rgb == invisibleColor) color.a = 0.0;
}
