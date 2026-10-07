$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

#include "util.sh"

SAMPLER2D(s_font, 0);
SAMPLER2D(s_mask, 1);
uniform vec4 u_hasMask;     // bool
uniform vec4 u_strength;    // vec3(u_differenceFactor, u_intensityShift, u_alphaShift)

float getIntensity(float texValue, float valueShift, float difference) {
    float intensity = saturate(0.5f + (texValue - valueShift) / (2.0f * difference * u_strength.x));
    return (3.0f - 2.0f * intensity) * pow(intensity, 2.0f);
}

void main() {
    float fontValue = texture2D(s_font, v_texcoord0).r;
    float maskValue = toBool(u_hasMask) ? texture2D(s_mask, v_texcoord1).r : 1.0f;
    if (fontValue <= 0.0f || maskValue <= 0.0f) discard;

    ivec2 intTexCoord = ivec2(v_texcoord0 * vec2(textureSize(s_font, 0)));
    float horizontalVal = texelFetchOffset(s_font, intTexCoord, 0, ivec2(1, 0)).r;
    float verticalVal = texelFetchOffset(s_font, intTexCoord, 0, ivec2(0, 1)).r;
    float difference = abs(fontValue - horizontalVal) + abs(fontValue - verticalVal);

    gl_FragColor.rgb = vec3_splat(getIntensity(fontValue, u_strength.y, difference)) * v_color0.rgb;
    gl_FragColor.a = getIntensity(fontValue, u_strength.z, difference) * v_color0.a * maskValue;
}
