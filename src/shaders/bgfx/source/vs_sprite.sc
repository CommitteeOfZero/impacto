$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_color0

#include <bgfx_shader.sh>

void main() {
    gl_Position = mul(u_proj, vec4(a_position, 0.0f, 1.0f));

    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
}
