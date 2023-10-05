float4 Alpha : register(c0);

struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float4 tint : COLOR;
  float2 maskUV : TEXCOORD1;
};

sampler ColorMap : register(s0);
sampler Mask : register(s1);

float remapMiddle(float value, float newMiddle) {
  if (value < 0.5) {
      return lerp(0.0, newMiddle, 2.0 * value);
  } else {
      return lerp(newMiddle, 1.0, 2.0 * value - 1.0);
  }
}

float4 main(PS_INPUT input) : COLOR {
  float4 color = tex2D(ColorMap, input.uv);
  float4 mask = tex2D(Mask, input.maskUV);
  float4 outColor = float4(
      remapMiddle(color.r, mask.r),
      remapMiddle(color.g, mask.g),
      remapMiddle(color.b, mask.b),
      mask.a * Alpha.r
  );

  return outColor;
}
