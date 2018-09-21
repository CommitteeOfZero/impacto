in vec2 uv;

out vec4 color;

uniform sampler2D ColorMap;

layout(std140) uniform Character3DCommon {
  mat4 ViewProjection;
  mat4 Model;
  mat4 Bones[ModelMaxBonesPerMesh];
  vec4 Tint;
  vec3 WorldLightPosition;
  vec3 WorldEyePosition;
  float ModelOpacity;
};

void main() {
  const vec4 outlineColor = vec4(0.46484375, 0.27734375, 0.22265625, 1.0);
  vec4 texColor = texture(ColorMap, uv);

  color = outlineColor * texColor;
  color.a *= ModelOpacity;
}