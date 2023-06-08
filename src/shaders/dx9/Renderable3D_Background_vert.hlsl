float4x4 MVP : register(c0);

struct VS_INPUT {
  float3 Position : POSITION;
  float2 UV : TEXCOORD0;
};

struct VS_OUTPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
  VS_OUTPUT output;

  output.position = mul(MVP, float4(input.Position, 1.0));
  output.uv = input.UV;

  return output;
}
