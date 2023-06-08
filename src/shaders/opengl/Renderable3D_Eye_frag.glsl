in vec2 uv;

out vec4 color;

uniform sampler2D IrisColorMap;
uniform sampler2D WhiteColorMap;
#if DASH == 0
uniform sampler2D IrisSpecularColorMap;
#endif
uniform sampler2D HighlightColorMap;

layout(std140) uniform Character3DScene {
  UNIFORM_PRECISION mat4 ViewProjection;
  UNIFORM_PRECISION vec4 Tint;
  UNIFORM_PRECISION vec3 WorldLightPosition;
  UNIFORM_PRECISION vec3 WorldEyePosition;
  bool DarkMode;
};

layout(std140) uniform Character3DModel { UNIFORM_PRECISION mat4 Model; };

layout(std140) uniform Character3DMesh {
  UNIFORM_PRECISION mat4 Bones[ModelMaxBonesPerMesh];
  UNIFORM_PRECISION float ModelOpacity;
  bool HasShadowColorMap;
};

const float FALLOFF_POWER = 0.8;
const float HIGHLIGHT_TINT_INTENSITY = 0.6;

void main() {
  vec4 eyeWhiteColor = texture(WhiteColorMap, uv);
  vec4 irisColor = texture(IrisColorMap, uv);
#if DASH == 0
  vec4 irisSpecularColor = texture(IrisSpecularColorMap, uv);
#endif
  vec4 highlightColor = texture(HighlightColorMap, uv);

  vec3 eyeColor = mix(eyeWhiteColor.rgb, irisColor.rgb, irisColor.a);

#if DASH == 0
  // "Specular"
  eyeColor += irisSpecularColor.rgb * 0.1;
#endif

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