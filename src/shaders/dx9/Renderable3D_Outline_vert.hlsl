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

struct VS_INPUT {
  float3 Position : POSITION;
  float3 Normal : NORMAL;
  float2 UV : TEXCOORD0;
  float4 BoneIndices : BLENDWEIGHT;
  float4 BoneWeights : BLENDINDICES;
};

struct VS_OUTPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float3 uvNoise : TEXCOORD1;
};

// TODO there's a uniform for this somewhere...
#if defined(IsDaSH)
static const float OutlineThickness = 0.0035;
#else
static const float OutlineThickness = 0.035;
#endif

VS_OUTPUT main(VS_INPUT input) {
  VS_OUTPUT output;

  // Accumulated skinning, thanks
  // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch04.html
  float4x4 skeletalTransform =
      mul(Bones[input.BoneIndices.x], input.BoneWeights.x) +
      mul(Bones[input.BoneIndices.y], input.BoneWeights.y) +
      mul(Bones[input.BoneIndices.z], input.BoneWeights.z) +
      mul(Bones[input.BoneIndices.w], input.BoneWeights.w);

  float4x4 transform = mul(Model, skeletalTransform);
  float3x3 normalMatrix = (float3x3)transform;

  float3 worldNormal = mul(normalMatrix, input.Normal);

  float4 viewNormal = normalize(mul(ViewProjection, float4(worldNormal, 0.0)));

  float4 worldPosition = mul(transform, float4(input.Position, 1.0));

  output.position = mul(ViewProjection, worldPosition);
  output.position += viewNormal * OutlineThickness;

  output.uv = input.UV;

#if defined(IsDaSH)
  output.uvNoise = ((output.position.xy / output.position.w) * 0.2).xyy;
#else
  output.uvNoise = (0.0).xxx;
#endif

  return output;
}
