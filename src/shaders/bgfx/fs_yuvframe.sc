$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_luma, 0);
SAMPLER2D(s_cb, 1);
SAMPLER2D(s_cr, 2);

uniform vec4 u_isAlpha;

vec4 getRgba(vec2 texUv) {
    mat4 yuv_to_rgb_rec601 = mtxFromRows(
        vec4(1.16438,  0.00000,  1.59603, -0.87079),
        vec4(1.16438, -0.39176, -0.81297,  0.52959),
        vec4(1.16438,  2.01723,  0.00000, -1.08139),
        vec4(0, 0, 0, 1)
    );

    return mul(
        yuv_to_rgb_rec601,
        vec4(
            texture2D(s_luma, texUv).r,
            texture2D(s_cb, texUv).r,
            texture2D(s_cr, texUv).r,
            1.0
        )
    );
}

void main() {
    if (u_isAlpha.x != 0.0) {
        gl_FragColor = getRgba(vec2(v_texcoord0.x, v_texcoord0.y / 2.0));
        gl_FragColor.a = getRgba(vec2(v_texcoord0.x, v_texcoord0.y / 2.0 + 0.5)).r;
    } else {
        gl_FragColor = getRgba(v_texcoord0);
    }

    gl_FragColor *= v_color0;
}
