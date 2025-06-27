in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;
uniform vec3 ColorShift;

void main() {
    color = tint * texture(ColorMap, uv);
    color.rgb = clamp(color.rgb + ColorShift, 0.0, 1.0);
}