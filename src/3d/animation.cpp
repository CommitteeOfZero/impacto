#include <vector>

#include "animation.h"

#include "../io/io.h"
#include "../log.h"
#include "../util.h"

namespace Impacto {

int const HeaderSize = 0x30;
int const HeaderDurationOffset = 0x24;
int const TrackSize = 0xE8;
int const TrackCountsOffset = 6;
int const TrackOffsetsOffset = 0x68;

float const fileFrameTime = 30.0f;

enum TargetType { TargetType_Bone = 0, TargetType_MeshGroup = 0x8000 };
enum SubTrackType {
  STT_Visibility = 0,
  STT_TranslateX = 1,
  STT_TranslateY = 2,
  STT_TranslateZ = 3,
  STT_RotateX = 4,
  STT_RotateY = 5,
  STT_RotateZ = 6,
  STT_ScaleX = 7,
  STT_ScaleY = 8,
  STT_ScaleZ = 9,
};

Animation* Animation::Load(SDL_RWops* stream, Model* model, uint16_t animId) {
  ImpLogSlow(LL_Trace, LC_ModelLoad, "Loading animation %hu for model %d\n",
             animId, model->Id);

  Animation* result = new Animation;
  result->Id = animId;

  SDL_RWseek(stream, HeaderDurationOffset, RW_SEEK_SET);
  result->Duration = ReadFloatLE32(stream) / fileFrameTime;
  uint32_t trackCount = SDL_ReadLE32(stream);
  uint32_t tracksOffset = SDL_ReadLE32(stream);

  ImpLogSlow(LL_Trace, LC_ModelLoad,
             "Duration (s): %f, track count: %d, tracksOffset: %08x\n",
             result->Duration, trackCount, tracksOffset);

  // Offset into result->CoordKeyframes[]
  int currentCoordOffset = 0;

  // Only get coord track counts/offsets and other simple data
  for (uint32_t i = 0; i < trackCount; i++) {
    ImpLogSlow(LL_Trace, LC_ModelLoad, "Pass 1 for track %d\n", i);

    SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
    uint16_t id = SDL_ReadLE16(stream);
    uint16_t targetType = SDL_ReadLE16(stream);
    if (targetType == TargetType_Bone) {
      ImpLogSlow(LL_Trace, LC_ModelLoad, "Track %d is bone %d\n", i, id);

      BoneTrack* track = &result->BoneTracks[result->BoneTrackCount];

      track->Bone = id;

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      // Skip id, targetType, unknown ushort and visibility
      SDL_RWseek(stream, 4 * sizeof(uint16_t), RW_SEEK_CUR);

      // Read coord offsets/counts, so far, so normal...

      ReadArrayLE16(track->KeyCounts + BKT_TranslateX, stream,
                    BKT_Rotate - BKT_TranslateX);
      for (int j = BKT_TranslateX; j < BKT_Rotate; j++) {
        track->KeyOffsets[j] = currentCoordOffset;
        currentCoordOffset += track->KeyCounts[j];
        ImpLogSlow(LL_Trace, LC_ModelLoad,
                   "Subtrack %d: count %d offset %08x\n", j,
                   track->KeyCounts[j], track->KeyOffsets[j]);
      }

      // Skip rotates
      SDL_RWseek(stream, 3 * sizeof(uint16_t), RW_SEEK_CUR);

      ReadArrayLE16(track->KeyCounts + BKT_ScaleX, stream,
                    BKT_Count - BKT_ScaleX);
      for (int j = BKT_ScaleX; j < BKT_Count; j++) {
        track->KeyOffsets[j] = currentCoordOffset;
        currentCoordOffset += track->KeyCounts[j];
        ImpLogSlow(LL_Trace, LC_ModelLoad,
                   "Subtrack %d: count %d offset %08x\n", j,
                   track->KeyCounts[j], track->KeyOffsets[j]);
      }

      result->BoneTrackCount++;
    } else {
      ImpLogSlow(LL_Trace, LC_ModelLoad, "Track %d is mesh group %d\n", i, id);

      // Mesh group track
      std::vector<Mesh*> meshes;
      for (int j = 0; j < model->MeshCount; j++) {
        if (model->Meshes[j].GroupId == id) meshes.push_back(&model->Meshes[j]);
      }

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      // Skip id, targetType, unknown ushort
      SDL_RWseek(stream, 3 * sizeof(uint16_t), RW_SEEK_CUR);

      uint16_t visibilityCount = SDL_ReadLE16(stream);
      int visibilityOffset = currentCoordOffset;
      currentCoordOffset += visibilityCount;
      ImpLogSlow(LL_Trace, LC_ModelLoad, "Visibility count %d offset %08x\n",
                 visibilityCount, visibilityOffset);

      SDL_RWseek(stream, 0x1E, RW_SEEK_CUR);

      uint16_t morphTargetCount = SDL_ReadLE16(stream);
      ImpLogSlow(LL_Trace, LC_ModelLoad, "Morph target count %d\n",
                 morphTargetCount);
      uint16_t virtualMorphTargetIds[AnimMaxMorphTargetsPerTrack];
      ReadArrayLE16(virtualMorphTargetIds, stream, morphTargetCount);

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, 0x48, RW_SEEK_CUR);

      uint16_t morphInfluenceCounts[AnimMaxMorphTargetsPerTrack];
      int morphInfluenceOffsets[AnimMaxMorphTargetsPerTrack];
      ReadArrayLE16(morphInfluenceCounts, stream, morphTargetCount);
      for (int j = 0; j < morphTargetCount; j++) {
        morphInfluenceOffsets[j] = currentCoordOffset;
        currentCoordOffset += morphInfluenceCounts[j];
        ImpLogSlow(LL_Trace, LC_ModelLoad,
                   "Morph %d: influence count %d offset %08x\n", j,
                   morphInfluenceCounts[j], morphInfluenceOffsets[j]);
      }

      for (int j = 0; j < meshes.size(); j++) {
        Mesh* mesh = meshes[j];
        MeshTrack* track = &result->MeshTracks[result->MeshTrackCount + j];
        track->Mesh = mesh->Id;
        ImpLogSlow(LL_Trace, LC_ModelLoad, "Mesh group %d <= mesh %d\n", id,
                   mesh->Id);

        track->KeyCounts[MKT_Visible] = visibilityCount;
        track->KeyOffsets[MKT_Visible] = visibilityOffset;
        track->MorphTargetCount = morphTargetCount;
        memcpy(track->KeyCounts + MKT_MorphInfluenceStart, morphInfluenceCounts,
               morphTargetCount * sizeof(uint16_t));
        memcpy(track->KeyOffsets + MKT_MorphInfluenceStart,
               morphInfluenceOffsets, morphTargetCount * sizeof(int));
        for (int k = 0; k < morphTargetCount; k++) {
          track->MorphTargetIds[k] =
              mesh->MorphTargetIds[virtualMorphTargetIds[k]];
          ImpLogSlow(LL_Trace, LC_ModelLoad,
                     "Morph %d for this mesh: morph target %d\n", k,
                     mesh->MorphTargetIds[virtualMorphTargetIds[k]]);
        }
      }
      result->MeshTrackCount += meshes.size();
    }
  }

