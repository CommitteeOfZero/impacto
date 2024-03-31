#include "model.h"

#include "animation.h"
#include "../../io/vfs.h"
#include "../../io/io.h"
#include "../../log.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../../io/memorystream.h"

#include "../../profile/scene3d.h"

using namespace Impacto::Io;

namespace Impacto {

const uint32_t ModelFileCountsOffset = 0x24;
const uint32_t ModelFileHeaderSize = 0x54;
const uint32_t ModelFileMeshInfoSize = 0x18C;
uint32_t const ModelFileMeshInfoSize_DaSH = 0x1A8;
const uint32_t ModelFileBoneSize = 0x1D0;
uint32_t const ModelFileBoneSize_DaSH = 0x1FC;
const uint32_t MorphTargetInfoSize = 16;
const uint32_t BoneBaseTransformOffset = 0x11C;
uint32_t const BoneBaseTransformOffset_DaSH = 0x13C;
uint32_t const MeshInfoCountsOffset = 0xE0;
uint32_t const MeshInfoCountsOffset_DaSH = 0xF8;

uint32_t* g_ModelIds;
char** g_ModelNames;
uint32_t g_ModelCount;

uint32_t* g_BackgroundModelIds;
char** g_BackgroundModelNames;
uint32_t g_BackgroundModelCount;

bool AnimationIsBlacklisted(uint32_t modelId, int16_t animId) {
  for (auto p : Profile::Scene3D::AnimationParseBlacklist) {
    if (p.first == modelId && p.second == animId) return true;
  }
  return false;
}

void Model::EnumerateModels() {
  // List models
  // TODO: We don't need this in the game - take it out when we remove the model
  // viewer, it's a waste of time then

  g_ModelCount = 0;
  g_BackgroundModelCount = 0;

  std::map<uint32_t, std::string> listing;
  IoError err = VfsListFiles("model", listing);

  for (auto const& file : listing) {
    if (Profile::Scene3D::ModelsToCharacters.count(file.first) != 0) {
      g_ModelCount++;
    } else {
      g_BackgroundModelCount++;
    }
  }

  uint32_t currentModel = 0;
  uint32_t currentBackgroundModel = 0;

  g_ModelIds = (uint32_t*)malloc(g_ModelCount * sizeof(uint32_t));
  g_ModelNames = (char**)malloc(g_ModelCount * sizeof(char*));
  g_BackgroundModelIds =
      (uint32_t*)malloc(g_BackgroundModelCount * sizeof(uint32_t));
  g_BackgroundModelNames =
      (char**)malloc(g_BackgroundModelCount * sizeof(char*));

  for (auto const& file : listing) {
    if (Profile::Scene3D::ModelsToCharacters.count(file.first) != 0) {
      g_ModelIds[currentModel] = file.first;
      g_ModelNames[currentModel] = strdup(file.second.c_str());
      currentModel++;
    } else {
      g_BackgroundModelIds[currentBackgroundModel] = file.first;
      g_BackgroundModelNames[currentBackgroundModel] =
          strdup(file.second.c_str());
      currentBackgroundModel++;
    }
  }
}

Model::~Model() {
  if (VertexBuffers) free(VertexBuffers);
  if (MorphVertexBuffers) free(MorphVertexBuffers);
  if (Indices) free(Indices);
  for (auto animation : Animations) {
    if (animation.second) delete animation.second;
  }
  if (AnimationIds) {
    free(AnimationIds);
  }
  if (AnimationNames) {
    for (size_t i = 0; i < AnimationCount; i++) {
      if (AnimationNames[i]) free(AnimationNames[i]);
    }
    free(AnimationNames);
  }
}

static IoError GetModelMountpoint(uint32_t modelId,
                                  std::string& outMountpoint) {
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    outMountpoint = "model";
    return IoError_OK;
  } else {
    FileMeta arcMeta;
    IoError err = VfsGetMeta("model", modelId, &arcMeta);
    if (err != IoError_OK) {
      ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive for %d\n",
             modelId);
      return err;
    }
    outMountpoint = "model_" + arcMeta.FileName;
    return IoError_OK;
  }
}

