layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Tint;
layout(location = 3) in vec2 MaskUV;

out vec2 uv;
out vec4 tint;
out vec2 maskUV;

uniform mat4 Projection;
uniform mat4 SpriteTransformation;
uniform mat4 MaskTransformation;

void main() {
  gl_Position = Projection * SpriteTransformation * vec4(Position, 0.0, 1.0);

  uv = UV;
  tint = Tint;
  maskUV = vec2(MaskTransformation * vec4(MaskUV, 0.0, 1.0));
}