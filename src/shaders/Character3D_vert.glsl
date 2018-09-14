layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;
layout(location = 3) in lowp ivec4 BoneIndices;  // indices into Mesh.BoneMap
layout(location = 4) in vec4 BoneWeights;

out vec3 normal;
out vec2 uv;

uniform mat4 ViewProjection;
uniform mat4 Model;
uniform mat4 Bones[ModelMaxBonesPerMesh];

void main() {
  // Accumulated skinning, thanks
  // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch04.html
  mat4 skeletalTransform = Bones[BoneIndices.x] * BoneWeights.x +
                           Bones[BoneIndices.y] * BoneWeights.y +
                           Bones[BoneIndices.z] * BoneWeights.z +
                           Bones[BoneIndices.w] * BoneWeights.w;

  mat4 transform = Model * skeletalTransform;
  mat3 normalMatrix = mat3(transpose(inverse(transform)));

  gl_Position = ViewProjection * transform * vec4(Position, 1.0);
  normal = normalMatrix * Normal;
  uv = UV;
}