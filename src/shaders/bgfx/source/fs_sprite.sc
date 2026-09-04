$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
uniform vec3 u_colorShift;

void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord0) * v_color0;
    gl_FragColor.rgb = clamp(gl_FragColor.rgb + u_colorShift, 0.0f, 1.0f);
}
