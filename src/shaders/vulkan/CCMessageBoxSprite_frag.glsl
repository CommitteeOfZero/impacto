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
	vec4 CCBoxAlpha;
} PushConstants;

void main() { 
	color = texture(ColorMap[0], uv);
	color *= tint;
	vec4 alp = texture(ColorMap[1], maskUV);
	float ga = alp.r;
	float ea = 1.0 - alp.g;
	float fa = 1.0 - alp.b;
	fa *= PushConstants.CCBoxAlpha.r;
	fa -= PushConstants.CCBoxAlpha.g;
	fa = clamp(fa,0.0,1.0);
	color.a *= fa;

	ea += PushConstants.CCBoxAlpha.b;
	if (ea > 1.0) ea = 2.0 - ea;
	ea *= 2.0;
	if (ea > 1.0) ea = 2.0 - ea;

	ga *= ea;
	ga = clamp(ga, 0.0, 1.0);
	color.a -= ga;
	color.a = clamp(color.a,0.0,1.0);
}