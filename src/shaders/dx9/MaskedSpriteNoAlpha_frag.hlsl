float2 Alpha : register(c0);
bool IsInverted : register(b0);

struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float4 tint : COLOR;
  float2 maskUV : TEXCOORD1;
};

sampler ColorMap : register(s0);
sampler Mask : register(s1);

float rgbToLightness(float3 rgb) {
  float maxVal = max(max(rgb.r, rgb.g), rgb.b);
  float minVal = min(min(rgb.r, rgb.g), rgb.b);

  return (maxVal + minVal) / 2.0;
}

float4 main(PS_INPUT input) : COLOR {
  float4 color = tex2D(ColorMap, input.uv);
  float4 alp = tex2D(Mask, input.maskUV);
  float maskAlpha = rgbToLightness(alp.rgb);
  if (IsInverted) maskAlpha = 1.0f - maskAlpha;
  maskAlpha *= Alpha.x;
  maskAlpha -= Alpha.y;
  maskAlpha = clamp(maskAlpha, 0.0, 1.0);
  color *= input.tint;
  color.a *= maskAlpha;
  return color;
}