  result->CoordKeyframeCount = currentCoordOffset;
  result->CoordKeyframes = (CoordKeyframe*)malloc(sizeof(CoordKeyframe) *
                                                  result->CoordKeyframeCount);

  ImpLogSlow(LL_Trace, LC_ModelLoad,
             "---\nTotal coord keyframe count: %d\n---\n",
             result->CoordKeyframeCount);

  //
  // Interleave rotations. Fun!
  //
  // Background: We need rotations as quaternions to interpolate between them
  // properly. We generate one quaternion keyframe for every time x, y or z
  // rotation changes. This also means that if e.g. y changes after x, we
  // interpolate across the x rotation first, then the y rotation (though if
  // they change on the same frame, we interpolate them simultaneously). Not
  // ideal, but best we can do without glitches, and for R;NE's animations it
  // looks fairly smooth in practice.
  //
  std::vector<std::vector<QuatKeyframe>> rotationTracks;
  rotationTracks.reserve(result->BoneTrackCount);

  result->QuatKeyframeCount = 0;

  int currentBoneTrack = 0;
  int currentMeshTrack = 0;
  for (uint32_t i = 0; i < trackCount; i++) {
    SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
    uint16_t id = SDL_ReadLE16(stream);
    uint16_t targetType = SDL_ReadLE16(stream);
    if (targetType == TargetType_Bone) {
      ImpLogSlow(LL_Trace, LC_ModelLoad,
                 "Interleaving rotations track %d bone %d\n", i, id);
      BoneTrack* track = &result->BoneTracks[currentBoneTrack];

      std::vector<QuatKeyframe> rotationTrack;

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, TrackCountsOffset + STT_RotateX * sizeof(uint16_t),
                 RW_SEEK_CUR);

      uint16_t rawRotXCount = SDL_ReadLE16(stream);
      uint16_t rawRotYCount = SDL_ReadLE16(stream);
      uint16_t rawRotZCount = SDL_ReadLE16(stream);

      ImpLogSlow(LL_Trace, LC_ModelLoad,
                 "rawRotXCount=%d, rawRotYCount=%d, rawRotZCount=%d\n",
                 rawRotXCount, rawRotYCount, rawRotZCount);
      rotationTrack.reserve(rawRotXCount + rawRotYCount + rawRotZCount);

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, TrackOffsetsOffset + STT_RotateX * sizeof(uint32_t),
                 RW_SEEK_CUR);

