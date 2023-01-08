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

static const float FALLOFF_POWER = 0.8;

static const float4 g_ToonFalloffGradVals =
    float4(0.546875, 0.421875, 0.468750, 0.281250);
static const float4 g_ToonFalloffGradScale =
    float4(10.66667, 10.66667, 32.0, 32.0);
static const float4 g_ToonFalloffGradDarkVals =
    float4(0.59375, 0.484375, 0.609375, 0.453125);
static const float4 g_ToonFalloffGradDarkMax = float4(0.79, 0.79, 0.61, 0.61);

struct PS_INPUT {
  float4 position : POSITION;
  float2 uv : TEXCOORD0;
  float3 worldFragPosition : TEXCOORD1;
  float3 worldNormal : TEXCOORD2;
};

sampler ColorMap : register(s0);
sampler GradientMaskMap : register(s1);
sampler SpecularColorMap : register(s2);
sampler ShadowColorMap : register(s3);

float4 main(PS_INPUT input) : COLOR {
  float4 texColor = tex2D(ColorMap, input.uv);
  float maskParam = tex2D(GradientMaskMap, input.uv).r;
  float3 specColor = tex2D(SpecularColorMap, input.uv).rgb;

  float3 normal = normalize(input.worldNormal);
  float3 dirToLight = normalize(WorldLightPosition - input.worldFragPosition);
  float3 dirToEye = normalize(WorldEyePosition - input.worldFragPosition);

  float NdotL = dot(normal, dirToLight);
  float scaledDiff = 0.5 * NdotL + 0.5;

  // Note: this is a uniform in R;NE ???
  float3 H = normalize(dirToLight + dirToEye);

  float4 toonFalloffGradInput;
  toonFalloffGradInput.xy = (scaledDiff).xx;

  float normDotEye = 1.0 - dot(normal, dirToEye);
  toonFalloffGradInput.zw = (scaledDiff * normDotEye).xx;

  // Original just uses step here but that looks bad
  float4 toonFalloffGradParamDark =
      smoothstep(g_ToonFalloffGradDarkVals * 0.95,
                 g_ToonFalloffGradDarkVals * 1.05, toonFalloffGradInput) *
      g_ToonFalloffGradDarkMax;

  float4 toonFalloffGradParamLight = saturate(
      (toonFalloffGradInput - g_ToonFalloffGradVals) * g_ToonFalloffGradScale);

  float4 toonFalloffGradParam;
  if (DarkMode) {
    toonFalloffGradParam = toonFalloffGradParamDark;
  } else {
    toonFalloffGradParam = toonFalloffGradParamLight;
  }

  float2 toonFalloffInterpParam =
      lerp(toonFalloffGradParam.xz, toonFalloffGradParam.yw, maskParam);

  float3 shadowColor;
#if defined(IsDaSH)
  if (HasShadowColorMap) {
    shadowColor = tex2D(ShadowColorMap, input.uv).rgb;
  } else {
#endif
    shadowColor = max(texColor.rgb, (0.5).xxx) * texColor.rgb;
#if IsDaSH
  }
#endif

  float3 toonColor = lerp(shadowColor, texColor.rgb, toonFalloffInterpParam.x);

  // Tint

  float3 baseTintColor = toonColor * Tint.rgb;
  toonColor = lerp(toonColor, baseTintColor, Tint.a);

  // Rimlight

  float3 falloffColor = FALLOFF_POWER * toonFalloffInterpParam.y * texColor.rgb;
  toonColor += falloffColor * ((1.0).xxx - toonColor);

  // Specular
  // R;NE does not use max() here - maybe manual control of H ensures it doesn't
  // go negative?
  float specIntensity = pow(max(dot(normal, H), 0.0), 20.0);
  toonColor += specIntensity * specColor;

  return float4(toonColor, texColor.a * ModelOpacity);
}
