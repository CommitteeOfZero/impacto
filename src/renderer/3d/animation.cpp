#include <vector>

#include "animation.h"

#include "../../log.h"
#include "../../util.h"

#include "../../profile/scene3d.h"

namespace Impacto {

static int constexpr HeaderSize = 0x30;
static int constexpr HeaderDurationOffset = 0x24;
static int constexpr TrackSize = 0xE8;
static int constexpr TrackSize_DaSH = 0x108;
static int constexpr TrackCountsOffset = 6;
static int constexpr TrackCountsOffset_DaSH = 0x26;
static int constexpr TrackOffsetsOffset = 0x68;
static int constexpr TrackOffsetsOffset_DaSH = 0x88;

enum TargetType {
  TargetType_Bone = 0,
  TargetType_MeshGroup = 0x8000,
  // Our invention
  TargetType_NotFound = 0xFFFF
};
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

using namespace Impacto::Io;

struct Target {
  uint8_t Name[32];
  TargetType Type;
  uint16_t Id;
};

Target GetTarget(Stream* stream, Model* model) {
  Target result;
  result.Type = TargetType_NotFound;

  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    stream->Read(result.Name, 32);

    stream->Seek(2 * sizeof(uint16_t), RW_SEEK_CUR);

    std::string nameStr = std::string((char*)result.Name);
    if (model->NamedBones.count(nameStr) != 0) {
      result.Type = TargetType_Bone;
      result.Id = model->NamedBones[nameStr];
    } else if (model->NamedMeshGroups.count(nameStr) != 0) {
      result.Type = TargetType_MeshGroup;
      result.Id = model->NamedMeshGroups[nameStr];
    }
  } else {
    memset(result.Name, 0, sizeof(result.Name));

    result.Id = ReadLE<uint16_t>(stream);
    uint16_t targetType = ReadLE<uint16_t>(stream);
    assert(targetType == TargetType_Bone || targetType == TargetType_MeshGroup);
    result.Type = (TargetType)targetType;
  }

  return result;
}

