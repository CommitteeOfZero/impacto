#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <vector>

#include "animation.h"

#include "../io/io.h"
#include "../log.h"

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

Animation* Animation::Load(SDL_RWops* stream, Model* model, uint16_t id) {
  ImpLog(LL_Debug, LC_ModelLoad, "Loading animation %hu for model %d\n", id,
         model->Id);

  Animation* result = new Animation;
  result->Id = id;

  SDL_RWseek(stream, HeaderDurationOffset, RW_SEEK_SET);
  result->Duration = ReadFloatLE32(stream) / fileFrameTime;
  uint32_t trackCount = SDL_ReadLE32(stream);
  uint32_t tracksOffset = SDL_ReadLE32(stream);

  // Offset into result->CoordKeyframes[]
  int currentCoordOffset = 0;

  // Only get coord track counts/offsets and other simple data
  for (uint32_t i = 0; i < trackCount; i++) {
    SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
    uint16_t id = SDL_ReadLE16(stream);
    uint16_t targetType = SDL_ReadLE16(stream);
    if (targetType == TargetType_Bone) {
      BoneTrack* track = &result->BoneTracks[result->BoneTrackCount];

      track->Bone = id;

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      // Skip id, targetType, unknown ushort and visibility
      SDL_RWseek(stream, 4 * sizeof(uint16_t), RW_SEEK_CUR);

      // Read coord offsets/counts, so far, so normal...

      for (int j = BKT_TranslateX; j < BKT_Rotate; j++) {
        track->KeyOffsets[j] = currentCoordOffset;
        track->KeyCounts[j] = SDL_ReadLE16(stream);
        currentCoordOffset += track->KeyCounts[j];
      }

      // Skip rotates
      SDL_RWseek(stream, 3 * sizeof(uint16_t), RW_SEEK_CUR);

      for (int j = BKT_ScaleX; j < BKT_Count; j++) {
        track->KeyOffsets[j] = currentCoordOffset;
        track->KeyCounts[j] = SDL_ReadLE16(stream);
        currentCoordOffset += track->KeyCounts[j];
      }

      result->BoneTrackCount++;
    } else {
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

      SDL_RWseek(stream, 0x1E, RW_SEEK_CUR);

      uint16_t morphTargetCount = SDL_ReadLE16(stream);

      uint16_t virtualMorphTargetIds[AnimMaxMorphTargetsPerTrack];
      for (int j = 0; j < morphTargetCount; j++) {
        virtualMorphTargetIds[j] = SDL_ReadLE16(stream);
      }

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, 0x48, RW_SEEK_CUR);

      uint16_t morphInfluenceCounts[AnimMaxMorphTargetsPerTrack];
      int morphInfluenceOffsets[AnimMaxMorphTargetsPerTrack];
      for (int j = 0; j < morphTargetCount; j++) {
        morphInfluenceCounts[j] = SDL_ReadLE16(stream);
        morphInfluenceOffsets[j] = currentCoordOffset;
        currentCoordOffset += morphInfluenceCounts[j];
      }

      for (int j = 0; j < meshes.size(); j++) {
        Mesh* mesh = meshes[j];
        MeshTrack* track = &result->MeshTracks[result->MeshTrackCount + j];
        track->Mesh = mesh->Id;

        track->KeyCounts[MKT_Visible] = visibilityCount;
        track->KeyOffsets[MKT_Visible] = visibilityOffset;
        track->MorphTargetCount = morphTargetCount;
        for (int k = 0; k < morphTargetCount; k++) {
          track->KeyCounts[MKT_MorphInfluenceStart + k] =
              morphInfluenceCounts[k];
          track->KeyOffsets[MKT_MorphInfluenceStart + k] =
              morphInfluenceOffsets[k];
          track->MorphTargetIds[k] =
              mesh->MorphTargetIds[virtualMorphTargetIds[k]];
        }
      }
      result->MeshTrackCount += meshes.size();
    }
  }

  result->CoordKeyframeCount = currentCoordOffset;
  result->CoordKeyframes = (CoordKeyframe*)malloc(sizeof(CoordKeyframe) *
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
      BoneTrack* track = &result->BoneTracks[currentBoneTrack];

      std::vector<QuatKeyframe> rotationTrack;

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, TrackCountsOffset + STT_RotateX * sizeof(uint16_t),
                 RW_SEEK_CUR);

      uint16_t rawRotXCount = SDL_ReadLE16(stream);
      uint16_t rawRotYCount = SDL_ReadLE16(stream);
      uint16_t rawRotZCount = SDL_ReadLE16(stream);

      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, TrackOffsetsOffset + STT_RotateX * sizeof(uint32_t),
                 RW_SEEK_CUR);

      int rawRotXOffset = SDL_ReadLE32(stream) + HeaderSize;
      int rawRotYOffset = SDL_ReadLE32(stream) + HeaderSize;
      int rawRotZOffset = SDL_ReadLE32(stream) + HeaderSize;

      float currentXValue = 0.0f, currentYValue = 0.0f, currentZValue = 0.0f;
      // The defaults are 0, not BaseRotation, yes, really...

      uint16_t nextX = 0;
      uint16_t nextY = 0;
      uint16_t nextZ = 0;

      int currentTime = -1;

      int nextXTime, nextYTime, nextZTime;
      float nextXValue, nextYValue, nextZValue;

      while (nextX < rawRotXCount || nextY < rawRotYCount ||
             nextZ < rawRotZCount) {
        int nextTime = INT_MAX;

        // Find lowest next time
        if (nextX < rawRotXCount) {
          SDL_RWseek(stream, rawRotXOffset + nextX * (2 * sizeof(float)),
                     RW_SEEK_SET);
          nextXTime = (int)ReadFloatLE32(stream);
          nextXValue = ReadFloatLE32(stream);
          if (nextXTime > currentTime && nextXTime < nextTime) {
            nextTime = nextXTime;
          }
        }
        if (nextY < rawRotYCount) {
          SDL_RWseek(stream, rawRotYOffset + nextY * (2 * sizeof(float)),
                     RW_SEEK_SET);
          nextYTime = (int)ReadFloatLE32(stream);
          nextYValue = ReadFloatLE32(stream);
          if (nextYTime > currentTime && nextYTime < nextTime) {
            nextTime = nextYTime;
          }
        }
        if (nextZ < rawRotZCount) {
          SDL_RWseek(stream, rawRotZOffset + nextZ * (2 * sizeof(float)),
                     RW_SEEK_SET);
          nextZTime = (int)ReadFloatLE32(stream);
          nextZValue = ReadFloatLE32(stream);
          if (nextZTime > currentTime && nextZTime < nextTime) {
            nextTime = nextZTime;
          }
        }

        // Advance each series to that time
        if (nextX < rawRotXCount) {
          if (nextXTime == nextTime) {
            currentXValue = nextXValue;
            nextX++;
          }
        }
        if (nextY < rawRotYCount) {
          if (nextYTime == nextTime) {
            currentYValue = nextYValue;
            nextY++;
          }
        }
        if (nextZ < rawRotZCount) {
          if (nextZTime == nextTime) {
            currentZValue = nextZValue;
            nextZ++;
          }
        }

        currentTime = nextTime;
        QuatKeyframe key;
        key.Time = (float)currentTime / fileFrameTime;
        key.Value = glm::quat_cast(
            glm::eulerAngleZYX(currentZValue, currentYValue, currentXValue));
        rotationTrack.push_back(key);
      }

      rotationTracks.push_back(rotationTrack);
      track->KeyCounts[BKT_Rotate] = rotationTrack.size();
      track->KeyOffsets[BKT_Rotate] = result->QuatKeyframeCount;
      result->QuatKeyframeCount += track->KeyCounts[BKT_Rotate];

      currentBoneTrack++;
    }
  }

  result->QuatKeyframes =
      (QuatKeyframe*)malloc(sizeof(QuatKeyframe) * result->QuatKeyframeCount);

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

        for (int k = 0; k < currentKeyframeCount; k++) {
          result->CoordKeyframes[currentCoordOffset].Time =
              ReadFloatLE32(stream) / fileFrameTime;
          result->CoordKeyframes[currentCoordOffset].Value =
              ReadFloatLE32(stream);
          currentCoordOffset++;
        }
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
      for (int j = 0; j < morphTargetCount; j++) {
        morphInfluenceCounts[j] = SDL_ReadLE16(stream);
      }

      // Visibility data
      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, TrackOffsetsOffset, RW_SEEK_CUR);
      int rawVisibilityOffset = SDL_ReadLE32(stream);
      SDL_RWseek(stream, HeaderSize + rawVisibilityOffset, RW_SEEK_SET);
      for (int j = 0; j < visibilityCount; j++) {
        result->CoordKeyframes[currentCoordOffset].Time =
            ReadFloatLE32(stream) / fileFrameTime;
        result->CoordKeyframes[currentCoordOffset].Value =
            ReadFloatLE32(stream);
        currentCoordOffset++;
      }

      // Morph influence data
      SDL_RWseek(stream, tracksOffset + TrackSize * i, RW_SEEK_SET);
      SDL_RWseek(stream, 0xA8, RW_SEEK_CUR);
      int rawInfluenceOffsets[16];
      for (int j = 0; j < morphTargetCount; j++) {
        rawInfluenceOffsets[j] = SDL_ReadLE32(stream);
      }
      for (int j = 0; j < morphTargetCount; j++) {
        SDL_RWseek(stream, HeaderSize + rawInfluenceOffsets[j], RW_SEEK_SET);
        for (int k = 0; k < morphInfluenceCounts[j]; k++) {
          result->CoordKeyframes[currentCoordOffset].Time =
              ReadFloatLE32(stream) / fileFrameTime;
          result->CoordKeyframes[currentCoordOffset].Value =
              ReadFloatLE32(stream) / 100.0f;
          currentCoordOffset++;
        }
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