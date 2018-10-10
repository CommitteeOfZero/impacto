layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Tint;

out vec2 uv;
out vec2 tint;

void main() {
  gl_Position = vec4(Position.x, Position.y, 0.0, 1.0);
  uv = UV;
  tint = Tint;
}