static IoError MountModel(uint32_t modelId, std::string& outMountpoint) {
  // DaSH doesn't have model archives
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    outMountpoint = "model";
    return IoError_OK;
  }

  FileMeta arcMeta;
  IoError err = VfsGetMeta("model", modelId, &arcMeta);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive for %d\n",
           modelId);
    return err;
  }
  void* arcMem;
  int64_t arcSz;
  err = VfsSlurp("model", modelId, &arcMem, &arcSz);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive for %d\n",
           modelId);
    return err;
  }
  std::string modelMountpoint = "model_" + arcMeta.FileName;
  err = VfsMountMemory(modelMountpoint, arcMeta.FileName, arcMem, arcSz, true);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not open model archive for %d\n",
           modelId);
    free(arcMem);
    return err;
  }

  outMountpoint = modelMountpoint;
  return IoError_OK;
}

static void UnmountModel(uint32_t modelId) {
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) return;

  FileMeta arcMeta;
  IoError err = VfsGetMeta("model", modelId, &arcMeta);
  if (err != IoError_OK) {
    ImpLog(
        LL_Error, LC_ModelLoad,
        "Could not unmount model archive for %d (this should never happen)\n",
        modelId);
    return;
  }
  VfsUnmount("model_" + arcMeta.FileName, arcMeta.FileName);
}

static IoError SlurpModel(uint32_t modelId, void** outMemory,
                          int64_t* outSize) {
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    return VfsSlurp("model", modelId, outMemory, outSize);
  } else {
    std::string mountpoint;
    IoError err = GetModelMountpoint(modelId, mountpoint);
    if (err != IoError_OK) return err;
    return VfsSlurp(mountpoint, 0, outMemory, outSize);
  }
}

static IoError SlurpAnim(uint32_t modelId, int16_t animId, void** outMemory,
                         int64_t* outSize, std::string& outName) {
  std::string mountpoint;
  IoError err;

  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    mountpoint = "motion";
  } else {
    err = GetModelMountpoint(modelId, mountpoint);
    if (err != IoError_OK) return err;
  }

  FileMeta animMeta;
  err = VfsGetMeta(mountpoint, animId, &animMeta);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad,
           "Could not find animation file %h for model %d\n", animId, modelId);
    return err;
  }
  outName = animMeta.FileName;

  return VfsSlurp(mountpoint, animId, outMemory, outSize);
}

