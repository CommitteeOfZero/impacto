#include "character2d.h"

#include "mem.h"
#include "io/io.h"
#include "io/vfs.h"
#include "util.h"
#include "profile/scriptvars.h"
#include "profile/vm.h"
#include "voicetable.h"
#include "background2d.h"

namespace Impacto {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

bool Character2D::LoadSync(uint32_t charaId) {
  int fileId = charaId & 0xFFFF;

  Io::Stream* stream;
  int64_t err = Io::VfsOpen(MountPoint, fileId, &stream);
  if (err != IoError_OK) return false;
  CharaTexture.Load(stream);
  delete stream;

  if (Profile::CharaIsMvl) {
    // MVL format
    Face = charaId;

    err = Io::VfsOpen(MountPoint, fileId + 1, &stream);
    if (err != IoError_OK) return false;

    Position = glm::vec2(Profile::DesignWidth, Profile::DesignHeight) / 2.0f;

    Io::ReadLE<int>(stream);
    int stateCount = Io::ReadLE<int>(stream);

    // Skip to state data
    stream->Seek(0x68, SEEK_CUR);

    int vertexCount = Io::ReadLE<int>(stream);
    int vertexOffset = Io::ReadLE<int>(stream);

    for (int i = 0; i < stateCount; i++) {
      int count = Io::ReadLE<int>(stream);
      int start = Io::ReadLE<int>(stream);

      // I... couldn't come up with anything better
      char name[32];
      Io::ReadArrayLE<char>(name, stream, 32);
      int id = 0;
      int idx = (int)strlen(name) - 2;
      if (isdigit(name[idx])) {
        id = atoi(&name[idx]);
      } else if (name[idx] == 'L') {  // Lip
        idx -= 2;
        id = atoi(&name[idx]);
        idx += 3;
        id = (0x40000000 | (id << 8)) | (atoi(&name[idx]) - 1);
      } else if (name[idx] == 'E') {  // Eye
        idx -= 2;
        id = atoi(&name[idx]);
        idx += 3;
        id = (0x30000000 | (id << 8)) | (atoi(&name[idx]) - 1);
      }
      int64_t back = stream->Position;

      stream->Seek(start, SEEK_SET);
      auto [stateItr, inserted] =
          States.try_emplace(id, count, Profile::CharaIsMvl);
      auto mvlIndicesPtr =
          std::get_if<Character2DState::MVLData>(&stateItr->second.Data)
              ->Indices.get();
      Io::ReadArrayLE<uint16_t>(mvlIndicesPtr, stream, count);

      stream->Seek(back, SEEK_SET);
      stream->Seek(0x18, SEEK_CUR);
    }

    // They seem to use the whole vertex array in all states, so... read it once
    // and forget about it?
    stream->Seek(vertexOffset, SEEK_SET);
    std::vector<float> mvlVertexInfo(vertexCount * 5);
    Io::ReadArrayLE<float>(mvlVertexInfo.data(), stream, vertexCount * 5);

    MvlVertices.resize(vertexCount);
    for (size_t i = 0; i < vertexCount; i++) {
      MvlVertices[i] = VertexBufferSprites{
          .Position = {mvlVertexInfo[i * 5], mvlVertexInfo[i * 5 + 1]},
          .UV = glm::vec2(mvlVertexInfo[i * 5 + 3], mvlVertexInfo[i * 5 + 4]),
          .Tint = Tint,
      };
    }

    delete stream;
  } else {
    // LAY format
    err = Io::VfsOpen("chara", fileId + 1, &stream);
    if (err != IoError_OK) return false;

    Face = charaId;

    Position = glm::vec2(Profile::DesignWidth, Profile::DesignHeight) / 2.0f;

    using StreamReadInt_t = auto (*)(Io::Stream*)->int;
    using StreamReadFloat_t = auto (*)(Io::Stream*)->float;
    StreamReadInt_t StreamReadInt;
    StreamReadFloat_t StreamReadFloat;

    if (Profile::LayFileBigEndian) {
      StreamReadInt = &Io::ReadBE<int>;
      StreamReadFloat = &Io::ReadBE<float>;
    } else {
      StreamReadInt = &Io::ReadLE<int>;
      StreamReadFloat = &Io::ReadLE<float>;
    }
    int stateCount = StreamReadInt(stream);
    StreamReadInt(stream);

    for (int i = 0; i < stateCount; i++) {
      int id = StreamReadInt(stream);
      int start = StreamReadInt(stream);
      int count = StreamReadInt(stream);

      int64_t back = stream->Position;
      auto [stateItr, inserted] =
          States.try_emplace(id, count, Profile::CharaIsMvl);
      stream->Seek(12 * (stateCount) + 8 + (start * 16), SEEK_SET);
      for (int i = 0; i < count; i++) {
        glm::vec2 screenCoords;
        glm::vec2 txtCoords;
        screenCoords.x = StreamReadFloat(stream);
        screenCoords.y = StreamReadFloat(stream);

        txtCoords.x = StreamReadFloat(stream) * Profile::LayFileTexXMultiplier;
        txtCoords.y = StreamReadFloat(stream) * Profile::LayFileTexYMultiplier;
        auto* layDataPtr =
            std::get_if<Character2DState::LAYData>(&stateItr->second.Data);
        layDataPtr->ScreenCoords[i] = screenCoords;
        layDataPtr->TextureCoords[i] = txtCoords;
      }

      stream->Seek(back, SEEK_SET);
    }

    delete stream;
  }
  return true;
}

void Character2D::UnloadSync() {
  Renderer->FreeTexture(CharaSpriteSheet.Texture);
  CharaSpriteSheet.DesignHeight = 0.0f;
  CharaSpriteSheet.DesignWidth = 0.0f;
  CharaSpriteSheet.Texture = 0;
  Show = false;
  std::fill(Layers.begin(), Layers.end(), -1);
  MvlVertices.clear();
  MvlIndices.clear();
  States.clear();
  StatesToDraw.clear();
}

void Character2D::MainThreadOnLoad(bool result) {
  CharaSpriteSheet.Texture = CharaTexture.Submit();
  CharaSpriteSheet.DesignWidth = (float)CharaTexture.Width;
  CharaSpriteSheet.DesignHeight = (float)CharaTexture.Height;
  CharaSprite.Sheet = CharaSpriteSheet;
  CharaSprite.BaseScale = glm::vec2(1.0f);
  CharaSprite.Bounds = RectF(0.0f, 0.0f, CharaSpriteSheet.DesignWidth,
                             CharaSpriteSheet.DesignHeight);
  Show = false;
  std::fill(Layers.begin(), Layers.end(), -1);
}

void Character2D::UpdateStatesToDraw() {
  if (Status != LoadStatus::Loaded) return;

  if (Profile::CharaIsMvl) {
    MvlIndices.clear();
    StatesToDraw.clear();
    StatesToDraw.push_back((Face & 0xFFFF0000) >> 16);  // face
    StatesToDraw.push_back(0x40000000 | ((Face & 0xFFFF0000) >> 8) |
                           LipFrame);  // lip
    StatesToDraw.push_back(0x30000000 | ((Face & 0xFFFF0000) >> 8) |
                           EyeFrame);  // eye

    for (auto id : StatesToDraw) {
      if (auto stateItr = States.find(id); stateItr != States.end()) {
        Character2DState const& state = stateItr->second;
        auto& stateIndices =
            std::get_if<Character2DState::MVLData>(&state.Data)->Indices;
        MvlIndices.insert(MvlIndices.end(), stateIndices.get(),
                          stateIndices.get() + state.Count);
      }
    }

  } else {
    StatesToDraw.clear();
    StatesToDraw.push_back(1);
    StatesToDraw.push_back(0x20000000 |
                           ((Face & 0xFFFF0000) >> 16));  // Just face
    StatesToDraw.push_back(0x40000000 | ((Face & 0xFFFF0000) >> 8) |
                           LipFrame);  // Just mouth
    StatesToDraw.push_back(0x60000000 | ((Face & 0xFFFF0000) >> 8) |
                           3 * EyeFrame +
                               LipFrame);  // Both eyes and mouth (3 frames of
                                           // mouth with each frame of the eyes)
    StatesToDraw.push_back(0x30000000 | ((Face & 0xFFFF0000) >> 8));
  }
}

void Character2D::UpdateEyeMouth() {
  // Pause check
  if ((ScrWork[SW_GAMESTATE] & 0b100) != 0) return;

  static std::array<int, CurEyeFrame.size()> eyeCounter{};
  static std::array<int, CurMouthIndex.size()> mouthCounter{};

  for (size_t i = 0; i < eyeCounter.size(); i++) {
    if (eyeCounter[i] == 0) {
      if (++CurEyeFrame[i] == 3) {
        CurEyeFrame[i] = 0;
        eyeCounter[i] = CALCrnd(230) + 200;
      } else {
        eyeCounter[i] = 4;
      }
    } else {
      eyeCounter[i]--;
    }
  }

  for (size_t i = 0; i < mouthCounter.size(); i++) {
    /* TODO: Add the reset flags for the counters from scrcommessync once it's
     * implemented*/

    if (mouthCounter[i] == 0) {
      if (++CurMouthIndex[i] == 20) {
        CurMouthIndex[i] = 0;
      }
      mouthCounter[i] = AnimeTable[CurMouthIndex[i]].second;
    } else {
      mouthCounter[i]--;
    }
  }
}

static uint8_t GetSoundLevel() {
  if (Audio::Channels[Audio::AC_VOICE0]->GetState() != Audio::ACS_Playing ||
      Audio::Channels[Audio::AC_VOICE0]->DurationInSeconds() -
              Audio::Channels[Audio::AC_VOICE0]->PositionInSeconds() <
          FLT_EPSILON) {
    return 0;
  }

  const int audioPos =
      Audio::Channels[Audio::AC_VOICE0]->PositionInSeconds() * 6;

  const int fileId =
      Audio::Channels[Audio::AC_VOICE0]->GetStream()->GetBaseStream()->Meta.Id;
  const uint8_t voiceData = VoiceTableData.GetVoiceData(fileId, audioPos / 4);

  const uint8_t result = (voiceData >> ((audioPos << 1) & 0b110)) & 0b11;
  return result;
}

void Character2D::UpdateState(const int chaId) {
  const size_t structOffset = ScrWorkChaStructSize * chaId;
  const size_t structOfsOffset = ScrWorkChaOffsetStructSize * chaId;

  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::MO6TW) {
    // If I don't do this it tries to access a label with an index of 65535,
    // which is... not good. I have no idea why this happens, the script code
    // does actually seem to do this on purpose, so... HACK (for now)
    if (ScrWork[SW_CHA1NO + structOffset] == 65535)
      ScrWork[SW_CHA1NO + structOffset] = 0;
  }

