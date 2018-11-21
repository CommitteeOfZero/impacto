layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;
layout(location = 3) in lowp ivec4 BoneIndices;  // indices into Mesh.BoneMap
layout(location = 4) in vec4 BoneWeights;

out vec2 uv;

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

// TODO there's a uniform for this somewhere...
const float OutlineThickness = 0.03;

void main() {
  // Accumulated skinning, thanks
  // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch04.html
  mat4 skeletalTransform = Bones[BoneIndices.x] * BoneWeights.x +
                           Bones[BoneIndices.y] * BoneWeights.y +
                           Bones[BoneIndices.z] * BoneWeights.z +
                           Bones[BoneIndices.w] * BoneWeights.w;

  mat4 transform = Model * skeletalTransform;
  mat3 normalMatrix = mat3(transpose(inverse(transform)));

  vec3 worldNormal = normalMatrix * Normal;

  vec4 viewNormal = normalize(ViewProjection * vec4(worldNormal, 0.0));

  vec4 worldPosition = transform * vec4(Position, 1.0);

  gl_Position = ViewProjection * worldPosition;
  gl_Position += viewNormal * OutlineThickness;

  uv = UV;
}