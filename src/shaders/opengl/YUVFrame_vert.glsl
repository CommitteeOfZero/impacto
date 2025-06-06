layout(location = 0) in vec2 Position;
layout(location = 2) in vec2 UV;
layout(location = 3) in vec4 Tint;

out vec2 uv;
out vec4 tint;

uniform mat4 Projection;

void main() {
  gl_Position = Projection * vec4(Position, 0.0, 1.0);
  uv = UV;
  tint = Tint;
}