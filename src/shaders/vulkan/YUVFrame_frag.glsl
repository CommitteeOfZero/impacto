#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 tint;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D YUVFrame[3];

layout(push_constant) uniform constants
{
    bool IsAlpha;
} PushConstants;

const mat4 yuv_to_rgb_rec601 = mat4(
    1.16438,  0.00000,  1.59603, -0.87079,
    1.16438, -0.39176, -0.81297,  0.52959,
    1.16438,  2.01723,  0.00000, -1.08139,
    0, 0, 0, 1
);

vec4 getRGBA(vec2 texUv) {
    vec4 yuvcolor = vec4(1.0);
    yuvcolor.r = texture(YUVFrame[0], texUv).r;
    yuvcolor.g = texture(YUVFrame[1], texUv).r;
    yuvcolor.b = texture(YUVFrame[2], texUv).r;
    return yuvcolor * yuv_to_rgb_rec601;
}

void main() {
    if (PushConstants.IsAlpha) {
        color = getRGBA(vec2(uv.x, uv.y / 2.0));
        color.a = getRGBA(vec2(uv.x, uv.y / 2.0 + 0.5)).r;
    } else {
        color = getRGBA(uv);
    }
	color *= tint;
}