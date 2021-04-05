#include "mask2d.h"

#include "io/memorystream.h"
#include "io/vfs.h"
#include "util.h"
#include "profile/game.h"

namespace Impacto {

Mask2D Masks2D[MaxMasks2D];

bool Mask2D::LoadSync(uint32_t maskId) {
  Io::InputStream* stream;
  int64_t err = Io::VfsOpen("mask", maskId, &stream);
  if (err != IoError_OK) return false;
  MaskTexture.Load(stream);
  delete stream;
  MaskSpriteSheet.Texture = MaskTexture.Submit();
  if ((MaskTexture.Width == 1) && (MaskTexture.Height == 1)) {
    MaskSpriteSheet.DesignWidth = Profile::DesignWidth;
    MaskSpriteSheet.DesignHeight = Profile::DesignHeight;
  } else {
    MaskSpriteSheet.DesignWidth = MaskTexture.Width;
    MaskSpriteSheet.DesignHeight = MaskTexture.Height;
  }
  MaskSprite.Sheet = MaskSpriteSheet;
  MaskSprite.BaseScale = glm::vec2(1.0f);
  MaskSprite.Bounds = RectF(0.0f, 0.0f, MaskSpriteSheet.DesignWidth,
                            MaskSpriteSheet.DesignHeight);
  return true;
}

void Mask2D::UnloadSync() {
  glDeleteTextures(1, &MaskSpriteSheet.Texture);
  MaskSpriteSheet.DesignHeight = 0.0f;
  MaskSpriteSheet.DesignWidth = 0.0f;
  MaskSpriteSheet.Texture = 0;
}

}  // namespace Impacto