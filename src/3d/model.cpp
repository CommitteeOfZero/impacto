#include "model.h"

#include "animation.h"
#include "../io/vfs.h"
#include "../io/io.h"
#include "../log.h"
#include "../texture/gxtloader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Impacto {

static VfsArchive* AllModelsArchive = NULL;

const uint32_t ModelFileCountsOffset = 0x24;
const uint32_t ModelFileHeaderSize = 0x54;
const uint32_t ModelFileMeshInfoSize = 0x18C;
const uint32_t ModelFileBoneSize = 0x1D0;
const uint32_t MorphTargetInfoSize = 16;
const uint32_t BoneBaseTransformOffset = 0x11C;

uint32_t* g_ModelIds;
char** g_ModelNames;
uint32_t g_ModelCount;

uint32_t* g_AnimationIds;
char** g_AnimationNames;
uint32_t g_AnimationCount;

bool AnimationIsBlacklisted(uint32_t modelId, uint16_t animId) {
  // This animation file is just broken
  if (modelId == 273 && animId == 22) return true;
  return false;
}

void Model::Init() {
  assert(AllModelsArchive == NULL);
  ImpLog(LL_Info, LC_ModelLoad, "Initializing model loader\n");
  IoError err = VfsArchive::Mount("model.cpk", &AllModelsArchive);
  if (err != IoError_OK) {
    AllModelsArchive = NULL;
    ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive: %d\n", err);
  }

  // List models
  // TODO: We don't need this in the game - take it out when we remove the model
  // viewer, it's a waste of time then

  g_AnimationCount = 0;
  g_AnimationNames = 0;
  g_AnimationIds = 0;

  g_ModelCount = 0;

  uint32_t iterator;
  VfsFileInfo modelInfo;
  err = AllModelsArchive->EnumerateStart(&iterator, &modelInfo);
  while (err == IoError_OK) {
    if (modelInfo.Name[0] == 'c' || modelInfo.Name[0] == 'C') {
      g_ModelCount++;
    }
    err = AllModelsArchive->EnumerateNext(&iterator, &modelInfo);
  }

  uint32_t currentModel = 0;

  g_ModelIds = (uint32_t*)malloc(g_ModelCount * sizeof(uint32_t));
  g_ModelNames = (char**)malloc(g_ModelCount * sizeof(char*));
  err = AllModelsArchive->EnumerateStart(&iterator, &modelInfo);
  while (err == IoError_OK) {
    if (modelInfo.Name[0] == 'c' || modelInfo.Name[0] == 'C') {
      g_ModelIds[currentModel] = modelInfo.Id;
      g_ModelNames[currentModel] = strdup(modelInfo.Name);
      currentModel++;
    }
    err = AllModelsArchive->EnumerateNext(&iterator, &modelInfo);
  }
}

Model::~Model() {
  if (VertexBuffers) free(VertexBuffers);
  if (MorphVertexBuffers) free(MorphVertexBuffers);
  if (Indices) free(Indices);
  for (auto animation : Animations) {
    if (animation.second) delete animation.second;
  }
  if (g_AnimationIds) {
    free(g_AnimationIds);
    g_AnimationIds = 0;
  }
  if (g_AnimationNames) {
    for (int i = 0; i < g_AnimationCount; i++) {
      if (g_AnimationNames[i]) free(g_AnimationNames[i]);
    }
    free(g_AnimationNames);
    g_AnimationNames = 0;
  }
  g_AnimationCount = 0;
}

