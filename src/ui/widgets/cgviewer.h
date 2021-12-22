#pragma once

#include "../widget.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../profile/data/savesystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {

int const MaxCgViewerCgs = 3;
int const MaxCgViewerVariations = 15;

class CgViewer : public Widget {
 public:
  CgViewer();

  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;

  void LoadCgSprites(std::string mountPoint,
                     uint16_t loadIds[][Profile::SaveSystem::MaxCGSprites]);

 protected:
  Texture CgTexture;
  Sprite CgSprites[MaxCgViewerVariations][MaxCgViewerCgs];
  SpriteSheet CgSpriteSheets[MaxCgViewerVariations][MaxCgViewerCgs];

  int CgCount[MaxCgViewerVariations];
  glm::vec2 Position = glm::vec2(0.0f);
  float Scale = 1.0f;
  float MinScale[MaxCgViewerVariations];
  int CurrentVariation = 0;
  int VariationCount = 0;

  void Clear();
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto