#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;
layout(location = 2) in vec2 maskUV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D ColorMap[2];

layout(push_constant) uniform constants
{
	vec4 Alpha;
} PushConstants;

float remapMiddle(float value, float newMiddle) {
    if (value < 0.5) {
        return mix(0.0, newMiddle, 2.0 * value);
    } else {
        return mix(newMiddle, 1.0, 2.0 * value - 1.0);
    }
}

void main() {
    vec4 color = texture(ColorMap[0], uv);
    vec4 mask = texture(ColorMap[1], maskUV);
    outColor = vec4(
        remapMiddle(color.r, mask.r),
        remapMiddle(color.g, mask.g),
        remapMiddle(color.b, mask.b),
        mask.a * PushConstants.Alpha.r
    );
}