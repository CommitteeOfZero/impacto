#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 tint;
layout(location = 2) in vec2 maskUV;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D ColorMap[2];

layout(push_constant) uniform constants
{
	vec2 Alpha;
	bool IsInverted;
} PushConstants;

float rgbToLightness(vec3 rgb) {
    float maxVal = max(max(rgb.r, rgb.g), rgb.b);
    float minVal = min(min(rgb.r, rgb.g), rgb.b);
    
    return (maxVal + minVal) / 2.0;
}

void main() { 
	color = texture(ColorMap[0], uv);
	vec4 alp = texture(ColorMap[1], maskUV);
	float maskAlpha = rgbToLightness(alp.rgb);
	if (PushConstants.IsInverted) maskAlpha = 1.0f - maskAlpha;
	maskAlpha *= PushConstants.Alpha.x;
	maskAlpha -= PushConstants.Alpha.y;
	maskAlpha = clamp(maskAlpha,0.0,1.0);
	color *= tint;
	color.a *= maskAlpha;
}