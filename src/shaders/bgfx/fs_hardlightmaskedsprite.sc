$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);

void main() {
    vec4 spriteColor = texture2D(s_texture, v_texcoord0);
    vec4 maskColor = texture2D(s_mask, v_texcoord1);

    gl_FragColor.rgb = mix(
        spriteColor.rgb,
        mix(
            2.0f * spriteColor.rgb * maskColor.rgb,
            vec3_splat(1.0f) - 2.0f *
                (vec3_splat(1.0f) - spriteColor.rgb) *
                (vec3_splat(1.0f) - maskColor.rgb),
            vec3(greaterThan(maskColor.rgb, vec3_splat(0.5f)))
        ),
        maskColor.a
    );

    gl_FragColor.a = spriteColor.a * v_color0.a;
}
