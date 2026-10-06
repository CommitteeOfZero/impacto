$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

#include "util.sh"

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);
uniform vec4 u_alpha;         // vec2
uniform vec4 u_isInverted;    // bool
uniform vec4 u_isSameTexture; // bool

void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord0);
    float maskAlpha = texture2D(s_mask, v_texcoord1).a;

    if (toBool(u_isInverted)) maskAlpha = 1.0f - maskAlpha;
    maskAlpha *= u_alpha.x;
    maskAlpha -= u_alpha.y;
    maskAlpha = saturate(maskAlpha);

    if (toBool(u_isSameTexture)) {
        gl_FragColor.a = maskAlpha;
    } else {
        gl_FragColor.a *= maskAlpha;
    }

    gl_FragColor *= v_color0;
}
