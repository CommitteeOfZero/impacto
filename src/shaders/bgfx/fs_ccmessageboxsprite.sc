$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);
uniform vec4 u_alpha;   // vec3

void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord0);
    gl_FragColor *= v_color0;

    vec3 maskColor = texture2D(s_mask, v_texcoord1).rgb;

    float fa = saturate((1.0f - maskColor.b) * u_alpha.r - u_alpha.g);
    gl_FragColor.a *= fa;

    float ea = 1.0f - maskColor.g + u_alpha.b;
    if (ea > 1.0f) ea = 2.0f - ea;
    ea *= 2.0f;
    if (ea > 1.0f) ea = 2.0f - ea;

    float ga = saturate(maskColor.r * ea);
    gl_FragColor.a -= ga;

    gl_FragColor.a = saturate(gl_FragColor.a);
}
