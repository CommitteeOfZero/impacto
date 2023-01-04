#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;
layout(location = 1) in vec2 uvNoise;

layout(location = 0) out vec4 color;

// ColorMap[0] - ColorMap, ColorMap[1] - NoiseMap
layout(binding = 3) uniform sampler2D ColorMap[4];

layout(binding = 0) uniform Character3DScene {
  mat4 ViewProjection;
  vec4 Tint;
  vec3 WorldLightPosition;
  vec3 WorldEyePosition;
  bool DarkMode;
};

layout(binding = 1) uniform Character3DModel { mat4 Model; };

layout(binding = 2) uniform Character3DMesh {
  mat4 Bones[32];
  float ModelOpacity;
  bool HasShadowColorMap;
};

layout(push_constant) uniform constants
{
	bool IsDash;
} PushConstants;

void main() {
  vec4 texColor = texture(ColorMap[0], uv);
  if (texColor.a <= 0.1) discard;

  if (PushConstants.IsDash) {
    vec3 factor1 = vec3(0.70588);
    vec3 factor2 = vec3(8.0);

    vec3 c1 =
        factor1 * texColor.rgb + clamp(texColor.rgb * factor2, 0.0, 1.0) * 0.25;
    // 0.4, 0.2, 0.2 is from mesh info unk9
    vec3 c2 = clamp(c1 + vec3(0.4, 0.2, 0.2) + vec3(-1.0), 0.0, 1.0);
    vec3 c3 = -c1 + c2;

    float noiseFactor = texture(ColorMap[1], uvNoise).r;

    color = vec4(c3 * noiseFactor + c1, 1.0);
  } else {
    const vec4 outlineColor = vec4(0.46484375, 0.27734375, 0.22265625, 1.0);
    color = outlineColor * texColor;
  }

  color.a *= ModelOpacity;
}