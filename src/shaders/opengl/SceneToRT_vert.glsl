layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;

out vec2 uv;

void main() {
  gl_Position = vec4(Position.x, Position.y, 0.0, 1.0);
  uv = UV;
}