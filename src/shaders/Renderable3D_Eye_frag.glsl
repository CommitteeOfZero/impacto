in vec2 uv;

out vec4 color;

uniform sampler2D IrisColorMap;
uniform sampler2D WhiteColorMap;
uniform sampler2D IrisSpecularColorMap;
uniform sampler2D HighlightColorMap;

layout(std140) uniform Character3DCommon {
  mat4 ViewProjection;
  mat4 Model;
  mat4 Bones[ModelMaxBonesPerMesh];
  vec4 Tint;
  vec3 WorldLightPosition;
  vec3 WorldEyePosition;
  float ModelOpacity;
};

const float FALLOFF_POWER = 0.8;
const float HIGHLIGHT_TINT_INTENSITY = 0.6;

void main() {
  vec4 eyeWhiteColor = texture(WhiteColorMap, uv);
  vec4 irisColor = texture(IrisColorMap, uv);
  vec4 irisSpecularColor = texture(IrisSpecularColorMap, uv);
  vec4 highlightColor = texture(HighlightColorMap, uv);

  vec3 eyeColor = mix(eyeWhiteColor.rgb, irisColor.rgb, irisColor.a);

  // "Specular"
  eyeColor += irisSpecularColor.rgb * 0.1;

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