Model* Model::Load(uint32_t modelId) {
  uint32_t meshInfoSize, meshInfoCountsOffset, boneSize,
      boneBaseTransformOffset;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    meshInfoSize = ModelFileMeshInfoSize_DaSH;
    meshInfoCountsOffset = MeshInfoCountsOffset_DaSH;
    boneSize = ModelFileBoneSize_DaSH;
    boneBaseTransformOffset = BoneBaseTransformOffset_DaSH;
  } else {
    meshInfoSize = ModelFileMeshInfoSize;
    meshInfoCountsOffset = MeshInfoCountsOffset;
    boneSize = ModelFileBoneSize;
    boneBaseTransformOffset = BoneBaseTransformOffset;
  }

  ImpLogSlow(LL_Debug, LC_ModelLoad, "Loading model %d\n", modelId);

  std::string modelMountpoint;
  IoError err = MountModel(modelId, modelMountpoint);
  if (err != IoError_OK) {
    return NULL;
  }

  void* file;
  int64_t fileSize;
  err = SlurpModel(modelId, &file, &fileSize);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_ModelLoad, "Could not read model file for %d\n",
           modelId);
    UnmountModel(modelId);
    return NULL;
  }

  // We read the whole file at once for speed, but we still want a stream for
  // convenience
  InputStream* stream = new MemoryStream(file, fileSize, true);

  Model* result = new Model;
  result->Id = modelId;
  uint32_t type = ReadLE<uint32_t>(stream);
  if (type == ModelType_DaSH_Unspecified) {
    assert(Profile::Scene3D::Version == +LKMVersion::DaSH);
    if (Profile::Scene3D::ModelsToCharacters.count(modelId) != 0) {
      type = ModelType_Character;
    } else {
      type = ModelType_Background;
    }
  }
  assert(type == ModelType_Background || type == ModelType_Character);
  result->Type = (ModelType)type;

  // Read model resource counts and offsets
  stream->Seek(ModelFileCountsOffset, RW_SEEK_SET);
  result->MeshCount = ReadLE<uint32_t>(stream);
  assert(result->MeshCount <= ModelMaxMeshesPerModel);
  result->BoneCount = ReadLE<uint32_t>(stream);
  if (result->Type == ModelType_Background) {
    // Backgrounds have a skeleton, but it seems to be unused
    result->BoneCount = 0;
  }
  assert(result->BoneCount <= ModelMaxBonesPerModel);
  result->TextureCount = ReadLE<uint32_t>(stream);
  assert(result->TextureCount <= ModelMaxTexturesPerModel);
  result->MorphTargetCount = ReadLE<uint32_t>(stream);
  assert(
      result->MorphTargetCount <= ModelMaxMorphTargetsPerModel &&
      (result->Type == ModelType_Character || result->MorphTargetCount == 0));

  ImpLog(LL_Debug, LC_ModelLoad,
         "Model %d MeshCount=%d BoneCount=%d TextureCount=%d "
         "MorphTargetCount=%d\n",
         modelId, result->MeshCount, result->BoneCount, result->TextureCount,
         result->MorphTargetCount);

  uint32_t MeshInfosOffset = ReadLE<uint32_t>(stream);
  uint32_t BonesOffset = ReadLE<uint32_t>(stream);
  uint32_t TexturesOffset = ReadLE<uint32_t>(stream);
  uint32_t MorphVertexOffset = ReadLE<uint32_t>(stream);
  uint32_t MorphTargetOffset = ReadLE<uint32_t>(stream);

  result->VertexCount = 0;
  result->IndexCount = 0;

  // Accumulate per-mesh vertex buffer / index counts
  for (uint32_t i = 0; i < result->MeshCount; i++) {
    int64_t seekPos = MeshInfosOffset + meshInfoSize * i;
    seekPos += meshInfoCountsOffset;
    stream->Seek(seekPos, RW_SEEK_SET);
    result->VertexCount += ReadLE<int32_t>(stream);
    result->IndexCount += ReadLE<int32_t>(stream);
  }

  if (result->Type == ModelType_Character) {
    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      result->VertexBuffers =
          calloc(result->VertexCount, sizeof(VertexBufferDaSH));
    } else {
      result->VertexBuffers = calloc(result->VertexCount, sizeof(VertexBuffer));
    }
  } else if (result->Type == ModelType_Background) {
    result->VertexBuffers = calloc(result->VertexCount, sizeof(BgVertexBuffer));
  }
  result->Indices = (uint16_t*)calloc(result->IndexCount, sizeof(uint16_t));

  // Read mesh attributes
  for (uint32_t i = 0; i < result->MeshCount; i++) {
    stream->Seek(MeshInfosOffset + meshInfoSize * i, RW_SEEK_SET);
    Mesh* mesh = &result->Meshes[i];

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      stream->Read(mesh->Name, 32);
    } else {
      memset(mesh->Name, 0, sizeof(mesh->Name));
    }

    mesh->Id = i;
    mesh->GroupId = ReadLE<int32_t>(stream);

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      std::string nameStr = std::string((char*)mesh->Name);
      assert(result->NamedMeshGroups.count(nameStr) == 0 ||
             result->NamedMeshGroups[nameStr] == mesh->GroupId ||
             nameStr == "");
      result->NamedMeshGroups[nameStr] = mesh->GroupId;
    }

    mesh->MeshBone = ReadLE<int16_t>(stream);
    stream->Seek(9, RW_SEEK_CUR);
    mesh->MorphTargetCount = ReadU8(stream);
    assert(mesh->MorphTargetCount <= ModelMaxMorphTargetsPerMesh);
    stream->Read(mesh->MorphTargetIds, ModelMaxMorphTargetsPerMesh);

    int64_t seekPos = (Profile::Scene3D::Version == +LKMVersion::DaSH)
                          ? ModelUnknownsAfterMorphTargets_DaSH
                          : ModelUnknownsAfterMorphTargets;
    // Skip translation, rotation and scale (these sometimes don't match the
    // matrix below, which is authoritative)
    seekPos += 3 * sizeof(glm::vec3);
    stream->Seek(seekPos, RW_SEEK_CUR);

    glm::mat4 modelMtx;
    ReadMat4LE(&modelMtx, stream);
    mesh->ModelTransform = Transform(modelMtx);
    // Skip model matrix inverse
    stream->Seek(sizeof(glm::mat4), RW_SEEK_CUR);

    mesh->VertexCount = ReadLE<int32_t>(stream);
    mesh->IndexCount = ReadLE<int32_t>(stream);

    stream->Seek(0x68, RW_SEEK_CUR);
    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      // Don't ask me why there's a float for this
      mesh->HasShadowColorMap = ReadLE<float>(stream) != 0.0f;
    } else {
      stream->Seek(4, RW_SEEK_CUR);
      mesh->HasShadowColorMap = false;
    }
    mesh->Opacity = ReadLE<float>(stream);

    stream->Seek(0x14, RW_SEEK_CUR);
    ReadArrayLE<TT_Count>(mesh->Maps, stream);

    mesh->Flags = ReadLE<uint32_t>(stream);

    // Choose material
    if (result->Type == ModelType_Background) {
      mesh->Material = MT_Background;
    } else if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      stream->Seek(3 * 4, RW_SEEK_CUR);
      mesh->Material = (MaterialType)ReadLE<int>(stream);
      assert(mesh->Material == MT_DaSH_Generic ||
             mesh->Material == MT_DaSH_Eye || mesh->Material == MT_DaSH_Face ||
             mesh->Material == MT_DaSH_Skin);
    } else {
      mesh->Material =
          mesh->Maps[TT_Eye_WhiteColorMap] >= 0 ? MT_Eye : MT_Generic;
    }
  }

  // Read vertex buffers and indices
  int32_t CurrentVertexOffset = 0;
  int32_t CurrentIndexOffset = 0;
  for (uint32_t i = 0; i < result->MeshCount; i++) {
    int64_t seekPos = MeshInfosOffset + meshInfoSize * i;
    Mesh* mesh = &result->Meshes[i];
    mesh->VertexOffset = CurrentVertexOffset;
    mesh->IndexOffset = CurrentIndexOffset;

    seekPos += meshInfoCountsOffset + 4 * sizeof(int);
    stream->Seek(seekPos, RW_SEEK_SET);
    int32_t RawVertexOffset = ModelFileHeaderSize + ReadLE<int32_t>(stream);
    int32_t RawIndexOffset = ModelFileHeaderSize + ReadLE<int32_t>(stream);

    mesh->UsedBones = ReadLE<uint32_t>(stream);
    assert(mesh->UsedBones <= ModelMaxBonesPerMesh);
    ReadArrayLE(mesh->BoneMap, stream, mesh->UsedBones);

    // Read vertex buffers
    stream->Seek(RawVertexOffset, RW_SEEK_SET);
    if (result->Type == ModelType_Character) {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        for (uint32_t j = 0; j < mesh->VertexCount; j++) {
          VertexBufferDaSH* vertex =
              &((VertexBufferDaSH*)result->VertexBuffers)[CurrentVertexOffset];
          CurrentVertexOffset++;
          // Position, then Normal
          ReadArrayLE<3 * 2>((float*)&vertex->Position, stream);
          // vec3 unk1, vec4 unk2
          stream->Seek(7 * sizeof(float), RW_SEEK_CUR);
          // UV
          ReadVec2LE(&vertex->UV, stream);

          if (mesh->UsedBones > 0) {
            stream->Read(vertex->BoneIndices, 4);

            ReadVec4LE(&vertex->BoneWeights, stream);
          } else {
            // TODO skinned/unskinned mesh distinction?
            *(int*)vertex->BoneIndices = 0;

            vertex->BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

            stream->Seek(4 * sizeof(uint8_t) + 4 * sizeof(float), RW_SEEK_CUR);
          }
        }
      } else {
        for (uint32_t j = 0; j < mesh->VertexCount; j++) {
          VertexBuffer* vertex =
              &((VertexBuffer*)result->VertexBuffers)[CurrentVertexOffset];
          CurrentVertexOffset++;
          // Position, then Normal, then UV
          ReadArrayLE<3 * 2 + 2 * 1>((float*)&vertex->Position, stream);

          if (mesh->UsedBones > 0) {
            stream->Read(vertex->BoneIndices, 4);

            ReadVec4LE(&vertex->BoneWeights, stream);
          } else {
            // TODO skinned/unskinned mesh distinction?
            *(int*)vertex->BoneIndices = 0;

            vertex->BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

            stream->Seek(4 * sizeof(uint8_t) + 4 * sizeof(float), RW_SEEK_CUR);
          }
        }
      }
    } else if (result->Type == ModelType_Background) {
      for (uint32_t j = 0; j < mesh->VertexCount; j++) {
        BgVertexBuffer* vertex =
            &((BgVertexBuffer*)result->VertexBuffers)[CurrentVertexOffset];
        CurrentVertexOffset++;
        // Position, then UV
        ReadArrayLE<3 * 1 + 2 * 1>((float*)&vertex->Position, stream);
      }
    }

    // Read indices
    stream->Seek(RawIndexOffset, RW_SEEK_SET);
    ReadArrayLE(result->Indices + CurrentIndexOffset, stream, mesh->IndexCount);
    CurrentIndexOffset += mesh->IndexCount;
  }

  // Read skeleton
  for (uint32_t i = 0; i < result->BoneCount; i++) {
    uint32_t seekPos = BonesOffset + boneSize * i;
    stream->Seek(BonesOffset + boneSize * i, RW_SEEK_SET);
    StaticBone* bone = &result->Bones[i];

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      stream->Read(bone->Name, 32);
    } else {
      memset(bone->Name, 0, sizeof(bone->Name));
    }

    bone->Id = ReadLE<int16_t>(stream);

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      std::string nameStr = std::string((char*)bone->Name);
      assert(result->NamedBones.count(nameStr) == 0 ||
             result->NamedBones[nameStr] == bone->Id || nameStr == "");
      result->NamedBones[nameStr] = bone->Id;
    }

    stream->Seek(2, RW_SEEK_CUR);
    bone->Parent = ReadLE<int16_t>(stream);
    if (bone->Parent < 0) {
      result->RootBones[result->RootBoneCount] = i;
      result->RootBoneCount++;
      assert(result->RootBoneCount < ModelMaxRootBones);
    }
    stream->Seek(8, RW_SEEK_CUR);
    bone->ChildrenCount = ReadLE<int16_t>(stream);
    assert(bone->ChildrenCount <= ModelMaxChildrenPerBone);

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      stream->Seek(2, RW_SEEK_CUR);
    }

    ReadArrayLE(bone->Children, stream, bone->ChildrenCount);
    stream->Seek(BonesOffset + boneSize * i + boneBaseTransformOffset,
                 RW_SEEK_SET);

    glm::vec3 position, euler, scale;
    ReadVec3LE(&position, stream);
    ReadVec3LE(&euler, stream);
    ReadVec3LE(&scale, stream);
    // More often than not these are actually not set...
    if (glm::length(scale) < 0.001f) scale = glm::vec3(1.0f);
    bone->BaseTransform = Transform(position, euler, scale);

    // skip over bindpose
    stream->Seek(sizeof(glm::mat4), RW_SEEK_CUR);

    ReadMat4LE(&bone->BindInverse, stream);
  }

  result->MorphVertexCount = 0;
  // Accumulate per-morph target vertex buffer counts
  for (uint32_t i = 0; i < result->MorphTargetCount; i++) {
    int64_t seekPos = MorphTargetOffset + MorphTargetInfoSize * i;
    seekPos += 4;
    stream->Seek(seekPos, RW_SEEK_SET);
    result->MorphVertexCount += ReadLE<int32_t>(stream);
  }

  result->MorphVertexBuffers = (MorphVertexBuffer*)calloc(
      result->MorphVertexCount, sizeof(MorphVertexBuffer));

  uint32_t CurrentMorphVertexOffset = 0;
  // Read morph targets
  for (uint32_t i = 0; i < result->MorphTargetCount; i++) {
    MorphTarget* target = &result->MorphTargets[i];
    target->VertexOffset = CurrentMorphVertexOffset;
    int64_t seekPos = MorphTargetOffset + MorphTargetInfoSize * i;
    seekPos += 4;
    stream->Seek(seekPos, RW_SEEK_SET);
    target->VertexCount = ReadLE<int32_t>(stream);
    stream->Seek(4, RW_SEEK_CUR);
    uint32_t RawMorphVertexOffset =
        MorphVertexOffset + ReadLE<uint32_t>(stream);

    // Read vertex buffers
    stream->Seek(RawMorphVertexOffset, RW_SEEK_SET);
    // Per morph vertex buffer: Position, then Normal
    ReadArrayLE((float*)&result->MorphVertexBuffers[CurrentMorphVertexOffset],
                stream, 3 * 2 * target->VertexCount);
    CurrentMorphVertexOffset += target->VertexCount;
  }

  // Read textures
  stream->Seek(TexturesOffset, RW_SEEK_SET);
  for (uint32_t i = 0; i < result->TextureCount; i++) {
    uint32_t size = ReadLE<uint32_t>(stream);
    ImpLogSlow(LL_Debug, LC_ModelLoad, "Loading texture %d, size=0x%08x\n", i,
               size);
    void* gxt = malloc(size);
    stream->Read(gxt, size);
    InputStream* gxtStream = new MemoryStream(gxt, size, true);
    if (!result->Textures[i].Load(gxtStream)) {
      ImpLog(LL_Debug, LC_ModelLoad,
             "Texture %d failed to load, falling back to 1x1 pixel\n", i);
      result->Textures[i].LoadCheckerboard();
    }
    delete gxtStream;
  }

  if (stream) {
    delete stream;
  }

  // Animations

  // TODO remove listing, see above
  // WOW this really shouldn't be here anyway
  // TODO should we still remove this?

  if (result->Type == ModelType_Character) {
    Profile::Scene3D::CharacterDef& charDef = Profile::Scene3D::Characters
        [Profile::Scene3D::ModelsToCharacters[modelId]];

    result->IdleAnimation = charDef.IdleAnimation;
    result->AnimationCount = 0;

    for (auto const& anim : charDef.Animations) {
      auto animId = anim.first;

      if (AnimationIsBlacklisted(modelId, animId)) continue;

      int64_t animSize;
      void* animData;

      if (!AnimationIsBlacklisted(modelId, animId)) {
        std::string animName;
        int64_t animSize;
        void* animData;

        if (SlurpAnim(modelId, animId, &animData, &animSize, animName) !=
            IoError_OK) {
          ImpLog(LL_Error, LC_ModelLoad,
                 "Could not read animation %h for model %d\n", animId, modelId);
          continue;
        }

        InputStream* animStream = new MemoryStream(animData, animSize, true);
        animStream->Meta.FileName = animName;
        ModelAnimation* anim = ModelAnimation::Load(animStream, result, animId);
        delete animStream;
        if (anim == 0) {
          ImpLog(LL_Error, LC_ModelLoad,
                 "Could not parse animation %h for model %d\n", animId,
                 modelId);
          continue;
        }
        assert(result->Animations.count(animId) == 0);
        result->Animations[animId] = anim;
        result->AnimationCount++;
      }
    }

    result->AnimationIds =
        (int32_t*)malloc(sizeof(int32_t) * result->AnimationCount);
    result->AnimationNames =
        (char**)malloc(sizeof(char*) * result->AnimationCount);

    int i = 0;
    for (auto const& anim : result->Animations) {
      result->AnimationIds[i] = anim.first;
      result->AnimationNames[i] = strdup(anim.second->Name.c_str());
      i++;
    }
  }

  UnmountModel(modelId);

  return result;
}

