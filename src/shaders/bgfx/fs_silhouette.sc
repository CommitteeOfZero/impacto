$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_coverageMap, 0);

void main() {
    float mask = texture2D(s_coverageMap, v_texcoord0).r;
    float alpha = v_color0.a * mask;

    gl_FragColor = vec4(v_color0.rgb * alpha, alpha);
}
