in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D CoverageMap;

void main() {
    float mask = texture(CoverageMap, uv).r;
    color = tint * mask;
    color.rgb = clamp(color.rgb, 0.0, 1.0);
}