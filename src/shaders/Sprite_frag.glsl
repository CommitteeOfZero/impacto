in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;

void main() { color = tint * texture(ColorMap, uv); }