// Character3DScene
float4x4 ViewProjection : register(c0);
float4 Tint : register(c4);
float3 WorldLightPosition : register(c5);
float3 WorldEyePosition : register(c6);
bool DarkMode : register(b0);

// Character3DModel
float4x4 Model : register(c7);

// Character3DMesh
bool HasShadowColorMap : register(b1);
float ModelOpacity : register(c11);
float4x4 Bones[32] : register(c12);

struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float2 uvNoise : TEXCOORD1;
};

sampler ColorMap : register(s0);
sampler NoiseMap : register(s1);

float4 main(PS_INPUT input) : COLOR {
  float4 color;
  float4 texColor = tex2D(ColorMap, input.uv);
  if (texColor.a <= 0.1) discard;

#if defined(IsDaSH)
  float3 factor1 = (0.70588).xxx;
  float3 factor2 = (8.0).xxx;

  float3 c1 =
      factor1 * texColor.rgb + clamp(texColor.rgb * factor2, 0.0, 1.0) * 0.25;
  // 0.4, 0.2, 0.2 is from mesh info unk9
  float3 c2 = clamp(c1 + vec3(0.4, 0.2, 0.2) + vec3(-1.0), 0.0, 1.0);
  float3 c3 = -c1 + c2;

  float noiseFactor = tex2D(NoiseMap, input.uvNoise).r;

  color = float4(c3 * noiseFactor + c1, 1.0);
#else
  static const float4 outlineColor =
      float4(0.46484375, 0.27734375, 0.22265625, 1.0);
  color = outlineColor * texColor;
#endif

  color.a *= ModelOpacity;

  return color;
}
