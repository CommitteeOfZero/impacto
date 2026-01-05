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

void main() {
    // Divide by TextureDimensions to normalize sprite dimensions to [0.0, 1.0]
    vec2 normalizedOffset = (IsHorizontal ? vec2(1.0, 0.0) : vec2(0.0, 1.0)) / TextureDimensions;

    // Vita's blur effect operates on a 480 x 255 texture
    float pixelSize = IsHorizontal ? TextureDimensions.x / 480.0 : TextureDimensions.y / 255.0;
    int weightDistance = max(int(round(sqrt(pixelSize))), 1);
    int distance = weightDistance * WEIGHTS_COUNT;

    /*
        To prevent banding on higher resolutions, we sample a discrete uniform distribution
        across the size that a single 480x255-texture pixel would cover on TextureDimensions,
        effectively sampling the same constant weights at a different resolution
    */
    float uniformNormalizer = 1.0 / weightDistance;

    color = vec4(0.0);
    for (int i = 0; i < distance; i++) {
        int weightIdx = i / weightDistance;
        float weight = WEIGHTS[weightIdx] * uniformNormalizer;

        color += weight * texture2D(ColorMap, uv + normalizedOffset * i);
        color += weight * texture2D(ColorMap, uv - normalizedOffset * i);
    }

    color.a = 1.0;
    color *= tint;
}