layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Tint;

out vec2 uv;
out vec4 tint;

uniform mat4 Projection;
uniform mat4 Transformation;

void main() {
  gl_Position = Projection * Transformation * vec4(Position, 0.0, 1.0);

  uv = UV;
  tint = Tint;
}