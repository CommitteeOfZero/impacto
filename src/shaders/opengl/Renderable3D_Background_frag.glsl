in vec2 uv;

out vec4 color;

uniform sampler2D ColorMap;

void main() { color = texture(ColorMap, uv); }