  Layers = {ScrWork[SW_CHA1PRI + structOffset],
            ScrWork[SW_CHA1PRI2 + structOffset]};
  Show = GetFlag(SF_CHA1DISP + chaId) &&
         ScrWork[SW_CHA1NO + structOffset] != 0xFFFFFF;

  Position = glm::vec2(ScrWork[SW_CHA1POSX + structOffset] +
                           ScrWork[SW_CHA1POSX_OFS + structOfsOffset],
                       ScrWork[SW_CHA1POSY + structOffset] +
                           ScrWork[SW_CHA1POSY_OFS + structOfsOffset]) *
             resolutionScale;

  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::MO8) {
    constexpr std::array<float, 7> BaseScaleValues = {
        1.3f, 1.0f, 0.6f, 0.4f, 0.13f, 0.8f, 0.7f,
    };

    const size_t baseScaleIndex = ScrWork[SW_CHA1BASESIZE + structOffset];
    const float baseScale = baseScaleIndex < BaseScaleValues.size()
                                ? BaseScaleValues[baseScaleIndex]
                                : 1.0f;

    Scale = baseScale *
            glm::vec2(ScrWork[SW_CHA1SIZEX + structOffset],
                      ScrWork[SW_CHA1SIZEY + structOffset]) /
            1000.0f;

    Rotation = ScrWorkAnglesToQuaternion(ScrWork[SW_CHA1ROTX + structOffset],
                                         ScrWork[SW_CHA1ROTY + structOffset],
                                         ScrWork[SW_CHA1ROTZ + structOffset]);

    // More magic, wouldn't be Mage... I'll excuse myself
    Position.y -= 228.0f + (baseScale * 1030.0f);

  } else {
    Scale = glm::vec2(ScrWork[SW_CHA1SIZEX + structOffset] +
                          ScrWork[SW_CHA1SIZEX_OFS + structOfsOffset],
                      ScrWork[SW_CHA1SIZEY + structOffset] +
                          ScrWork[SW_CHA1SIZEY_OFS + structOfsOffset]) /
            1000.0f;

    Rotation = ScrWorkAnglesToQuaternion(
        ScrWork[SW_CHA1ROTX + structOffset] +
            ScrWork[SW_CHA1ROTX_OFS + structOfsOffset],
        ScrWork[SW_CHA1ROTY + structOffset] +
            ScrWork[SW_CHA1ROTY_OFS + structOfsOffset],
        ScrWork[SW_CHA1ROTZ + structOffset] +
            ScrWork[SW_CHA1ROTZ_OFS + structOfsOffset]);
  }

  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::CC) {
    Tint = ScrWorkGetColor(SW_CHA1FILTER + structOffset);
  } else {
    Tint = glm::vec4(1.0f);
  }

  Tint.a = (ScrWork[SW_CHA1ALPHA + structOffset] +
            ScrWork[SW_CHA1ALPHA_OFS + structOfsOffset]) /
           256.0f;
  if (ScrWork[SW_CHA1FADETYPE + structOffset] == 1) {
    Tint.a *= ScrWork[SW_CHA1FADECT + structOffset] / 256.0f;
  }

  Face = ScrWork[SW_CHA1FACE + structOffset] << 16;

  if (ScrWork[SW_CHA1ANIME_EYE + structOffset] == 0xFF) {
    EyeFrame = CurEyeFrame[chaId];
  } else {
    EyeFrame = ScrWork[SW_CHA1ANIME_EYE + structOffset];
  }

  if (ScrWork[SW_CHA1ANIME_MOUTH + structOffset] != 0xFF) {
    LipFrame = ScrWork[SW_CHA1ANIME_MOUTH + structOffset];
  } else {
    bool charSpeaking = false;
    const uint32_t chaIndexMask = 1 << chaId & 0x1F;

    for (size_t dialoguePageId = 0; dialoguePageId < DialoguePageCount;
         dialoguePageId++) {
      if (chaIndexMask & DialoguePages[dialoguePageId].AnimationId) {
        LipFrame = GetSoundLevel() > 0
                       ? AnimeTable[CurMouthIndex[dialoguePageId]].first
                       : 0;
        charSpeaking = true;
        break;
      }
    }

    if (!charSpeaking) LipFrame = 0;
  }

  UpdateStatesToDraw();
}

