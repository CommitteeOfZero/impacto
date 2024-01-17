#pragma once

#include "../button.h"
#include "../label.h"
#include "../../../spriteanimation.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class MovieMenuEntryButton : public Widgets::Button {
 public:
  MovieMenuEntryButton(int id, Sprite const& movieThumbnail,
                       Sprite const& lockedMovieThumbnail,
                       glm::vec2 thumbnailPos, glm::vec2 boxPos);

  void Update(float dt) override;
  void Render() override;

  bool IsLocked = true;

 private:
  SpriteAnimation MovieBoxAnim;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto