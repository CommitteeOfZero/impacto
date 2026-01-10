in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;
uniform vec2 TextureDimensions;
uniform bool IsHorizontal;

const int WEIGHTS_COUNT = 4;

// Taken from Vita
float WEIGHTS[WEIGHTS_COUNT] = float[](
    0.3992 / 2.0,
    0.2421,
    0.05402,
    0.004433
);

/*
    To prevent banding on higher resolutions, we sample each pixel over the requested distance,
    instead of merely at four equidistant points. We then lerp the weights respecting the blur
    distance the original game expects on a 480x255 texture, but on a texture of TextureDimensions
    instead.
*/
float interpolateWeight(float progress) {
    int lowerWeightIdx = min(int(floor(progress)), WEIGHTS_COUNT - 2);
    int upperWeightIdx = lowerWeightIdx + 1;

    float weightProgress = clamp(progress - lowerWeightIdx, 0.0, 1.0);
    return mix(WEIGHTS[lowerWeightIdx], WEIGHTS[upperWeightIdx], weightProgress);
}

void main() {
    // Divide by TextureDimensions to normalize sprite dimensions to [0.0, 1.0]
    vec2 normalizedOffset = (IsHorizontal ? vec2(1.0, 0.0) : vec2(0.0, 1.0)) / TextureDimensions;

    // Vita's blur effect operates on a 480 x 255 texture
    float weightDistance = IsHorizontal ? TextureDimensions.x / 480.0 : TextureDimensions.y / 255.0;
    int maxDistance = int(ceil(weightDistance * WEIGHTS_COUNT));

    color = vec4(0.0);
    float totalWeight = 0.0;
    for (int i = 0; i < maxDistance; i++) {
        float weight = interpolateWeight(float(i) / weightDistance);
        totalWeight += weight * 2.0;

        color += weight * texture2D(ColorMap, uv + normalizedOffset * i);
        color += weight * texture2D(ColorMap, uv - normalizedOffset * i);
    }
    color /= totalWeight;

    color.a = 1.0;
    color *= tint;
}
