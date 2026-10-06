$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

#include "util.sh"

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);
uniform vec4 u_isInverted;  // bool

void main() {
    vec4 maskColor = texture2D(s_mask, v_texcoord1);
    vec3 invisibleColor = vec3_splat(toBool(u_isInverted) ? 1.0f : 0.0f);
    if (all(equal(maskColor.rgb, invisibleColor))) discard;

    gl_FragColor = v_color0;

    vec4 spriteColor = texture2D(s_texture, v_texcoord0);
    gl_FragColor.rgb *= spriteColor.rgb * vec3_splat(spriteColor.a);
}
