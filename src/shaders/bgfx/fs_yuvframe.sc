$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

#include "util.sh"

SAMPLER2D(s_luma, 0);
SAMPLER2D(s_cb, 1);
SAMPLER2D(s_cr, 2);

uniform vec4 u_isAlpha; // bool

vec4 getRgba(vec2 texUv) {
    const mat4 yuv_to_rgb_rec601 = mtxFromVals(
        1.16438f,  0.00000f,  1.59603f, -0.87079f,
        1.16438f, -0.39176f, -0.81297f,  0.52959f,
        1.16438f,  2.01723f,  0.00000f, -1.08139f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return mul(
        yuv_to_rgb_rec601,
        vec4(
            texture2D(s_luma, texUv).r,
            texture2D(s_cb, texUv).r,
            texture2D(s_cr, texUv).r,
            1.0f
        )
    );
}

void main() {
    if (toBool(u_isAlpha)) {
        gl_FragColor = getRgba(vec2(v_texcoord0.x, v_texcoord0.y / 2.0));
        gl_FragColor.a = getRgba(vec2(v_texcoord0.x, v_texcoord0.y / 2.0 + 0.5)).r;
    } else {
        gl_FragColor = getRgba(v_texcoord0);
    }

    gl_FragColor *= v_color0;
}
