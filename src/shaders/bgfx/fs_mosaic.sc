$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
uniform vec4 u_tileSize;    // float

void main() {
    gl_FragColor = v_color0;

    if (u_tileSize.x <= 1.0f) {
        gl_FragColor *= texture2D(s_texture, v_texcoord0);
        return;
    }

    vec2 tileUvSize = u_tileSize.x / vec2(textureSize(s_texture, 0));
    vec2 tileCoord = floor(v_texcoord0 / tileUvSize);
    vec2 tileCenterPos = (tileCoord + vec2_splat(0.5f)) * tileUvSize;

    gl_FragColor *= texture2D(s_texture, tileCenterPos);
}
