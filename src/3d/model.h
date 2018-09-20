#pragma once

#include "../impacto.h"
#include "../texture/texture.h"
#include "transform.h"

#include <glm/glm.hpp>

#include "../../vendor/include/flat_hash_map.hpp"

namespace Impacto {

enum MeshFlag : uint32_t { MeshFlag_DoubleSided = (1 << 0) };

int const ModelMaxChildrenPerBone = 134;
int const ModelMaxMorphTargetsPerModel = 256;
int const ModelMaxMorphTargetsPerMesh = 0x50;
int const ModelMaxBonesPerModel = 320;
int const ModelMaxBonesPerMesh = 32;
int const ModelMaxMeshesPerModel = 32;
int const ModelMaxRootBones = 32;
int const ModelMaxTexturesPerModel = 32;

struct VertexBuffer {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 UV;
  uint8_t BoneIndices[4];  // indices into Mesh.BoneMap
  glm::vec4 BoneWeights;
};

struct MorphVertexBuffer {
  glm::vec3 Position;
  glm::vec3 Normal;
};

struct MorphTarget {
  int32_t VertexCount;
  // Offset (in sizeof(MorphVertexBuffer) units) into Model.MorphVertexBuffers
  int32_t VertexOffset;
};

struct StaticBone {
  int16_t Id;
  int16_t Parent;
  int16_t ChildrenCount;
  int16_t Children[ModelMaxChildrenPerBone];  // TODO check

  Transform BaseTransform;

  glm::mat4 BindInverse;
};

struct Mesh {
  // Meshes in one group are animated together
  int32_t GroupId;

  int32_t Id;

  int32_t VertexCount;
  // Offset (in sizeof(VertexBuffer) units) into Model.VertexBuffers
  int32_t VertexOffset;
  int32_t IndexCount;
  // Offset (in sizeof(uint16_t) units) into Model.Indices
  int32_t IndexOffset;

  uint32_t Flags;
  float Opacity;

  uint8_t MorphTargetIds[ModelMaxMorphTargetsPerMesh];

  uint32_t UsedBones;                     // 0 => not skinned
  int16_t BoneMap[ModelMaxBonesPerMesh];  // indices into Model.Bones

  int16_t ColorMap;
  int16_t DecalMap;
  int16_t GradientMaskMap;
  int16_t FourthMap;
  int16_t SpecularColorMap;
  int16_t SixthMap;

  // Sets transform for meshes without individual vertex skinning
  int16_t MeshBone;

  uint8_t MorphTargetCount;
};

class Animation;

class Model {
 public:
  static void Init();

  // Parses a R;NE model file. No GPU submission happens in this class.
  static Model* Load(uint32_t modelId);
  // Ground plane
  static Model* MakePlane();

  ~Model();

  uint32_t Id;

  int32_t MeshCount = 0;
  Mesh Meshes[ModelMaxMeshesPerModel];

  int32_t VertexCount = 0;
  VertexBuffer* VertexBuffers = 0;

  int32_t MorphTargetCount = 0;
  MorphTarget MorphTargets[ModelMaxMorphTargetsPerModel];

  int32_t MorphVertexCount = 0;
  MorphVertexBuffer* MorphVertexBuffers = 0;

  int32_t IndexCount = 0;
  uint16_t* Indices = 0;

  int32_t TextureCount = 0;
  Texture Textures[ModelMaxTexturesPerModel];

  int32_t BoneCount = 0;
  StaticBone Bones[ModelMaxBonesPerModel];

  int32_t RootBoneCount = 0;
  int16_t RootBones[ModelMaxRootBones];

  ska::flat_hash_map<uint16_t, Animation*> Animations;
};

}  // namespace Impacto