layout(location = 0) in vec2 Position;
layout(location = 1) in uint ColorMap;
layout(location = 2) in vec2 UV;
layout(location = 3) in vec4 Tint;
layout(location = 4) in uint Mask;
layout(location = 5) in vec2 MaskUV;

flat out uint colorMap;
out vec2 uv;
out vec4 tint;
flat out uint mask;
out vec2 maskUV;

uniform mat4 Projection;
uniform mat4 SpriteTransformation;
uniform mat4 MaskTransformation;

void main() {
  gl_Position = Projection * SpriteTransformation * vec4(Position, 0.0, 1.0);

  colorMap = ColorMap;
  uv = UV;
  tint = Tint;
  mask = Mask;
  maskUV = vec2(MaskTransformation * vec4(MaskUV, 0.0, 1.0));
}