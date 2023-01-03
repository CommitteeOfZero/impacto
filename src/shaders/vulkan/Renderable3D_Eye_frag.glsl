#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 color;

// IrisColorMap[0] - IrisColorMap, IrisColorMap[1] - WhiteColorMap, IrisColorMap[2] - HighlightColorMap, IrisColorMap[3] - IrisSpecularColorMap
layout(binding = 3) uniform sampler2D IrisColorMap[4];

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

const float FALLOFF_POWER = 0.8;
const float HIGHLIGHT_TINT_INTENSITY = 0.6;

void main() {
  vec4 eyeWhiteColor = texture(IrisColorMap[1], uv);
  vec4 irisColor = texture(IrisColorMap[0], uv);
//#if DASH == 0
//  vec4 irisSpecularColor = texture(IrisColorMap[3], uv);
//#endif
  vec4 highlightColor = texture(IrisColorMap[2], uv);

  vec3 eyeColor = mix(eyeWhiteColor.rgb, irisColor.rgb, irisColor.a);

//#if DASH == 0
  // "Specular"
//  eyeColor += irisSpecularColor.rgb * 0.1;
//#endif

  // Tint
  vec3 baseTintColor = eyeColor * Tint.rgb;
  eyeColor = mix(eyeColor, baseTintColor, Tint.a);

  // Highlight
  vec3 highlightTintColor = highlightColor.rgb * Tint.rgb;
  highlightColor.rgb = mix(highlightColor.rgb, highlightTintColor,
                           HIGHLIGHT_TINT_INTENSITY * Tint.a);
  eyeColor = mix(eyeColor, highlightColor.rgb, highlightColor.a);

  color = vec4(eyeColor, ModelOpacity);
}