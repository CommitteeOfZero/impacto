#include "character2d.h"

#include "io/io.h"
#include "io/vfs.h"
#include "util.h"

namespace Impacto {

Character2D Characters2D[MaxCharacters2D];

bool Character2D::LoadSync(uint32_t charaId) {
  int fileId = charaId & 0xFFFF;

  Io::InputStream* stream;
  int64_t err = Io::VfsOpen("chara", fileId, &stream);
  if (err != IoError_OK) return false;
  CharaTexture.Load(stream);
  delete stream;

  if (Profile::CharaIsMvl) {
    // MVL format
    Face = charaId;
    LipFrame = 1;
    EyeFrame = 1;

    err = Io::VfsOpen("chara", fileId + 1, &stream);
    if (err != IoError_OK) return false;

    OffsetX = Profile::DesignWidth / 2;
    OffsetY = Profile::DesignHeight / 2;

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
      int idx = strlen(name) - 2;
      if (isdigit(name[idx])) {
        id = atoi(&name[idx]);
      } else if (name[idx] == 'L') {  // Lip
        idx -= 2;
        id = atoi(&name[idx]);
        idx += 3;
        id = (0x40000000 | (id << 8)) | atoi(&name[idx]);
      } else if (name[idx] == 'E') {  // Eye
        idx -= 2;
        id = atoi(&name[idx]);
        idx += 3;
        id = (0x30000000 | (id << 8)) | atoi(&name[idx]);
      }
      States[id].Count = count;

      long back = stream->Position;

      stream->Seek(start, SEEK_SET);
      States[id].Indices = (uint16_t*)malloc(sizeof(uint16_t) * count);
      Io::ReadArrayLE<uint16_t>(States[id].Indices, stream, count);

      stream->Seek(back, SEEK_SET);
      stream->Seek(0x18, SEEK_CUR);
    }

    // They seem to use the whole vertex array in all states, so... read it once
    // and forget about it?
    stream->Seek(vertexOffset, SEEK_SET);
    MvlVertices = (float*)malloc(sizeof(float) * MvlVerticesCount * 5);
    Io::ReadArrayLE<float>(MvlVertices, stream, MvlVerticesCount * 5);

    delete stream;
  } else {
    // LAY format
    err = Io::VfsOpen("chara", fileId + 1, &stream);
    if (err != IoError_OK) return false;

    Face = charaId;

    OffsetX = Profile::DesignWidth / 2;
    OffsetY = Profile::DesignHeight / 2;

    int (*StreamReadInt)(Io::InputStream*);
    float (*StreamReadFloat)(Io::InputStream*);
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

      long back = stream->Position;

      States[id].Count = count;
      States[id].ScreenCoords = (glm::vec2*)malloc(count * sizeof(glm::vec2));
      States[id].TextureCoords = (glm::vec2*)malloc(count * sizeof(glm::vec2));
      stream->Seek(12 * (stateCount) + 8 + (start * 16), SEEK_SET);
      for (int i = 0; i < count; i++) {
        States[id].ScreenCoords[i].x = StreamReadFloat(stream) + 1.0f;
        States[id].ScreenCoords[i].y = StreamReadFloat(stream) + 1.0f;
        States[id].TextureCoords[i].x =
            StreamReadFloat(stream) * Profile::LayFileTexXMultiplier - 1.0f;
        States[id].TextureCoords[i].y =
            StreamReadFloat(stream) * Profile::LayFileTexYMultiplier - 1.0f;
      }

      stream->Seek(back, SEEK_SET);
    }

    delete stream;
  }
  return true;
}

void Character2D::UnloadSync() {
  glDeleteTextures(1, &CharaSpriteSheet.Texture);
  CharaSpriteSheet.DesignHeight = 0.0f;
  CharaSpriteSheet.DesignWidth = 0.0f;
  CharaSpriteSheet.Texture = 0;
  Show = false;
  Layer = -1;
  if (MvlVertices) {
    MvlVerticesCount = 0;
    free(MvlVertices);
  }
  MvlIndicesCount = 0;
  States.clear();
  StatesToDraw.clear();
}

void Character2D::MainThreadOnLoad() {
  CharaSpriteSheet.Texture = CharaTexture.Submit();
  CharaSpriteSheet.DesignWidth = CharaTexture.Width;
  CharaSpriteSheet.DesignHeight = CharaTexture.Height;
  CharaSprite.Sheet = CharaSpriteSheet;
  CharaSprite.BaseScale = glm::vec2(1.0f);
  CharaSprite.Bounds = RectF(0.0f, 0.0f, CharaSpriteSheet.DesignWidth,
                             CharaSpriteSheet.DesignHeight);
  Show = false;
  Layer = -1;
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
      if (States.count(id)) {
        Character2DState state = States[id];
        memcpy(&MvlIndices[MvlIndicesCount], state.Indices,
               state.Count * sizeof(uint16_t));
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
                           EyeFrame);  // Both eyes and mouth (3 frames of mouth
                                       // with each frame of the eyes)
    StatesToDraw.push_back(0x30000000 | ((Face & 0xFFFF0000) >> 8));
  }
}

void Character2D::Render(glm::vec4 col) {
  if (Profile::CharaIsMvl) {
    Renderer2D::DrawCharacterMvl(CharaSprite, glm::vec2(OffsetX, OffsetY),
                                 MvlVerticesCount, MvlVertices, MvlIndicesCount,
                                 MvlIndices, false, col);
  } else {
    for (auto id : StatesToDraw) {
      if (States.count(id)) {
        Character2DState state = States[id];
        for (int i = 0; i < state.Count; i++) {
          CharaSprite.Bounds = RectF(state.TextureCoords[i].x,
                                     state.TextureCoords[i].y, 32.0f, 32.0f);
          Renderer2D::DrawSprite(
              CharaSprite,
              RectF(state.ScreenCoords[i].x + OffsetX,
                    state.ScreenCoords[i].y + OffsetY, 32.0f, 32.0f),
              col);
        }
      }
    }
  }
}

}  // namespace Impacto