void Character2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) return;

  if (Profile::CharaIsMvl) {
    std::transform(MvlVertices.begin(), MvlVertices.end(), MvlVertices.begin(),
                   [this](VertexBufferSprites vertex) {
                     vertex.Tint = Tint;
                     return vertex;
                   });

    const glm::mat4 transformation = TransformationMatrix(
        {0.0f, 0.0f}, Scale, glm::vec3(0.0f), Rotation, Position);

    if (!Profile::UseBgEffects || !LastRenderedBackground ||
        !LastRenderedBackground->BgEffChaLoaded) {
      Renderer->DrawVertices(CharaSpriteSheet, ShaderProgramType::Sprite,
                             MvlVertices, MvlIndices, transformation);
    } else {
      const ShaderProgramType shader = LastRenderedBackground->BgEffShaders[3];

      Renderer->DrawVertices(CharaSpriteSheet,
                             &LastRenderedBackground->BgEffChaSprite.Sheet,
                             shader, MvlVertices, MvlIndices, transformation);
    }

  } else {
    for (auto id : StatesToDraw) {
      if (auto stateItr = States.find(id); stateItr != States.end()) {
        Character2DState const& state = stateItr->second;
        const auto& layData =
            *std::get_if<Character2DState::LAYData>(&state.Data);
        for (int i = 0; i < state.Count; i++) {
          CharaSprite.Bounds = RectF(layData.TextureCoords[i].x,
                                     layData.TextureCoords[i].y, 30.0f, 30.0f);

          const glm::mat4 transformation = TransformationMatrix(
              {0.0f, 0.0f}, Scale, glm::vec3(0.0f), Rotation);
          const glm::vec2 pos = layData.ScreenCoords[i] + Position;
          const RectF dest(pos.x, pos.y, 30.0f, 30.0f);

          Renderer->DrawSprite(CharaSprite, dest, transformation, Tint);
        }
      }
    }
  }
}

void CharacterPortrait2D::UpdateState(const int chaId) {
  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  Show = GetFlag(SF_FACEEX1DISP + chaId);
  Face = ScrWork[SW_FACEEX1FACE + 5 * chaId] << 16;
  Position =
      glm::vec2(ScrWork[SW_FACEPOSX], ScrWork[SW_FACEPOSY]) * resolutionScale;

  Scale = {1.0f, 1.0f};
  Rotation = glm::quat();
}

}  // namespace Impacto