bool IsAlpha : register(b0);

struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float4 tint : COLOR;
  float2 maskUV : TEXCOORD1;
};

sampler Luma : register(s0);
sampler Cb : register(s1);
sampler Cr : register(s2);

static float4x4 yuv_to_rgb_rec601 = {
    1.16438, 0.00000, 1.59603, -0.87079, 1.16438, -0.39176, -0.81297, 0.52959,
    1.16438, 2.01723, 0.00000, -1.08139, 0,       0,        0,        1};

float4 getRGBA(float2 texUv) {
  float4 yuvcolor = {1.0, 1.0, 1.0, 1.0};
  yuvcolor.r = tex2D(Luma, texUv).a;
  yuvcolor.g = tex2D(Cb, texUv).a;
  yuvcolor.b = tex2D(Cr, texUv).a;

  return mul(yuv_to_rgb_rec601, yuvcolor);
}

float4 main(PS_INPUT input) : COLOR {
  float4 color = {1.0, 1.0, 1.0, 1.0};

  if (IsAlpha) {
    color = getRGBA(float2(input.uv.x, input.uv.y / 2.0));
    color.a = getRGBA(float2(input.uv.x, input.uv.y / 2.0 + 0.5)).a;
  } else {
    color = getRGBA(input.uv);
  }
  color *= input.tint;

  return color;
}
