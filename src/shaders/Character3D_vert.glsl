layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;
layout(location = 3) in lowp ivec4 BoneIndices;  // indices into Mesh.BoneMap
layout(location = 4) in vec4 BoneWeights;

out vec3 normal;
out vec2 uv;

uniform mat4 ViewProjection;
uniform mat4 Bones[ModelMaxBonesPerMesh];

void main() {
  // Accumulated skinning, thanks
  // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch04.html
  mat4 SkeletalTransform = Bones[BoneIndices.x] * BoneWeights.x +
                           Bones[BoneIndices.y] * BoneWeights.y +
                           Bones[BoneIndices.z] * BoneWeights.z +
                           Bones[BoneIndices.w] * BoneWeights.w;

  gl_Position = ViewProjection * SkeletalTransform * vec4(Position, 1.0);
  normal = normalize(SkeletalTransform * vec4(Normal, 0.0)).xyz;
  uv = UV;
}