#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 color;

layout(binding = 3) uniform sampler2D ColorMap;

void main() { color = texture(ColorMap, uv); }