      int rawRotXOffset = SDL_ReadLE32(stream) + HeaderSize;
      int rawRotYOffset = SDL_ReadLE32(stream) + HeaderSize;
      int rawRotZOffset = SDL_ReadLE32(stream) + HeaderSize;

      ImpLogSlow(LL_Trace, LC_ModelLoad,
                 "rawRotXOffset=%d, rawRotYOffset=%d, rawRotZOffset=%d\n",
                 rawRotXOffset, rawRotYOffset, rawRotZOffset);

      CoordKeyframe* rotXBuffer =
          (CoordKeyframe*)malloc(sizeof(CoordKeyframe) * rawRotXCount);
      CoordKeyframe* rotYBuffer =
          (CoordKeyframe*)malloc(sizeof(CoordKeyframe) * rawRotYCount);
      CoordKeyframe* rotZBuffer =
          (CoordKeyframe*)malloc(sizeof(CoordKeyframe) * rawRotZCount);
      SDL_RWseek(stream, rawRotXOffset, RW_SEEK_SET);
      ReadArrayFloatLE32((float*)rotXBuffer, stream, 2 * rawRotXCount);
      SDL_RWseek(stream, rawRotYOffset, RW_SEEK_SET);
      ReadArrayFloatLE32((float*)rotYBuffer, stream, 2 * rawRotYCount);
      SDL_RWseek(stream, rawRotZOffset, RW_SEEK_SET);
      ReadArrayFloatLE32((float*)rotZBuffer, stream, 2 * rawRotZCount);

      glm::vec3 currentEuler = glm::vec3(0.0f);
      // The defaults are 0, not BaseRotation, yes, really...

      uint16_t nextX = 0;
      uint16_t nextY = 0;
      uint16_t nextZ = 0;

      float currentTime = -1;

