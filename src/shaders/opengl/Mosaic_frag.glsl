in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;
uniform vec2 TextureDimensions;
uniform float TileSize;

void main() {
    color = tint;

    if (TileSize <= 1.0) {
        color *= texture2D(ColorMap, uv);
        return;
    }

    vec2 tileUvSize = TileSize / TextureDimensions;
    vec2 tileCoord = floor(uv / tileUvSize);
    vec2 tileCenterPos = (tileCoord + vec2(0.5)) * tileUvSize;

    color *= texture2D(ColorMap, tileCenterPos);
}
