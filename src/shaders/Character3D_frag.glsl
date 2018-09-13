in vec3 normal;
in vec2 uv;
out vec4 color;

uniform sampler2D colorMap;

uniform float Alpha;

void main() {
  // vec3 direction = normalize(vec3(-1.0, -1.0, 1.0));
  // float diffuse = max(dot(normal, direction), 0.0);
  color = texture(colorMap, uv);
  color.a *= Alpha;
}