#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV;

layout(location = 0) out vec2 uv;

layout(binding = 0) uniform UniformBufferObject {
    mat4 MVP;
} ubo;

layout(push_constant) uniform constants
{
	bool IsDash;
} PushConstants;

void main() {
  vec4 test = ubo.MVP * vec4(Position, 1.0);
  test.y = -test.y;
  gl_Position = test;
  uv = UV;
}