      while (nextX < rawRotXCount || nextY < rawRotYCount ||
             nextZ < rawRotZCount) {
        float nextTime = FLT_MAX;

        // Find lowest next time
        if (nextX < rawRotXCount) {
          if (rotXBuffer[nextX].Time > currentTime &&
              rotXBuffer[nextX].Time < nextTime) {
            nextTime = rotXBuffer[nextX].Time;
          }
        }
        if (nextY < rawRotYCount) {
          if (rotYBuffer[nextY].Time > currentTime &&
              rotYBuffer[nextY].Time < nextTime) {
            nextTime = rotYBuffer[nextY].Time;
          }
        }
        if (nextZ < rawRotZCount) {
          if (rotZBuffer[nextZ].Time > currentTime &&
              rotZBuffer[nextZ].Time < nextTime) {
            nextTime = rotZBuffer[nextZ].Time;
          }
        }

        // Advance each series to that time
        if (nextX < rawRotXCount) {
          if (rotXBuffer[nextX].Time == nextTime) {
            currentEuler.x = rotXBuffer[nextX].Value;
            nextX++;
          }
        }
        if (nextY < rawRotYCount) {
          if (rotYBuffer[nextY].Time == nextTime) {
            currentEuler.y = rotYBuffer[nextY].Value;
            nextY++;
          }
        }
        if (nextZ < rawRotZCount) {
          if (rotZBuffer[nextZ].Time == nextTime) {
            currentEuler.z = rotZBuffer[nextZ].Value;
            nextZ++;
          }
        }

        currentTime = nextTime;
        QuatKeyframe key;
        key.Time = currentTime / fileFrameTime;
        eulerZYXToQuat(&currentEuler, &key.Value);
        rotationTrack.push_back(key);

        ImpLogSlow(LL_Trace, LC_ModelLoad,
                   "QuatKeyframe with z=%f, y=%f, x=%f - nextX=%d, nextY=%d, "
                   "nextZ=%d - currentTime=%f\n",
                   currentEuler.z, currentEuler.y, currentEuler.x, nextX, nextY,
                   nextZ, currentTime);
      }

      free(rotXBuffer);
      free(rotYBuffer);
      free(rotZBuffer);

      rotationTrack.shrink_to_fit();
      rotationTracks.push_back(rotationTrack);
      track->KeyCounts[BKT_Rotate] = rotationTrack.size();
      track->KeyOffsets[BKT_Rotate] = result->QuatKeyframeCount;
      result->QuatKeyframeCount += track->KeyCounts[BKT_Rotate];

      ImpLogSlow(LL_Trace, LC_ModelLoad, "QuatKeyframe count %d offset %08x\n",
                 track->KeyCounts[BKT_Rotate], track->KeyOffsets[BKT_Rotate]);

