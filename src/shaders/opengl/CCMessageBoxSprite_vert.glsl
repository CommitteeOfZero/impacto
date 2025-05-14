layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Tint;
layout(location = 3) in vec2 MaskUV;

out vec2 uv;
out vec4 tint;
out vec2 maskUV;

uniform mat4 Projection;

void main() {
  gl_Position = Projection * vec4(Position, 0.0, 1.0);
  uv = UV;
  tint = Tint;
  maskUV = MaskUV;
}