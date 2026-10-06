$input v_texcoord0, v_color0, v_texcoord1

#include <bgfx_shader.sh>

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_mask, 1);

float softLight(float spriteVal, float maskVal) {
    float spriteFactor = spriteVal - spriteVal * spriteVal;
    float maskFactor = 2.0f * maskVal - 1.0f;

    float val = spriteVal;
    if (maskVal < 0.5f) {
        val += spriteFactor * maskFactor;
    } else {
        val += (sqrt(spriteVal) - spriteVal) * maskFactor;
    }

    return val;
}

void main() {
    vec4 spriteColor = texture2D(s_texture, v_texcoord0);
    vec4 maskColor = texture2D(s_mask, v_texcoord1);

    gl_FragColor = vec4(
        softLight(spriteColor.r, maskColor.r),
        softLight(spriteColor.g, maskColor.g),
        softLight(spriteColor.b, maskColor.b),
        spriteColor.a * v_color0.a
    );

    gl_FragColor.rgb = mix(spriteColor.rgb, gl_FragColor.rgb, maskColor.a);
}
