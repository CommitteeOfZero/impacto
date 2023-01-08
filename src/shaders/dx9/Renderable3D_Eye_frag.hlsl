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
};

static const float FALLOFF_POWER = 0.8;
static const float HIGHLIGHT_TINT_INTENSITY = 0.6;

sampler IrisColorMap : register(s0);
sampler WhiteColorMap : register(s1);
sampler HighlightColorMap : register(s2);
sampler IrisSpecularColorMap : register(s3);

float4 main(PS_INPUT input) : COLOR {
  float4 eyeWhiteColor = tex2D(WhiteColorMap, input.uv);
  float4 irisColor = tex2D(IrisColorMap, input.uv);
  float4 highlightColor = tex2D(HighlightColorMap, input.uv);

  float3 eyeColor = lerp(eyeWhiteColor.rgb, irisColor.rgb, irisColor.a);

#if !defined(IsDaSH)
  // "Specular"
  float4 irisSpecularColor = tex2D(IrisSpecularColorMap, input.uv);
  eyeColor += irisSpecularColor.rgb * 0.1;
#endif

  // Tint
  float3 baseTintColor = eyeColor * Tint.rgb;
  eyeColor = lerp(eyeColor, baseTintColor, Tint.a);

  // Highlight
  float3 highlightTintColor = highlightColor.rgb * Tint.rgb;
  highlightColor.rgb = lerp(highlightColor.rgb, highlightTintColor,
                            HIGHLIGHT_TINT_INTENSITY * Tint.a);
  eyeColor = lerp(eyeColor, highlightColor.rgb, highlightColor.a);

  return float4(eyeColor, ModelOpacity);
}
