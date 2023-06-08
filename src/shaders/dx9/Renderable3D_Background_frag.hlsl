struct PS_INPUT {
  float2 uv : TEXCOORD0;
};

sampler ColorMap : register(s0);

float4 main(PS_INPUT input) : COLOR {
  return tex2D(ColorMap, input.uv);
}
