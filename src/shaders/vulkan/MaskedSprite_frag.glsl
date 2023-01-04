#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 tint;
layout(location = 2) in vec2 maskUV;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D ColorMap[2];

layout(push_constant) uniform constants
{
	bool IsInverted;
	bool IsSameTexture;
	vec2 Alpha;
} PushConstants;

void main() { 
	color = texture(ColorMap[0], uv);
	vec4 alp = texture(ColorMap[1], maskUV);
	if (PushConstants.IsInverted) alp.a = 1.0f - alp.a;
	alp.a *= PushConstants.Alpha.x;
	alp.a -= PushConstants.Alpha.y;
	alp.a = clamp(alp.a,0.0,1.0);
	if (PushConstants.IsSameTexture) {
		color.a = alp.a;
		color.a *= tint.a;
	} else {
		color *= tint;
		color.a *= alp.a;
	}
}