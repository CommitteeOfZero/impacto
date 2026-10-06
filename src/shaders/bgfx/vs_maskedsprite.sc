$input a_position, a_texcoord0, a_color0, a_texcoord1
$output v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

#include "util.sh"

uniform mat4 u_maskTransformation;  // mat4
uniform vec4 u_fullscreenMask;      // bool

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 0.0f, 1.0f));

    if (toBool(u_fullscreenMask)) {
        v_texcoord1 = (gl_Position.xy + vec2_splat(1.0f)) / 2.0f;
    } else {
        v_texcoord1 = mul(u_maskTransformation, vec4(a_texcoord1, 0.0f, 1.0f)).xy;
    }

    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
}
