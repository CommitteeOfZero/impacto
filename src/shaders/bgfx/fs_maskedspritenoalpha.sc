$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

#include "util.sh"

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);
uniform vec4 u_alpha;         // vec2
uniform vec4 u_isInverted;    // bool

float rgbToLightness(vec3 rgb) {
    float maxVal = max(max(rgb.r, rgb.g), rgb.b);
    float minVal = min(min(rgb.r, rgb.g), rgb.b);

    return (minVal + maxVal) / 2.0f;
}

void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord0);

    vec4 maskColor = texture2D(s_mask, v_texcoord1);
    float maskAlpha = rgbToLightness(maskColor.rgb);

    if (toBool(u_isInverted)) maskAlpha = 1.0f - maskAlpha;
    maskAlpha *= u_alpha.x;
    maskAlpha -= u_alpha.y;
    maskAlpha = saturate(maskAlpha);

    gl_FragColor.a *= maskAlpha;
    gl_FragColor *= v_color0;
}
