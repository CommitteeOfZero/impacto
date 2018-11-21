in vec2 uv;

out vec4 color;

uniform sampler2D ColorMap;

layout(std140) uniform Character3DScene {
  UNIFORM_PRECISION mat4 ViewProjection;
  UNIFORM_PRECISION vec4 Tint;
  UNIFORM_PRECISION vec3 WorldLightPosition;
  UNIFORM_PRECISION vec3 WorldEyePosition;
};

layout(std140) uniform Character3DModel { UNIFORM_PRECISION mat4 Model; };

layout(std140) uniform Character3DMesh {
  UNIFORM_PRECISION mat4 Bones[ModelMaxBonesPerMesh];
  UNIFORM_PRECISION float ModelOpacity;
};

void main() {
  const vec4 outlineColor = vec4(0.46484375, 0.27734375, 0.22265625, 1.0);
  vec4 texColor = texture(ColorMap, uv);

  color = outlineColor * texColor;
  color.a *= ModelOpacity;
}