Model* Model::Load(uint32_t modelId) {
  assert(AllModelsArchive != NULL);
  ImpLogSlow(LL_Debug, LC_ModelLoad, "Loading model %d\n", modelId);

  VfsArchive* modelArchive;
  IoError err = AllModelsArchive->MountChild(modelId, &modelArchive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive for %d\n",
           modelId);
    return NULL;
  }

  void* file;
  int64_t fileSize;
  err = modelArchive->Slurp((uint32_t)0, &file, &fileSize);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not read model file for %d\n",
           modelId);
    delete modelArchive;
    return NULL;
  }

  // We read the whole file at once for speed, but we still want a stream for
  // convenience
  SDL_RWops* stream = SDL_RWFromConstMem(file, fileSize);

  Model* result = new Model;
  result->Id = modelId;

  // Read model resource counts and offsets
  SDL_RWseek(stream, ModelFileCountsOffset, RW_SEEK_SET);
  result->MeshCount = SDL_ReadLE32(stream);
  assert(result->MeshCount <= ModelMaxMeshesPerModel);
  result->BoneCount = SDL_ReadLE32(stream);
  assert(result->BoneCount <= ModelMaxBonesPerModel);
  result->TextureCount = SDL_ReadLE32(stream);
  assert(result->TextureCount <= ModelMaxTexturesPerModel);
  result->MorphTargetCount = SDL_ReadLE32(stream);
  assert(result->MorphTargetCount <= ModelMaxMorphTargetsPerModel);

  uint32_t MeshInfosOffset = SDL_ReadLE32(stream);
  uint32_t BonesOffset = SDL_ReadLE32(stream);
  uint32_t TexturesOffset = SDL_ReadLE32(stream);
  uint32_t MorphVertexOffset = SDL_ReadLE32(stream);
  uint32_t MorphTargetOffset = SDL_ReadLE32(stream);

  result->VertexCount = 0;
  result->IndexCount = 0;

  // Accumulate per-mesh vertex buffer / index counts
  for (uint32_t i = 0; i < result->MeshCount; i++) {
    SDL_RWseek(stream, MeshInfosOffset + ModelFileMeshInfoSize * i,
               RW_SEEK_SET);
    SDL_RWseek(stream, 0xE0, RW_SEEK_CUR);
    result->VertexCount += SDL_ReadLE32(stream);
    result->IndexCount += SDL_ReadLE32(stream);
  }

  result->VertexBuffers =
      (VertexBuffer*)calloc(result->VertexCount, sizeof(VertexBuffer));
  result->Indices = (uint16_t*)calloc(result->IndexCount, sizeof(uint16_t));

  // Read mesh attributes
  for (uint32_t i = 0; i < result->MeshCount; i++) {
    SDL_RWseek(stream, MeshInfosOffset + ModelFileMeshInfoSize * i,
               RW_SEEK_SET);
    Mesh* mesh = &result->Meshes[i];

    mesh->Id = i;

    mesh->GroupId = SDL_ReadLE32(stream);
    mesh->MeshBone = SDL_ReadLE16(stream);
    SDL_RWseek(stream, 9, RW_SEEK_CUR);
    mesh->MorphTargetCount = SDL_ReadU8(stream);
    assert(mesh->MorphTargetCount <= ModelMaxMorphTargetsPerMesh);
    SDL_RWread(stream, mesh->MorphTargetIds, 1, ModelMaxMorphTargetsPerMesh);
    SDL_RWseek(stream, 0x80, RW_SEEK_CUR);
    mesh->VertexCount = SDL_ReadLE32(stream);
    mesh->IndexCount = SDL_ReadLE32(stream);

    SDL_RWseek(stream, 0x6C, RW_SEEK_CUR);
    mesh->Opacity = ReadFloatLE32(stream);

    SDL_RWseek(stream, 0x14, RW_SEEK_CUR);
    ReadArrayLE16(mesh->Maps, stream, TT_Count);

    SDL_RWseek(stream, 4, RW_SEEK_CUR);
    mesh->Flags = SDL_ReadLE32(stream);
  }

  // Read vertex buffers and indices
  int32_t CurrentVertexOffset = 0;
  int32_t CurrentIndexOffset = 0;
  for (uint32_t i = 0; i < result->MeshCount; i++) {
    SDL_RWseek(stream, MeshInfosOffset + ModelFileMeshInfoSize * i,
               RW_SEEK_SET);
    Mesh* mesh = &result->Meshes[i];
    mesh->VertexOffset = CurrentVertexOffset;
    mesh->IndexOffset = CurrentIndexOffset;

    SDL_RWseek(stream, 0xF0, RW_SEEK_CUR);
    int32_t RawVertexOffset = ModelFileHeaderSize + SDL_ReadLE32(stream);
    int32_t RawIndexOffset = ModelFileHeaderSize + SDL_ReadLE32(stream);

    mesh->UsedBones = SDL_ReadLE32(stream);
    assert(mesh->UsedBones <= ModelMaxBonesPerMesh);
    ReadArrayLE16(mesh->BoneMap, stream, mesh->UsedBones);

    // Read vertex buffers
    SDL_RWseek(stream, RawVertexOffset, RW_SEEK_SET);
    for (uint32_t j = 0; j < mesh->VertexCount; j++) {
      VertexBuffer* vertex = &result->VertexBuffers[CurrentVertexOffset];
      CurrentVertexOffset++;
      // Position, then Normal, then UV
      ReadArrayFloatLE32((float*)&vertex->Position, stream, 3 * 2 + 2 * 1);

      if (mesh->UsedBones > 0) {
        SDL_RWread(stream, vertex->BoneIndices, 1, 4);

        ReadVec4LE32(&vertex->BoneWeights, stream);
      } else {
        // TODO skinned/unskinned mesh distinction?
        *(int*)vertex->BoneIndices = 0;

        vertex->BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

        SDL_RWseek(stream, 4 * sizeof(uint8_t) + 4 * sizeof(float),
                   RW_SEEK_CUR);
      }
    }

    // Read indices
    SDL_RWseek(stream, RawIndexOffset, RW_SEEK_SET);
    ReadArrayLE16(result->Indices + CurrentIndexOffset, stream,
                  mesh->IndexCount);
    CurrentIndexOffset += mesh->IndexCount;
  }

  // Read skeleton
  for (uint32_t i = 0; i < result->BoneCount; i++) {
    SDL_RWseek(stream, BonesOffset + ModelFileBoneSize * i, RW_SEEK_SET);
    StaticBone* bone = &result->Bones[i];

    bone->Id = SDL_ReadLE16(stream);
    SDL_RWseek(stream, 2, RW_SEEK_CUR);
    bone->Parent = SDL_ReadLE16(stream);
    if (bone->Parent < 0) {
      result->RootBones[result->RootBoneCount] = i;
      result->RootBoneCount++;
      assert(result->RootBoneCount < ModelMaxRootBones);
    }
    SDL_RWseek(stream, 8, RW_SEEK_CUR);
    bone->ChildrenCount = SDL_ReadLE16(stream);
    assert(bone->ChildrenCount <= ModelMaxChildrenPerBone);
    ReadArrayLE16(bone->Children, stream, bone->ChildrenCount);
    SDL_RWseek(stream,
               BonesOffset + ModelFileBoneSize * i + BoneBaseTransformOffset,
               RW_SEEK_SET);

    glm::vec3 position, euler, scale;
    ReadVec3LE32(&position, stream);
    ReadVec3LE32(&euler, stream);
    ReadVec3LE32(&scale, stream);
    // More often than not these are actually not set...
    if (glm::length(scale) < 0.001f) scale = glm::vec3(1.0f);
    bone->BaseTransform = Transform(position, euler, scale);

    // skip over bindpose
    SDL_RWseek(stream, sizeof(glm::mat4), RW_SEEK_CUR);

    ReadMat4LE32(&bone->BindInverse, stream);
  }

  result->MorphVertexCount = 0;
  // Accumulate per-morph target vertex buffer counts
  for (uint32_t i = 0; i < result->MorphTargetCount; i++) {
    SDL_RWseek(stream, MorphTargetOffset + MorphTargetInfoSize * i,
               RW_SEEK_SET);
    SDL_RWseek(stream, 4, RW_SEEK_CUR);
    result->MorphVertexCount += SDL_ReadLE32(stream);
  }

  result->MorphVertexBuffers = (MorphVertexBuffer*)calloc(
      result->MorphVertexCount, sizeof(MorphVertexBuffer));

  uint32_t CurrentMorphVertexOffset = 0;
  // Read morph targets
  for (uint32_t i = 0; i < result->MorphTargetCount; i++) {
    MorphTarget* target = &result->MorphTargets[i];
    target->VertexOffset = CurrentMorphVertexOffset;
    SDL_RWseek(stream, MorphTargetOffset + MorphTargetInfoSize * i,
               RW_SEEK_SET);
    SDL_RWseek(stream, 4, RW_SEEK_CUR);
    target->VertexCount = SDL_ReadLE32(stream);
    SDL_RWseek(stream, 4, RW_SEEK_CUR);
    uint32_t RawMorphVertexOffset = MorphVertexOffset + SDL_ReadLE32(stream);

    // Read vertex buffers
    SDL_RWseek(stream, RawMorphVertexOffset, RW_SEEK_SET);
    // Per morph vertex buffer: Position, then Normal
    ReadArrayFloatLE32(
        (float*)&result->MorphVertexBuffers[CurrentMorphVertexOffset], stream,
        3 * 2 * target->VertexCount);
    CurrentMorphVertexOffset += target->VertexCount;
  }

  // Read textures
  SDL_RWseek(stream, TexturesOffset, RW_SEEK_SET);
  for (uint32_t i = 0; i < result->TextureCount; i++) {
    uint32_t size = SDL_ReadLE32(stream);
    ImpLogSlow(LL_Debug, LC_ModelLoad, "Loading texture %d, size=0x%08x\n", i,
               size);
    void* gxt = malloc(size);
    SDL_RWread(stream, gxt, 1, size);
    SDL_RWops* gxtStream = SDL_RWFromConstMem(gxt, size);
    if (!TextureLoadGXT(gxtStream, &result->Textures[i])) {
      ImpLog(LL_Debug, LC_ModelLoad,
             "Texture %d failed to load, falling back to 1x1 pixel\n", i);
      result->Textures[i].Load1x1();
    }
    SDL_RWclose(gxtStream);
    free(gxt);
  }

  if (stream) {
    SDL_RWclose(stream);
  }
  if (file) {
    free(file);
  }

  // Animations

  // TODO remove listing, see above

  g_AnimationCount = 0;

  uint32_t iterator;
  VfsFileInfo animFileInfo;
  err = modelArchive->EnumerateStart(&iterator, &animFileInfo);
  while (err == IoError_OK) {
    if (animFileInfo.Id != 0 &&
        !AnimationIsBlacklisted(modelId, animFileInfo.Id)) {
      int64_t animSize;
      void* animData;
      modelArchive->Slurp(animFileInfo.Id, &animData, &animSize);
      SDL_RWops* animStream = SDL_RWFromConstMem(animData, animSize);
      result->Animations[animFileInfo.Id] =
          Animation::Load(animStream, result, animFileInfo.Id);
      SDL_RWclose(animStream);
      free(animData);

      g_AnimationCount++;
    }
    err = modelArchive->EnumerateNext(&iterator, &animFileInfo);
  }

  uint32_t currentAnim = 0;

  g_AnimationIds = (uint32_t*)malloc(g_AnimationCount * sizeof(uint32_t));
  g_AnimationNames = (char**)malloc(g_AnimationCount * sizeof(char*));

  err = modelArchive->EnumerateStart(&iterator, &animFileInfo);
  while (err == IoError_OK) {
    if (animFileInfo.Id != 0 &&
        !AnimationIsBlacklisted(modelId, animFileInfo.Id)) {
      g_AnimationIds[currentAnim] = animFileInfo.Id;
      g_AnimationNames[currentAnim] = strdup(animFileInfo.Name);
      currentAnim++;
    }
    err = modelArchive->EnumerateNext(&iterator, &animFileInfo);
  }

  delete modelArchive;

  return result;
}

