in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D CoverageMap;

void main() {
    float mask = texture(CoverageMap, uv).r;
    float alpha = tint.a * mask;
    color = vec4(tint.rgb * alpha, alpha);
}