ModelAnimation* ModelAnimation::Load(Stream* stream, Model* model,
                                     int16_t animId) {
  int trackSize, trackCountsOffset, trackOffsetsOffset;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    trackSize = TrackSize_DaSH;
    trackCountsOffset = TrackCountsOffset_DaSH;
    trackOffsetsOffset = TrackOffsetsOffset_DaSH;
  } else {
    trackSize = TrackSize;
    trackCountsOffset = TrackCountsOffset;
    trackOffsetsOffset = TrackOffsetsOffset;
  }

  ModelAnimation* result = new ModelAnimation;
  result->Id = animId;
  result->Name = stream->Meta.FileName;

  ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
             "Loading animation {:d} ({:s}) for model {:d}\n", animId,
             result->Name, model->Id);

  stream->Seek(HeaderDurationOffset, RW_SEEK_SET);
  result->Duration =
      ReadLE<float>(stream) / Profile::Scene3D::AnimationDesignFrameRate;
  uint32_t trackCount = ReadLE<uint32_t>(stream);
  uint32_t tracksOffset = ReadLE<uint32_t>(stream);

  ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
             "Duration (s): {:f}, track count: {:d}, tracksOffset: 0x{:08x}\n",
             result->Duration, trackCount, tracksOffset);

  // Offset into result->CoordKeyframes[]
  int currentCoordOffset = 0;

  // Input data is bad and sometimes contains more than one track for the same
  // ID (especially 0). The reference implementation ignores everything after
  // the first track for an ID. We need to do that to, otherwise glitches e.g.
  // on c002_030.
  // TODO: Is this per-track (current implementation) or per-*subtrack*? E.g.
  // can there be a track specifying only TranslateX for a bone and another
  // specifying only TranslateY?
  int32_t TrackForBone[ModelMaxBonesPerModel];
  memset(TrackForBone, 0xFF, sizeof(TrackForBone));

  // Only get coord track counts/offsets and other simple data
  for (uint32_t i = 0; i < trackCount; i++) {
    ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
               "Pass 1 for track {:d}\n", i);

    stream->Seek(tracksOffset + trackSize * i, RW_SEEK_SET);
    Target target = GetTarget(stream, model);
    if (target.Type == TargetType_Bone) {
      if (TrackForBone[target.Id] != -1) {
        ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                   "Skipping duplicate track {:d} for bone {:d}\n", i,
                   target.Id);
        continue;
      }
      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "Track {:d} is bone {:d}\n", i, target.Id);

      BoneTrack* track = &result->BoneTracks[result->BoneTrackCount];
      memcpy(track->Name, target.Name, sizeof(target.Name));

      track->Bone = target.Id;
      TrackForBone[target.Id] = i;

      uint32_t seekPos = tracksOffset + trackSize * i;
      // Skip name
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) seekPos += 32;
      // Skip id, targetType, unknown ushort and visibility
      seekPos += (4 * sizeof(uint16_t));
      stream->Seek(seekPos, RW_SEEK_SET);

      // Read coord offsets/counts, so far, so normal...

      ReadArrayLE<(BKT_Rotate - BKT_TranslateX)>(
          track->KeyCounts + BKT_TranslateX, stream);
      for (int j = BKT_TranslateX; j < BKT_Rotate; j++) {
        track->KeyOffsets[j] = currentCoordOffset;
        currentCoordOffset += track->KeyCounts[j];
        ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                   "Subtrack {:d}: count {:d} offset 0x{:08x}\n", j,
                   track->KeyCounts[j], track->KeyOffsets[j]);
      }

      // Skip rotates
      stream->Seek(3 * sizeof(uint16_t), RW_SEEK_CUR);

      ReadArrayLE<(BKT_Count - BKT_ScaleX)>(track->KeyCounts + BKT_ScaleX,
                                            stream);
      for (int j = BKT_ScaleX; j < BKT_Count; j++) {
        track->KeyOffsets[j] = currentCoordOffset;
        currentCoordOffset += track->KeyCounts[j];
        ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                   "Subtrack {:d}: count {:d} offset 0x{:08x}\n", j,
                   track->KeyCounts[j], track->KeyOffsets[j]);
      }

      result->BoneTrackCount++;
    } else if (target.Type == TargetType_MeshGroup) {
      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "Track {:d} is mesh group {:d}\n", i, target.Id);

      // Mesh group track
      std::vector<Mesh*> meshes;
      for (uint32_t j = 0; j < model->MeshCount; j++) {
        if (model->Meshes[j].GroupId == target.Id)
          meshes.push_back(&model->Meshes[j]);
      }

      uint32_t seekPos = tracksOffset + trackSize * i;
      // Skip name
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) seekPos += 32;
      // Skip id, targetType, unknown ushort
      seekPos += 3 * sizeof(uint16_t);
      stream->Seek(seekPos, RW_SEEK_SET);

      uint16_t visibilityCount = ReadLE<uint16_t>(stream);
      int visibilityOffset = currentCoordOffset;
      currentCoordOffset += visibilityCount;
      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "Visibility count {:d} offset 0x{:08x}\n", visibilityCount,
                 visibilityOffset);

      stream->Seek(0x1E, RW_SEEK_CUR);

      uint16_t morphTargetCount = ReadLE<uint16_t>(stream);
      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "Morph target count {:d}\n", morphTargetCount);
      uint16_t morphTargetIds[AnimMaxMorphTargetsPerTrack];
      ReadArrayLE(morphTargetIds, stream, morphTargetCount);

      seekPos = tracksOffset + trackSize * i;
      // Skip name
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) seekPos += 32;
      seekPos += 0x48;
      stream->Seek(seekPos, RW_SEEK_SET);

      uint16_t morphInfluenceCounts[AnimMaxMorphTargetsPerTrack];
      int morphInfluenceOffsets[AnimMaxMorphTargetsPerTrack];
      ReadArrayLE(morphInfluenceCounts, stream, morphTargetCount);
      for (int j = 0; j < morphTargetCount; j++) {
        morphInfluenceOffsets[j] = currentCoordOffset;
        currentCoordOffset += morphInfluenceCounts[j];
        ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                   "Morph {:d}: influence count {:d} offset 0x{:08x}\n", j,
                   morphInfluenceCounts[j], morphInfluenceOffsets[j]);
      }

      for (size_t j = 0; j < meshes.size(); j++) {
        Mesh* mesh = meshes[j];
        MeshTrack* track = &result->MeshTracks[result->MeshTrackCount + j];
        memcpy(track->Name, target.Name, sizeof(target.Name));
        track->Mesh = mesh->Id;
        ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                   "Mesh group {:d} <= mesh {:d}\n", target.Id, mesh->Id);

        track->KeyCounts[MKT_Visible] = visibilityCount;
        track->KeyOffsets[MKT_Visible] = visibilityOffset;
        track->MorphTargetCount = morphTargetCount;
        memcpy(track->KeyCounts + MKT_MorphInfluenceStart, morphInfluenceCounts,
               morphTargetCount * sizeof(uint16_t));
        memcpy(track->KeyOffsets + MKT_MorphInfluenceStart,
               morphInfluenceOffsets, morphTargetCount * sizeof(int));
        memcpy(track->MorphTargetIds, morphTargetIds,
               morphTargetCount * sizeof(uint16_t));
      }
      result->MeshTrackCount += (int)meshes.size();
    }
  }

  result->CoordKeyframeCount = currentCoordOffset;
  result->CoordKeyframes = (CoordKeyframe*)malloc(sizeof(CoordKeyframe) *
                                                  result->CoordKeyframeCount);

  ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
             "---\nTotal coord keyframe count: {:d}\n---\n",
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
  for (uint32_t i = 0; i < trackCount; i++) {
    uint32_t seekPos = tracksOffset + trackSize * i;
    stream->Seek(seekPos, RW_SEEK_SET);
    Target target = GetTarget(stream, model);
    if (target.Type == TargetType_Bone) {
      if (static_cast<int32_t>(i) != TrackForBone[target.Id]) continue;

      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "Interleaving rotations track {:d} bone {:d}\n", i, target.Id);
      BoneTrack* track = &result->BoneTracks[currentBoneTrack];

      std::vector<QuatKeyframe> rotationTrack;

      seekPos = tracksOffset + trackSize * i;
      seekPos += (trackCountsOffset + STT_RotateX * sizeof(uint16_t));
      stream->Seek(seekPos, RW_SEEK_SET);

      uint16_t rawRotXCount = ReadLE<uint16_t>(stream);
      uint16_t rawRotYCount = ReadLE<uint16_t>(stream);
      uint16_t rawRotZCount = ReadLE<uint16_t>(stream);

      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "rawRotXCount={:d}, rawRotYCount={:d}, rawRotZCount={:d}\n",
                 rawRotXCount, rawRotYCount, rawRotZCount);
      rotationTrack.reserve(rawRotXCount + rawRotYCount + rawRotZCount);

      seekPos = tracksOffset + trackSize * i;
      seekPos += (trackOffsetsOffset + STT_RotateX * sizeof(uint32_t));
      stream->Seek(seekPos, RW_SEEK_SET);

      int rawRotXOffset = ReadLE<int>(stream) + HeaderSize;
      int rawRotYOffset = ReadLE<int>(stream) + HeaderSize;
      int rawRotZOffset = ReadLE<int>(stream) + HeaderSize;

      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "rawRotXOffset={:d}, rawRotYOffset={:d}, rawRotZOffset={:d}\n",
                 rawRotXOffset, rawRotYOffset, rawRotZOffset);

      CoordKeyframe* rotXBuffer =
          (CoordKeyframe*)malloc(sizeof(CoordKeyframe) * rawRotXCount);
      CoordKeyframe* rotYBuffer =
          (CoordKeyframe*)malloc(sizeof(CoordKeyframe) * rawRotYCount);
      CoordKeyframe* rotZBuffer =
          (CoordKeyframe*)malloc(sizeof(CoordKeyframe) * rawRotZCount);
      stream->Seek(rawRotXOffset, RW_SEEK_SET);
      ReadArrayLE((float*)rotXBuffer, stream, 2 * rawRotXCount);
      stream->Seek(rawRotYOffset, RW_SEEK_SET);
      ReadArrayLE((float*)rotYBuffer, stream, 2 * rawRotYCount);
      stream->Seek(rawRotZOffset, RW_SEEK_SET);
      ReadArrayLE((float*)rotZBuffer, stream, 2 * rawRotZCount);

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
        key.Time = currentTime / Profile::Scene3D::AnimationDesignFrameRate;
        eulerZYXToQuat(&currentEuler, &key.Value);
        rotationTrack.push_back(key);

        ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                   "QuatKeyframe with z={:f}, y={:f}, x={:f} - nextX={:d}, "
                   "nextY={:d}, "
                   "nextZ={:d} - currentTime={:f}\n",
                   currentEuler.z, currentEuler.y, currentEuler.x, nextX, nextY,
                   nextZ, currentTime);
      }

      free(rotXBuffer);
      free(rotYBuffer);
      free(rotZBuffer);

      rotationTrack.shrink_to_fit();
      rotationTracks.push_back(rotationTrack);
      track->KeyCounts[BKT_Rotate] = (uint16_t)rotationTrack.size();
      track->KeyOffsets[BKT_Rotate] = result->QuatKeyframeCount;
      result->QuatKeyframeCount += track->KeyCounts[BKT_Rotate];

      ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
                 "QuatKeyframe count {:d} offset 0x{:08x}\n",
                 track->KeyCounts[BKT_Rotate], track->KeyOffsets[BKT_Rotate]);

      currentBoneTrack++;
    }
  }

  result->QuatKeyframes =
      (QuatKeyframe*)malloc(sizeof(QuatKeyframe) * result->QuatKeyframeCount);

  ImpLogSlow(LogLevel::Trace, LogChannel::ModelLoad,
             "---\nTotal quat keyframe count: {:d}\n---\n",
             result->QuatKeyframeCount);

  // Now that we have the buffers, fill with data

  currentBoneTrack = 0;
  currentCoordOffset = 0;
  for (uint32_t i = 0; i < trackCount; i++) {
    uint32_t seekPos = tracksOffset + trackSize * i;
    stream->Seek(seekPos, RW_SEEK_SET);
    Target target = GetTarget(stream, model);
    if (target.Type == TargetType_Bone) {
      if (static_cast<int32_t>(i) != TrackForBone[target.Id]) continue;

      BoneTrack* track = &result->BoneTracks[currentBoneTrack];

      // Coords
      for (int j = STT_Visibility; j <= STT_ScaleZ; j++) {
        if (j == STT_Visibility || j == STT_RotateX || j == STT_RotateY ||
            j == STT_RotateZ)
          continue;

        seekPos = tracksOffset + trackSize * i;
        seekPos += trackCountsOffset + sizeof(uint16_t) * j;
        stream->Seek(seekPos, RW_SEEK_SET);
        uint16_t currentKeyframeCount = ReadLE<uint16_t>(stream);
        seekPos = tracksOffset + trackSize * i;
        seekPos += trackOffsetsOffset + sizeof(uint32_t) * j;
        stream->Seek(seekPos, RW_SEEK_SET);
        uint32_t currentKeyframeOffset = ReadLE<uint32_t>(stream);

        stream->Seek(HeaderSize + currentKeyframeOffset, RW_SEEK_SET);

        ReadArrayLE((float*)(result->CoordKeyframes + currentCoordOffset),
                    stream, 2 * currentKeyframeCount);
        for (float* time =
                 (float*)(result->CoordKeyframes + currentCoordOffset);
             time < (float*)(result->CoordKeyframes + currentCoordOffset +
                             currentKeyframeCount);
             time += 2) {
          *time /= Profile::Scene3D::AnimationDesignFrameRate;
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
            // assert(result->QuatKeyframes[track->KeyOffsets[j]].Time == 0.0f);
            // This is quite far off for some stuff (e.g. a CoordKeyframe in
            // c001_000@10_out_pokecom.lka) - may need to fix this properly
            result->QuatKeyframes[track->KeyOffsets[j]].Time = 0.0f;
          } else {
            // assert(result->CoordKeyframes[track->KeyOffsets[j]].Time ==
            // 0.0f);
            result->CoordKeyframes[track->KeyOffsets[j]].Time = 0.0f;
          }
        }
      }

      currentBoneTrack++;
    } else if (target.Type == TargetType_MeshGroup) {
      // Mesh group track

      uint32_t seekPos = tracksOffset + trackSize * i;
      // Skip id, targetType, unknown ushort
      seekPos += trackCountsOffset;
      stream->Seek(seekPos, RW_SEEK_SET);

      uint16_t visibilityCount = ReadLE<uint16_t>(stream);

      stream->Seek(0x1E, RW_SEEK_CUR);
      uint16_t morphTargetCount = ReadLE<uint16_t>(stream);

      seekPos = tracksOffset + trackSize * i;
      // Skip name
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) seekPos += 32;
      seekPos += 0x48;
      stream->Seek(seekPos, RW_SEEK_SET);

      uint16_t morphInfluenceCounts[AnimMaxMorphTargetsPerTrack];
      ReadArrayLE(morphInfluenceCounts, stream, morphTargetCount);

      // Visibility data
      seekPos = tracksOffset + trackSize * i;
      seekPos += trackOffsetsOffset;
      stream->Seek(seekPos, RW_SEEK_SET);
      int rawVisibilityOffset = ReadLE<int>(stream);
      stream->Seek(HeaderSize + rawVisibilityOffset, RW_SEEK_SET);

      ReadArrayLE((float*)(result->CoordKeyframes + currentCoordOffset), stream,
                  2 * visibilityCount);
      for (float* time = (float*)(result->CoordKeyframes + currentCoordOffset);
           time < (float*)(result->CoordKeyframes + currentCoordOffset +
                           visibilityCount);
           time += 2) {
        *time /= Profile::Scene3D::AnimationDesignFrameRate;
      }
      currentCoordOffset += visibilityCount;

      // Morph influence data
      seekPos = tracksOffset + trackSize * i;
      // Skip name
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) seekPos += 32;
      seekPos += 0xA8;
      stream->Seek(seekPos, RW_SEEK_SET);
      int rawInfluenceOffsets[16];
      ReadArrayLE(rawInfluenceOffsets, stream, morphTargetCount);
      for (int j = 0; j < morphTargetCount; j++) {
        stream->Seek(HeaderSize + rawInfluenceOffsets[j], RW_SEEK_SET);
        ReadArrayLE((float*)(result->CoordKeyframes + currentCoordOffset),
                    stream, morphInfluenceCounts[j] * 2);
        for (CoordKeyframe* key = result->CoordKeyframes + currentCoordOffset;
             key < (result->CoordKeyframes + currentCoordOffset +
                    morphInfluenceCounts[j]);
             key++) {
          key->Time /= Profile::Scene3D::AnimationDesignFrameRate;
          key->Value /= 100.0f;
        }
        currentCoordOffset += morphInfluenceCounts[j];
      }
    }
  }

  // Ahhh, now fetch metadata

  result->LoopEnd = result->Duration;

  auto charId = Profile::Scene3D::ModelsToCharacters.find(model->Id);
  if (charId != Profile::Scene3D::ModelsToCharacters.end()) {
    auto const& character = Profile::Scene3D::Characters[charId->second];
    auto animDef = character.Animations.find(animId);
    if (animDef != character.Animations.end()) {
      if (animDef->second.OneShot) {
        if (animId != character.IdleAnimation) {
          // Idle animation always loops, even if the data says otherwise...
          result->OneShot = true;
        }
      } else {
        result->LoopStart = animDef->second.LoopStart /
                            Profile::Scene3D::AnimationDesignFrameRate;
        if (animDef->second.LoopEnd > 0) {
          result->LoopEnd = animDef->second.LoopEnd /
                            Profile::Scene3D::AnimationDesignFrameRate;
        }
      }
    }
  }

  return result;
}

ModelAnimation::~ModelAnimation() {
  if (CoordKeyframes) free(CoordKeyframes);
  if (QuatKeyframes) free(QuatKeyframes);
}

}  // namespace Impacto