$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);

void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord0);
    gl_FragColor.rgb = vec3_splat(1.0f) - gl_FragColor.rgb;

    gl_FragColor *= v_color0;
}
