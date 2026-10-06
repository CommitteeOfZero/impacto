$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);

void main() {
    vec4 spriteColor = texture2D(s_texture, v_texcoord0);
    vec4 maskColor = texture2D(s_mask, v_texcoord1);

    gl_FragColor.rgb = spriteColor.rgb * maskColor.rgb;
    gl_FragColor.rgb = mix(spriteColor.rgb, gl_FragColor.rgb, maskColor.a);

    gl_FragColor.a = spriteColor.a * v_color0.a;
}
