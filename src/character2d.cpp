#include "character2d.h"

#include "mem.h"
#include "io/io.h"
#include "io/vfs.h"
#include "util.h"
#include "profile/scriptvars.h"
#include "profile/vm.h"

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

    OffsetX = Profile::DesignWidth / 2.0f;
    OffsetY = Profile::DesignHeight / 2.0f;

    Io::ReadLE<int>(stream);
    int stateCount = Io::ReadLE<int>(stream);

    // Skip to state data
    stream->Seek(0x68, SEEK_CUR);

    MvlVerticesCount = Io::ReadLE<int>(stream);
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
    MvlVertices.resize(MvlVerticesCount * 5);
    Io::ReadArrayLE<float>(MvlVertices.data(), stream, MvlVerticesCount * 5);
    delete stream;
  } else {
    // LAY format
    err = Io::VfsOpen("chara", fileId + 1, &stream);
    if (err != IoError_OK) return false;

    Face = charaId;

    OffsetX = Profile::DesignWidth / 2.0f;
    OffsetY = Profile::DesignHeight / 2.0f;

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
  MvlIndicesCount = 0;
  States.clear();
  StatesToDraw.clear();
}

void Character2D::MainThreadOnLoad() {
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

void Character2D::Update(float dt) {
  if (Profile::CharaIsMvl) {
    MvlIndicesCount = 0;
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
        std::copy(stateIndices.get(), stateIndices.get() + state.Count,
                  MvlIndices.begin() + MvlIndicesCount);
        MvlIndicesCount += state.Count;
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

void Character2D::Render(int layer) {
  if (Status != LS_Loaded || !OnLayer(layer) || !Show) return;

  if (Profile::CharaIsMvl) {
    Renderer->DrawCharacterMvl(CharaSprite, glm::vec2(OffsetX, OffsetY),
                               MvlVerticesCount, MvlVertices.data(),
                               MvlIndicesCount, MvlIndices.data(), false, Tint,
                               glm::vec2(ScaleX, ScaleY));
  } else {
    for (auto id : StatesToDraw) {
      if (auto stateItr = States.find(id); stateItr != States.end()) {
        Character2DState const& state = stateItr->second;
        const auto& layData =
            *std::get_if<Character2DState::LAYData>(&state.Data);
        for (int i = 0; i < state.Count; i++) {
          CharaSprite.Bounds = RectF(layData.TextureCoords[i].x,
                                     layData.TextureCoords[i].y, 30.0f, 30.0f);
          Renderer->DrawSprite(CharaSprite,
                               glm::vec2(layData.ScreenCoords[i].x + OffsetX,
                                         layData.ScreenCoords[i].y + OffsetY),
                               Tint);
        }
      }
    }
  }
}

}  // namespace Impacto