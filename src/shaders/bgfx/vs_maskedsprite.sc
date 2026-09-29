$input a_position, a_texcoord0, a_color0, a_texcoord1
$output v_texcoord0, v_color0, v_texcoord1

uniform mat4 u_maskTransformation; // mat4

#include <bgfx_shader.sh>

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 0.0f, 1.0f));
    v_texcoord1 = mul(u_maskTransformation, vec4(a_texcoord1, 0.0f, 1.0f)).xy;

    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
}
