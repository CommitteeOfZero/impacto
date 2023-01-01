float2 Alpha : register(c10);
int IsInverted : register(c11);
int IsSameTexture : register(c12);

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
  float4 alp = tex2D(Mask, input.maskUV);
  if (IsInverted) alp.a = 1.0f - alp.a;
  alp.a *= Alpha.x;
  alp.a -= Alpha.y;
  alp.a = clamp(alp.a, 0.0, 1.0);
  if (IsSameTexture) {
    color.a = alp.a;
    color.a *= input.tint.a;
  } else {
    color *= input.tint;
    color.a *= alp.a;
  }

  return color;
}
