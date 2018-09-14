#include "model.h"
#include "io/vfs.h"
#include "io/io.h"
#include "log.h"
#include "texture/gxtloader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Impacto {

static VfsArchive* ModelArchive = NULL;

const uint32_t ModelFileCountsOffset = 0x24;
const uint32_t ModelFileHeaderSize = 0x54;
const uint32_t ModelFileMeshInfoSize = 0x18C;
const uint32_t ModelFileBoneSize = 0x1D0;
const uint32_t MorphTargetInfoSize = 16;
const uint32_t BoneBaseTransformOffset = 0x11C;

void Model::Init() {
  assert(ModelArchive == NULL);
  ImpLog(LL_Info, LC_ModelLoad, "Initializing model loader\n");
  IoError err = VfsArchive::Mount("model.mpk", &ModelArchive);
  if (err != IoError_OK) {
    ModelArchive = NULL;
    ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive: %d\n", err);
  }
}

Model::~Model() {
  if (VertexBuffers) free(VertexBuffers);
  if (MorphTargets) free(MorphTargets);
  if (MorphVertexBuffers) free(MorphVertexBuffers);
  if (Indices) free(Indices);
  if (Bones) free(Bones);
}

Model* Model::Load(uint32_t modelId) {
  assert(ModelArchive != NULL);
  ImpLog(LL_Debug, LC_ModelLoad, "Loading model %d\n", modelId);

  void* file;
  int64_t fileSize;
  IoError err = ModelArchive->Slurp(modelId, &file, &fileSize);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not read model file for %d\n",
           modelId);
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
  result->Bones = (StaticBone*)calloc(result->BoneCount, sizeof(StaticBone));
  result->TextureCount = SDL_ReadLE32(stream);
  assert(result->TextureCount <= ModelMaxTexturesPerModel);
  result->MorphTargetCount = SDL_ReadLE32(stream);
  result->MorphTargets =
      (MorphTarget*)calloc(result->MorphTargetCount, sizeof(MorphTarget));

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
    mesh->ColorMap = SDL_ReadLE16(stream);
    mesh->DecalMap = SDL_ReadLE16(stream);
    mesh->RimlightMap = SDL_ReadLE16(stream);
    mesh->FourthMap = SDL_ReadLE16(stream);
    mesh->SpecularColorMap = SDL_ReadLE16(stream);
    mesh->SixthMap = SDL_ReadLE16(stream);

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
      ReadVec3LE32(&vertex->Position, stream);
      ReadVec3LE32(&vertex->Normal, stream);

      // Attention: we are switching the coordinates
      glm::vec2 _uv = glm::vec2(ReadFloatLE32(stream), ReadFloatLE32(stream));
      vertex->UV = glm::vec2(_uv.y, _uv.x);

      if (mesh->UsedBones > 0) {
        vertex->BoneIndices[0] = SDL_ReadU8(stream);
        vertex->BoneIndices[1] = SDL_ReadU8(stream);
        vertex->BoneIndices[2] = SDL_ReadU8(stream);
        vertex->BoneIndices[3] = SDL_ReadU8(stream);

        ReadVec4LE32(&vertex->BoneWeights, stream);
      } else {
        // TODO skinned/unskinned mesh distinction?
        vertex->BoneIndices[0] = 0;
        vertex->BoneIndices[1] = 0;
        vertex->BoneIndices[2] = 0;
        vertex->BoneIndices[3] = 0;

        vertex->BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

        SDL_RWseek(stream, 4 * sizeof(uint8_t) + 4 * sizeof(float),
                   RW_SEEK_CUR);
      }
    }

    // Read indices
    SDL_RWseek(stream, RawIndexOffset, RW_SEEK_SET);
    for (uint32_t j = 0; j < mesh->IndexCount; j++) {
      result->Indices[CurrentIndexOffset] = SDL_ReadLE16(stream);
      CurrentIndexOffset++;
    }
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
    for (uint32_t j = 0; j < target->VertexCount; j++) {
      MorphVertexBuffer* buffer =
          &result->MorphVertexBuffers[CurrentMorphVertexOffset];
      CurrentMorphVertexOffset++;
      ReadVec3LE32(&buffer->Position, stream);
      ReadVec3LE32(&buffer->Normal, stream);
    }
  }

  // Read textures
  SDL_RWseek(stream, TexturesOffset, RW_SEEK_SET);
  for (uint32_t i = 0; i < result->TextureCount; i++) {
    uint32_t size = SDL_ReadLE32(stream);
    ImpLog(LL_Debug, LC_ModelLoad, "Loading texture %d, size=0x%08x\n", i,
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

  return result;
}

}  // namespace Impacto