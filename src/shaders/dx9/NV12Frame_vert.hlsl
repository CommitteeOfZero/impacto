struct VS_INPUT {
  float2 Position : POSITION;
  float2 UV : TEXCOORD0;
  float4 Tint : COLOR;
  float2 MaskUV : TEXCOORD1;
};

struct VS_OUTPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float4 tint : COLOR;
  float2 maskUV : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input) {
  VS_OUTPUT output;

  output.position = float4(input.Position, 0.0, 1.0);
  output.uv = input.UV;
  output.tint = input.Tint;
  output.maskUV = input.MaskUV;

  return output;
}
