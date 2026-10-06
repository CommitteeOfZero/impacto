$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
uniform vec4 u_colorShift; // vec3

void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord0) * v_color0;
    gl_FragColor.rgb = saturate(gl_FragColor.rgb + u_colorShift.rgb);
}
