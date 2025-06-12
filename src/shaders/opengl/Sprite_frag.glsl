flat in uint colorMap;
in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D[15] Textures;

void main() { color = tint * texture(Textures[colorMap], uv); }