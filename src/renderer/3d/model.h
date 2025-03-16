#pragma once

#include <enum.h>
#include "../../impacto.h"
#include "../../texture/texture.h"
#include "transform.h"

#include <glm/glm.hpp>

#include <ankerl/unordered_dense.h>

// BIG TODO: Use integer types consistently internally (not always possible for
// file content, but eh)

namespace Impacto {

BETTER_ENUM(LKMVersion, int, RNE = 0, DaSH)

enum ModelType : uint32_t {
  ModelType_Background = 1,
  ModelType_Character = 2,
  ModelType_DaSH_Unspecified = (2 << 16)
};

enum MeshFlag : uint32_t {
  // MeshFlag_DoubleSided = (1 << 0),
  MeshFlag_Later = (1 << 1)
};

int constexpr ModelMaxChildrenPerBone = 133;
int constexpr ModelMaxMorphTargetsPerModel = 256;
int constexpr ModelMaxMorphTargetsPerMesh = 32;
int constexpr ModelUnknownsAfterMorphTargets = 12;
int constexpr ModelUnknownsAfterMorphTargets_DaSH = 4;
// TODO: How do we actually want to do this?
// Character models generally have <300 bones. Some background models have >600
// bones (what are these for? - some of them seem broken).
// Dynamic array?
int constexpr ModelMaxBonesPerModel = 768;
int constexpr ModelMaxBonesPerMesh = 32;
int constexpr ModelMaxMeshesPerModel = 64;
int constexpr ModelMaxRootBones = 32;
int constexpr ModelMaxTexturesPerModel = 64;

inline uint32_t* g_ModelIds;
inline char** g_ModelNames;
inline uint32_t g_ModelCount;

inline uint32_t* g_BackgroundModelIds;
inline char** g_BackgroundModelNames;
inline uint32_t g_BackgroundModelCount;

struct VertexBuffer {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 UV;
  uint8_t BoneIndices[4];  // indices into Mesh.BoneMap
  glm::vec4 BoneWeights;
};

struct VertexBufferDaSH {
  glm::vec3 Position;
  glm::vec3 Normal;

  glm::vec3 unk1;  // tangent, only used in generic and skin
  glm::vec4 unk2;  // all 1 in the first few vertices of c001_010, apparently
                   // not used in rendering

  glm::vec2 UV;
  uint8_t BoneIndices[4];  // indices into Mesh.BoneMap
  glm::vec4 BoneWeights;
};

struct BgVertexBuffer {
  glm::vec3 Position;
  glm::vec2 UV;
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
  // DaSH addition
  uint8_t Name[32];

  int16_t Id;
  int16_t Parent;
  int16_t ChildrenCount;
  int16_t Children[ModelMaxChildrenPerBone];  // TODO check

  Transform BaseTransform;

  glm::mat4 BindInverse;
};

enum TextureType {
  TT_ColorMap = 0,
  TT_GradientMaskMap = 2,
  TT_SpecularColorMap = 4,
  TT_Character3D_Max = 4,

  TT_Eye_IrisColorMap = 0,
  TT_Eye_WhiteColorMap = 1,
  TT_Eye_IrisSpecularColorMap = 2,
  TT_Eye_HighlightColorMap = 3,
  TT_Eye_Max = 3,

  TT_DaSH_ColorMap = 0,
  TT_DaSH_Unk1 = 1,  // 1 and 3 are gradients
  TT_DaSH_Unk2 = 2,  // 1 pixel
  TT_DaSH_Unk3 = 3,
  TT_DaSH_ShadowColorMap = 4,
  TT_DaSH_GradientMaskMap = 5,
  TT_DaSH_SpecularColorMap = 6,
  TT_DaSH_NoiseMap = 7,  // Noise

  TT_DaSH_Eye_WhiteColorMap = 0,
  TT_DaSH_Eye_IrisColorMap = 1,
  TT_DaSH_Eye_HighlightColorMap = 2,

  TT_Count = 8
};

enum MaterialType {
  // Ours
  MT_None = 0,
  MT_Outline = 1,
  MT_Generic = 2,
  MT_Eye = 3,
  MT_Background = 4,
  // DaSH
  MT_DaSH_Generic = 200,
  MT_DaSH_Eye = 300,
  MT_DaSH_Face = 400,
  MT_DaSH_Skin = 500
};

struct Mesh {
  // DaSH addition
  uint8_t Name[32];

  // Meshes in one group are animated together
  int32_t GroupId;

  int32_t Id;

  uint32_t VertexCount;
  // Offset (in sizeof(VertexBuffer) units) into Model.VertexBuffers
  int32_t VertexOffset;
  int32_t IndexCount;
  // Offset (in sizeof(uint16_t) units) into Model.Indices
  int32_t IndexOffset;

  MaterialType Material;
  uint32_t Flags;
  float Opacity;

  // This is only used with background models
  Transform ModelTransform;

  uint8_t MorphTargetIds[ModelMaxMorphTargetsPerMesh];

  uint32_t UsedBones;                     // 0 => not skinned
  int16_t BoneMap[ModelMaxBonesPerMesh];  // indices into Model.Bones

  int16_t Maps[TT_Count];
  bool HasShadowColorMap;

  // Sets transform for meshes without individual vertex skinning
  int16_t MeshBone;

  uint8_t MorphTargetCount;
};

class ModelAnimation;

class Model {
 public:
  static void EnumerateModels();

  // Parses a R;NE model file. No GPU submission happens in this class.
  static Model* Load(uint32_t modelId);
  // Ground plane
  static Model* MakePlane();

  ~Model();

  uint32_t Id;

  ModelType Type;

  uint32_t MeshCount = 0;
  Mesh Meshes[ModelMaxMeshesPerModel];

  int32_t VertexCount = 0;
  void* VertexBuffers = 0;

  uint32_t MorphTargetCount = 0;
  MorphTarget MorphTargets[ModelMaxMorphTargetsPerModel];

  int32_t MorphVertexCount = 0;
  MorphVertexBuffer* MorphVertexBuffers = 0;

  int32_t IndexCount = 0;
  uint16_t* Indices = 0;

  uint32_t TextureCount = 0;
  Texture Textures[ModelMaxTexturesPerModel];

  uint32_t BoneCount = 0;
  StaticBone Bones[ModelMaxBonesPerModel];

  int32_t RootBoneCount = 0;
  int16_t RootBones[ModelMaxRootBones];

  ankerl::unordered_dense::map<int16_t, ModelAnimation*> Animations;

  // These are only filled for DaSH
  ankerl::unordered_dense::map<std::string, int32_t> NamedMeshGroups;
  ankerl::unordered_dense::map<std::string, uint16_t> NamedBones;

  int16_t IdleAnimation = -1;

  int32_t* AnimationIds = 0;
  char** AnimationNames = 0;
  uint32_t AnimationCount = 0;
};

}  // namespace Impacto