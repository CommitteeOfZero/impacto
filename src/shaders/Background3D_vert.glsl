layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV;

out vec2 uv;

uniform mat4 ViewProjection;
uniform mat4 Model;

void main() {
  gl_Position = ViewProjection * Model * vec4(Position, 1.0);
  uv = UV;
}