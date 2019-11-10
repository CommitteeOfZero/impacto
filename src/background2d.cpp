#include "background2d.h"

#include "io/memorystream.h"
#include "io/vfs.h"
#include "util.h"
#include "profile/game.h"

namespace Impacto {

Background2D Backgrounds2D[MaxBackgrounds2D];

bool Background2D::LoadSync(uint32_t bgId) {
  if (bgId & 0xFF000000) {
    BgTexture.Load1x1(bgId & 0xFF, (bgId >> 8) & 0xFF, (bgId >> 16) & 0xFF,
                      0xFF);
  } else {
    Io::InputStream* stream;
    int64_t err = Io::VfsOpen("bg", bgId, &stream);
    if (err != IoError_OK) return false;
    BgTexture.Load(stream);
    delete stream;
  }
  return true;
}

void Background2D::UnloadSync() {
  glDeleteTextures(1, &BgSpriteSheet.Texture);
  BgSpriteSheet.DesignHeight = 0.0f;
  BgSpriteSheet.DesignWidth = 0.0f;
  BgSpriteSheet.Texture = 0;
  Show = false;
  Layer = -1;
}

void Background2D::MainThreadOnLoad() {
  BgSpriteSheet.Texture = BgTexture.Submit();
  if ((BgTexture.Width == 1) && (BgTexture.Height == 1)) {
    BgSpriteSheet.DesignWidth = Profile::DesignWidth;
    BgSpriteSheet.DesignHeight = Profile::DesignHeight;
  } else {
    BgSpriteSheet.DesignWidth = BgTexture.Width;
    BgSpriteSheet.DesignHeight = BgTexture.Height;
  }
  BgSprite.Sheet = BgSpriteSheet;
  BgSprite.BaseScale = glm::vec2(1.0f);
  BgSprite.Bounds =
      RectF(0.0f, 0.0f, BgSpriteSheet.DesignWidth, BgSpriteSheet.DesignHeight);
  Show = false;
  Layer = -1;
}

}  // namespace Impacto