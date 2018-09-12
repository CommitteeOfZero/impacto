in vec3 normal;
out vec4 color;
void main() {
  vec3 direction = normalize(vec3(-1.0, -1.0, 1.0));
  float diffuse = max(dot(normal, direction), 0.0);
  color = vec4(0.3, 0.3, 0.3, 1.0) + diffuse * vec4(0.8, 0.8, 0.8, 1.0);
}