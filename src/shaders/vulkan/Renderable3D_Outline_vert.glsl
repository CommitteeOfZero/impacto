#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;
layout(location = 3) in lowp ivec4 BoneIndices;  // indices into Mesh.BoneMap
layout(location = 4) in vec4 BoneWeights;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec2 uvNoise;

layout(binding = 0) uniform Character3DScene {
  mat4 ViewProjection;
  vec4 Tint;
  vec3 WorldLightPosition;
  vec3 WorldEyePosition;
  bool DarkMode;
};

layout(binding = 1) uniform Character3DModel { highp mat4 Model; };

layout(binding = 2) uniform Character3DMesh {
  mat4 Bones[32];
  float ModelOpacity;
  bool HasShadowColorMap;
};

// TODO there's a uniform for this somewhere...
// #if DASH
// const float OutlineThickness = 0.0035;
// #else
// const float OutlineThickness = 0.035;
// #endif

layout(push_constant) uniform constants
{
	bool IsDash;
} PushConstants;

void main() {
  // Accumulated skinning, thanks
  // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch04.html
  mat4 skeletalTransform = Bones[BoneIndices.x] * BoneWeights.x +
                           Bones[BoneIndices.y] * BoneWeights.y +
                           Bones[BoneIndices.z] * BoneWeights.z +
                           Bones[BoneIndices.w] * BoneWeights.w;

  mat4 transform = Model * skeletalTransform;
  mat3 normalMatrix = mat3(inverse(transform));

  vec3 worldNormal = normalMatrix * Normal;

  vec4 viewNormal = normalize(ViewProjection * vec4(worldNormal, 0.0));

  vec4 worldPosition = transform * vec4(Position, 1.0);

  float OutlineThickness = 0.035;
  if (PushConstants.IsDash) {
    OutlineThickness = 0.0035;
  }

  gl_Position = ViewProjection * worldPosition;
  gl_Position += viewNormal * OutlineThickness;

  uv = UV;

  uvNoise = (gl_Position.xy / gl_Position.w) * 0.2;
}