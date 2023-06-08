#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 tint;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D ColorMap;

void main() { color = tint * texture(ColorMap, uv); }