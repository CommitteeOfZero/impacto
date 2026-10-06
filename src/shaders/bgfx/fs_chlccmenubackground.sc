$input v_texcoord0, v_texcoord1

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);
uniform vec4 u_alpha;   // float

void main() {
    vec3 color = texture2D(s_texture, v_texcoord0).rgb;
    vec4 maskColor = texture2D(s_mask, v_texcoord1);

    gl_FragColor.rgb = mix(
        mix(vec3_splat(0.0f), maskColor.rgb, 2.0f * color.rgb),
        mix(maskColor.rgb, vec3_splat(1.0f), 2.0f * color.rgb - vec3_splat(1.0f)),
        vec3(greaterThan(color.rgb, vec3_splat(0.5f)))
    );
    gl_FragColor.a = maskColor.a * u_alpha.x;
}
