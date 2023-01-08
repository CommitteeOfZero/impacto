float4 Alpha : register(c0);

struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float4 tint : COLOR;
  float2 maskUV : TEXCOORD1;
};

sampler ColorMap : register(s0);
sampler Mask : register(s1);

float4 main(PS_INPUT input) : COLOR {
  float4 color = tex2D(ColorMap, input.uv);
  color *= input.tint;
  float4 alp = tex2D(Mask, input.maskUV);
  float ga = alp.r;
  float ea = 1.0 - alp.g;
  float fa = 1.0 - alp.b;
  fa *= Alpha.r;
  fa -= Alpha.g;
  fa = clamp(fa, 0.0, 1.0);
  color.a *= fa;

  ea += Alpha.b;
  if (ea > 1.0) ea = 2.0 - ea;
  ea *= 2.0;
  if (ea > 1.0) ea = 2.0 - ea;

  ga *= ea;
  ga = clamp(ga, 0.0, 1.0);
  color.a -= ga;
  color.a = clamp(color.a, 0.0, 1.0);

  return color;
}