// WARNING: Breaks with DaSH
Model* Model::MakePlane() {
  Model* result = new Model;
  result->Id = -1;
  result->Type = ModelType_Character;
  result->MeshCount = 1;
  result->VertexCount = 4;
  result->IndexCount = 6;
  result->TextureCount = 1;
  result->BoneCount = 1;
  result->RootBoneCount = 1;
  result->MorphTargetCount = 0;

  result->VertexBuffers = (VertexBuffer*)malloc(4 * sizeof(VertexBuffer));
  result->Indices = (uint16_t*)malloc(6 * sizeof(uint16_t));

  VertexBuffer* vertexBuffers = (VertexBuffer*)result->VertexBuffers;

  vertexBuffers[0].Position = glm::vec3(-20.0f, 0.0f, -20.0f);
  vertexBuffers[1].Position = glm::vec3(20.0f, 0.0f, -20.0f);
  vertexBuffers[2].Position = glm::vec3(-20.0f, 0.0f, 20.0f);
  vertexBuffers[3].Position = glm::vec3(20.0f, 0.0f, 20.0f);

  vertexBuffers[0].Normal = vertexBuffers[1].Normal = vertexBuffers[2].Normal =
      vertexBuffers[3].Normal = glm::vec3(0.0f, 1.0f, 0.0f);

  vertexBuffers[0].UV = glm::vec2(0.0f, 0.0f);
  vertexBuffers[1].UV = glm::vec2(1.0f, 0.0f);
  vertexBuffers[2].UV = glm::vec2(0.0f, 1.0f);
  vertexBuffers[3].UV = glm::vec2(1.0f, 1.0f);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) vertexBuffers[i].BoneIndices[j] = 0;
    vertexBuffers[i].BoneWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  }

  result->Indices[0] = 0;
  result->Indices[1] = 2;
  result->Indices[2] = 1;
  result->Indices[3] = 1;
  result->Indices[4] = 2;
  result->Indices[5] = 3;

  memset(result->Bones[0].Name, 0, sizeof(result->Bones[0].Name));
  result->Bones[0].Id = 0;
  result->Bones[0].Parent = -1;
  result->Bones[0].ChildrenCount = 0;
  result->Bones[0].BindInverse = glm::mat4(1.0f);
  result->Bones[0].BaseTransform = Transform();

  result->RootBones[0] = 0;

  result->Textures[0].LoadPoliticalCompass();

  memset(result->Meshes[0].Name, 0, sizeof(result->Meshes[0].Name));
  result->Meshes[0].MorphTargetCount = 0;
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
  result->Meshes[0].Material = MT_Generic;

  return result;
}

}  // namespace Impacto