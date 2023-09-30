#pragma once

#include "../button.h"
#include "../../../profile/dialogue.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class TrackSelectButton : public Button {
 public:
  TrackSelectButton(int id, Sprite const &focused, glm::vec2 pos,
                    glm::vec2 trackOffset, glm::vec2 artistOffset);
  void SetTrackText(uint8_t *str);
  void SetArtistText(uint8_t *str);
  void Render() override;
  void MoveTracks(glm::vec2 baseline);

 private:
  Label TrackName;
  glm::vec2 TrackTextPos;

  Label Artist;
  glm::vec2 ArtistTextPos;
};
}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
