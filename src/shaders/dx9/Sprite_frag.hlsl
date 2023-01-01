struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float4 tint : COLOR;
  float2 maskUV : TEXCOORD1;
};

sampler ColorMap : register(s0);

float4 main(PS_INPUT input) : COLOR {
  return input.tint * tex2D(ColorMap, input.uv);
}
