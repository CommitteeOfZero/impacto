in vec2 uv;
#if DASH
in vec2 uvNoise;
#endif

out vec4 color;

uniform sampler2D ColorMap;
#if DASH
uniform sampler2D NoiseMap;
#endif

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
};

void main() {
  vec4 texColor = texture(ColorMap, uv);
  if (texColor.a <= 0.1) discard;

#if DASH
  // These vary by scene - also seen: 0.75 and 1275, 0.58824 and 1.81818
  vec3 factor1 = vec3(0.70588);
  vec3 factor2 = vec3(8.0);

  vec3 c1 =
      factor1 * texColor.rgb + clamp(texColor.rgb * factor2, 0.0, 1.0) * 0.25;
  // 0.4, 0.2, 0.2 is from mesh info unk9
  vec3 c2 = clamp(c1 + vec3(0.4, 0.2, 0.2) + vec3(-1.0), 0.0, 1.0);
  vec3 c3 = -c1 + c2;

  float noiseFactor = texture(NoiseMap, uvNoise).r;

  color = vec4(c3 * noiseFactor + c1, 1.0);
#else
  const vec4 outlineColor = vec4(0.46484375, 0.27734375, 0.22265625, 1.0);
  color = outlineColor * texColor;
#endif

  color.a *= ModelOpacity;
}