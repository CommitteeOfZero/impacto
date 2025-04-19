in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;
uniform vec4 ColorShift;

void main() {
    color = texture(ColorMap, uv) * tint;
    float alpha = color.w;
    color = clamp(color + ColorShift, 0.0, 1.0);
    color.w = alpha;
}