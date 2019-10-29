#include "character2d.h"

#include "io/io.h"
#include "io/vfs.h"
#include "util.h"

namespace Impacto {

Character2D Characters2D[MaxCharacters2D];

struct Character2DStateDef {
  int Start;
  int Count;
};

bool Character2D::LoadSync(uint32_t charaId) {
  int fileId = charaId & 0xFFFF;
  int statesToLoad[5];
  statesToLoad[0] = 1;
  statesToLoad[1] = 0x20000000 | ((charaId & 0xFFFF0000) >> 16);  // Just face
  statesToLoad[2] = 0x40000000 | ((charaId & 0xFFFF0000) >> 8);   // Just mouth
  statesToLoad[3] = 0x60000000 | ((charaId & 0xFFFF0000) >> 8);   // Face and
                                                                  // mouth
  statesToLoad[4] = 0x30000000 | ((charaId & 0xFFFF0000) >> 8);

  Io::InputStream* stream;
  int64_t err = Io::VfsOpen("chara", fileId, &stream);
  if (err != IoError_OK) return false;
  CharaTexture.Load(stream);
  delete stream;

  err = Io::VfsOpen("chara", fileId + 1, &stream);
  if (err != IoError_OK) return false;

  CharaOffsetX = Profile::DesignWidth / 2;
  CharaOffsetY = Profile::DesignHeight / 2;

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

  ska::flat_hash_map<int, Character2DStateDef> states;

  for (int i = 0; i < stateCount; i++) {
    int id = StreamReadInt(stream);
    Character2DStateDef state;
    state.Start = StreamReadInt(stream);
    state.Count = StreamReadInt(stream);
    states[id] = state;
  }

  for (int i = 0; i < 5; i++) {
    auto it = states.find(statesToLoad[i]);
    if (it != states.end()) {
      int cnt = states[statesToLoad[i]].Count;
      Character2DState stateToDraw;
      stateToDraw.Count = cnt;
      stateToDraw.ScreenCoords = (glm::vec2*)malloc(cnt * sizeof(glm::vec2));
      stateToDraw.TextureCoords = (glm::vec2*)malloc(cnt * sizeof(glm::vec2));
      stream->Seek(12 * (stateCount) + 8 + (states[statesToLoad[i]].Start * 16),
                   SEEK_SET);
      for (int i = 0; i < cnt; i++) {
        stateToDraw.ScreenCoords[i].x = StreamReadFloat(stream) + 1.0f;
        stateToDraw.ScreenCoords[i].y = StreamReadFloat(stream) + 1.0f;
        stateToDraw.TextureCoords[i].x =
            StreamReadFloat(stream) * Profile::LayFileTexXMultiplier - 1.0f;
        stateToDraw.TextureCoords[i].y =
            StreamReadFloat(stream) * Profile::LayFileTexYMultiplier - 1.0f;
      }
      StatesToDraw.push_back(stateToDraw);
    }
  }

  delete stream;
  return true;
}

void Character2D::UnloadSync() {
  glDeleteTextures(1, &CharaSpriteSheet.Texture);
  CharaSpriteSheet.DesignHeight = 0.0f;
  CharaSpriteSheet.DesignWidth = 0.0f;
  CharaSpriteSheet.Texture = 0;
  Show = false;
  Layer = -1;
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

void Character2D::Render() {
  for (auto state : StatesToDraw) {
    for (int i = 0; i < state.Count; i++) {
      CharaSprite.Bounds = RectF(state.TextureCoords[i].x,
                                 state.TextureCoords[i].y, 32.0f, 32.0f);
      Renderer2D::DrawSprite(
          CharaSprite,
          RectF(state.ScreenCoords[i].x + CharaOffsetX,
                state.ScreenCoords[i].y + CharaOffsetY, 32.0f, 32.0f));
    }
  }
}

}  // namespace Impacto