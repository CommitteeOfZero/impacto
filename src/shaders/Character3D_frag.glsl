in vec3 worldFragPosition;
in vec3 worldNormal;
in vec2 uv;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D GradientMaskMap;
uniform sampler2D SpecularColorMap;

uniform float ModelOpacity;
uniform vec3 WorldLightPosition;
uniform vec3 WorldEyePosition;
uniform vec4 Tint;

const float FALLOFF_POWER = 0.8;

const vec4 g_ToonFalloffGradVals = vec4(0.546875, 0.421875, 0.468750, 0.281250);
const vec4 g_ToonFalloffGradScale = vec4(10.66667, 10.66667, 32.0, 32.0);
const vec4 g_ToonFalloffGradDarkVals =
    vec4(0.59375, 0.484375, 0.609375, 0.453125);
const vec4 g_ToonFalloffGradDarkMax = vec4(0.79, 0.79, 0.61, 0.61);

vec4 saturate(vec4 x) { return clamp(x, 0.0, 1.0); }

void main() {
  vec4 texColor = texture(ColorMap, uv);
  float maskParam = texture(GradientMaskMap, uv).r;
  vec3 specColor = texture(SpecularColorMap, uv).rgb;

  vec3 normal = normalize(worldNormal);
  vec3 dirToLight = normalize(WorldLightPosition - worldFragPosition);
  vec3 dirToEye = normalize(WorldEyePosition - worldFragPosition);

  float NdotL = dot(normal, dirToLight);
  float scaledDiff = 0.5 * NdotL + 0.5;

  // Note: this is a uniform in R;NE ???
  vec3 H = normalize(dirToLight + dirToEye);

  vec4 toonFalloffGradInput;
  toonFalloffGradInput.xy = vec2(scaledDiff);

  float normDotEye = 1.0 - dot(normal, dirToEye);
  toonFalloffGradInput.zw = vec2(scaledDiff * normDotEye);

  /*vec4 toonFalloffGradParam =
      step(g_ToonFalloffGradDarkVals, toonFalloffGradInput) *
      g_ToonFalloffGradDarkMax;*/

  vec4 toonFalloffGradParam = saturate(
      (toonFalloffGradInput - g_ToonFalloffGradVals) * g_ToonFalloffGradScale);

  vec2 toonFalloffInterpParam =
      mix(toonFalloffGradParam.xz, toonFalloffGradParam.yw, maskParam);

  vec3 shadowColor = max(texColor.rgb, vec3(0.5)) * texColor.rgb;
  vec3 toonColor = mix(shadowColor, texColor.rgb, toonFalloffInterpParam.x);

  // Tint

  vec3 baseTintColor = toonColor * Tint.rgb;
  toonColor = mix(toonColor, baseTintColor, Tint.a);

  // Rimlight

  vec3 falloffColor = FALLOFF_POWER * toonFalloffInterpParam.y * texColor.rgb;
  toonColor += falloffColor * (vec3(1.0) - toonColor);

  // Specular
  // R;NE does not use max() here - maybe manual control of H ensures it doesn't
  // go negative?
  float specIntensity = pow(max(dot(normal, H), 0.0), 20.0);
  toonColor += specIntensity * specColor;

  color = vec4(toonColor, texColor.a * ModelOpacity);
}