      currentBoneTrack++;
    }
  }

  result->QuatKeyframes =
      (QuatKeyframe*)malloc(sizeof(QuatKeyframe) * result->QuatKeyframeCount);

  ImpLogSlow(LL_Trace, LC_ModelLoad,
             "---\nTotal quat keyframe count: %d\n---\n",
             result->QuatKeyframeCount);

  // Now that we have the buffers, fill with data

  currentBoneTrack = 0;
  currentCoordOffset = 0;
  for (uint32_t i = 0; i < trackCount; i++) {
    SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
    uint16_t id = SDL_ReadLE16(stream);
    uint16_t targetType = SDL_ReadLE16(stream);
    if (targetType == 0) {
      BoneTrack* track = &result->BoneTracks[currentBoneTrack];

      // Coords
      for (int j = STT_Visibility; j <= STT_ScaleZ; j++) {
        if (j == STT_Visibility || j == STT_RotateX || j == STT_RotateY ||
            j == STT_RotateZ)
          continue;

        SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
        SDL_RWseek(stream, TrackCountsOffset + sizeof(uint16_t) * j,
                   RW_SEEK_CUR);
        uint16_t currentKeyframeCount = SDL_ReadLE16(stream);
        SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
        SDL_RWseek(stream, TrackOffsetsOffset + sizeof(uint32_t) * j,
                   RW_SEEK_CUR);
        uint32_t currentKeyframeOffset = SDL_ReadLE32(stream);

        SDL_RWseek(stream, HeaderSize + currentKeyframeOffset, RW_SEEK_SET);

        ReadArrayFloatLE32(
            (float*)(result->CoordKeyframes + currentCoordOffset), stream,
            2 * currentKeyframeCount);
        for (float* time =
                 (float*)(result->CoordKeyframes + currentCoordOffset);
             time < (float*)(result->CoordKeyframes + currentCoordOffset +
                             currentKeyframeCount);
             time += 2) {
          *time /= fileFrameTime;
        }
        currentCoordOffset += currentKeyframeCount;
      }

      // Already have rotate keyframe data
      QuatKeyframe* quatSrc = rotationTracks[currentBoneTrack].data();
      memcpy(result->QuatKeyframes + track->KeyOffsets[BKT_Rotate], quatSrc,
             track->KeyCounts[BKT_Rotate] * sizeof(QuatKeyframe));

      // Animator currently needs this
      for (int j = BKT_TranslateX; j < BKT_Count; j++) {
        if (track->KeyCounts[j]) {
          if (j == BKT_Rotate) {
            assert(result->QuatKeyframes[track->KeyOffsets[j]].Time == 0.0f);
          } else {
            assert(result->CoordKeyframes[track->KeyOffsets[j]].Time == 0.0f);
          }
        }
      }

      currentBoneTrack++;
    } else {
      // Mesh group track
      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      // Skip id, targetType, unknown ushort
      SDL_RWseek(stream, TrackCountsOffset, RW_SEEK_CUR);

      uint16_t visibilityCount = SDL_ReadLE16(stream);

      SDL_RWseek(stream, 0x1E, RW_SEEK_CUR);
      uint16_t morphTargetCount = SDL_ReadLE16(stream);

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, 0x48, RW_SEEK_CUR);

      uint16_t morphInfluenceCounts[AnimMaxMorphTargetsPerTrack];
      ReadArrayLE16(morphInfluenceCounts, stream, morphTargetCount);

      // Visibility data
      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, TrackOffsetsOffset, RW_SEEK_CUR);
      int rawVisibilityOffset = SDL_ReadLE32(stream);
      SDL_RWseek(stream, HeaderSize + rawVisibilityOffset, RW_SEEK_SET);

      ReadArrayFloatLE32((float*)(result->CoordKeyframes + currentCoordOffset),
                         stream, 2 * visibilityCount);
      for (float* time = (float*)(result->CoordKeyframes + currentCoordOffset);
           time < (float*)(result->CoordKeyframes + currentCoordOffset +
                           visibilityCount);
           time += 2) {
        *time /= fileFrameTime;
      }
      currentCoordOffset += visibilityCount;

      // Morph influence data
      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, 0xA8, RW_SEEK_CUR);
      int rawInfluenceOffsets[16];
      ReadArrayLE32(rawInfluenceOffsets, stream, morphTargetCount);
      for (int j = 0; j < morphTargetCount; j++) {
        SDL_RWseek(stream, HeaderSize + rawInfluenceOffsets[j], RW_SEEK_SET);
        ReadArrayFloatLE32(
            (float*)(result->CoordKeyframes + currentCoordOffset), stream,
            morphInfluenceCounts[j] * 2);
        for (CoordKeyframe* key = result->CoordKeyframes + currentCoordOffset;
             key < (result->CoordKeyframes + currentCoordOffset +
                    morphInfluenceCounts[j]);
             key++) {
          key->Time /= fileFrameTime;
          key->Value /= 100.0f;
        }
        currentCoordOffset += morphInfluenceCounts[j];
      }
    }
  }

  return result;
}

Animation::~Animation() {
  if (CoordKeyframes) free(CoordKeyframes);
  if (QuatKeyframes) free(QuatKeyframes);
}

}  // namespace Impacto