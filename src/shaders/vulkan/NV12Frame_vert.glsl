#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Tint;
layout(location = 3) in vec2 MaskUV;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec4 tint;

void main() {
  gl_Position = vec4(Position, 0.0, 1.0);
  uv = UV;
  tint = Tint;
}