Model* Model::MakePlane() {
  Model* result = new Model;
  result->Id = -1;
  result->MeshCount = 1;
  result->VertexCount = 4;
  result->IndexCount = 6;
  result->TextureCount = 1;
  result->BoneCount = 1;
  result->RootBoneCount = 1;

  result->VertexBuffers = (VertexBuffer*)malloc(4 * sizeof(VertexBuffer));
  result->Indices = (uint16_t*)malloc(6 * sizeof(uint16_t));

  result->VertexBuffers[0].Position = glm::vec3(-20.0f, 0.0f, -20.0f);
  result->VertexBuffers[1].Position = glm::vec3(20.0f, 0.0f, -20.0f);
  result->VertexBuffers[2].Position = glm::vec3(-20.0f, 0.0f, 20.0f);
  result->VertexBuffers[3].Position = glm::vec3(20.0f, 0.0f, 20.0f);

  result->VertexBuffers[0].Normal = result->VertexBuffers[1].Normal =
      result->VertexBuffers[2].Normal = result->VertexBuffers[3].Normal =
          glm::vec3(0.0f, 1.0f, 0.0f);

  result->VertexBuffers[0].UV = glm::vec2(0.0f, 0.0f);
  result->VertexBuffers[1].UV = glm::vec2(1.0f, 0.0f);
  result->VertexBuffers[2].UV = glm::vec2(0.0f, 1.0f);
  result->VertexBuffers[3].UV = glm::vec2(1.0f, 1.0f);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) result->VertexBuffers[i].BoneIndices[j] = 0;
    result->VertexBuffers[i].BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  }

  result->Indices[0] = 0;
  result->Indices[1] = 2;
  result->Indices[2] = 1;
  result->Indices[3] = 1;
  result->Indices[4] = 2;
  result->Indices[5] = 3;

  result->Bones[0].Id = 0;
  result->Bones[0].Parent = -1;
  result->Bones[0].ChildrenCount = 0;
  result->Bones[0].BindInverse = glm::mat4(1.0f);
  result->Bones[0].BaseTransform = Transform();

  result->RootBones[0] = 0;

  result->Textures[0].LoadPoliticalCompass();

  result->Meshes[0].BoneMap[0] = 0;
  result->Meshes[0].VertexCount = 4;
  result->Meshes[0].IndexCount = 6;
  result->Meshes[0].VertexOffset = 0;
  result->Meshes[0].IndexOffset = 0;
  result->Meshes[0].Opacity = 1.0f;
  result->Meshes[0].UsedBones = 0;
  result->Meshes[0].Maps[TT_ColorMap] = 0;
  for (int i = TT_ColorMap + 1; i < TT_Count; i++) {
    result->Meshes[0].Maps[i] = -1;
  }
  result->Meshes[0].MeshBone = 0;

  return result;
}

}  // namespace Impacto