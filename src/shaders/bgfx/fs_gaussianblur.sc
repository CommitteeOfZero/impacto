$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

#include "util.sh"

SAMPLER2D(s_texture, 0);
uniform vec4 u_isHorizontal;    // bool

// Taken from Vita
#define WEIGHTS_COUNT 4u
const ARRAY_BEGIN(float, WEIGHTS, WEIGHTS_COUNT)
    0.3992f / 2.0f,
    0.2421f,
    0.05402f,
    0.004433f
ARRAY_END();

/*
    To prevent banding on higher resolutions, we sample each pixel over the requested distance,
    instead of merely at four equidistant points. We then mix the weights respecting the blur
    distance the original game expects on a 480x255 texture, but on a texture of TextureDimensions
    instead.
*/
float interpolateWeight(float progress) {
    uint lowerWeightIdx = min(uint(floor(progress)), WEIGHTS_COUNT - 2u);
    uint upperWeightIdx = lowerWeightIdx + 1u;

    float weightProgress = saturate(progress - float(lowerWeightIdx));
    return mix(WEIGHTS[lowerWeightIdx], WEIGHTS[upperWeightIdx], weightProgress);
}

void main() {
    ivec2 textureDimensions = textureSize(s_texture, 0);
    vec2 normalizedOffset =
        (toBool(u_isHorizontal) ? vec2(1.0f, 0.0f) : vec2(0.0f, 1.0f)) /
        vec2(textureDimensions);

    // Vita's blur effect operates on a 480 x 255 texture
    float weightDistance = toBool(u_isHorizontal) ?
        float(textureDimensions.x) / 480.0f :
        float(textureDimensions.y) / 255.0f;
    uint maxDistance = uint(ceil(weightDistance * float(WEIGHTS_COUNT)));

    gl_FragColor = vec4_splat(0.0f);
    float totalWeight = 0.0f;
    for (uint i = 0u; i < maxDistance; i++) {
        float weight = interpolateWeight(float(i) / weightDistance);
        totalWeight += weight * 2.0f;

        gl_FragColor += weight * texture2D(s_texture, v_texcoord0 + normalizedOffset * float(i));
        gl_FragColor += weight * texture2D(s_texture, v_texcoord0 - normalizedOffset * float(i));
    }
    gl_FragColor /= totalWeight;

    gl_FragColor.a = 1.0f;
    gl_FragColor *= v_color0;
}
