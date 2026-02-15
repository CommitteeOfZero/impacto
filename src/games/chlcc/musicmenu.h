#pragma once

#include "animations/selectprompt.h"
#include "animations/menutransition.h"

#include "../../ui/menu.h"
#include "../../util.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/clickarea.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/scrollbar.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

enum class MusicPlaybackMode : uint8_t {
  One = 0b00,
  Playlist = 0b01,
  RepeatOne = 0b10,
  RepeatPlaylist = 0b11,
  COUNT  // number of entries in the enum
};

constexpr MusicPlaybackMode operator&(MusicPlaybackMode mode,
                                      MusicPlaybackMode other) {
  return static_cast<MusicPlaybackMode>(to_underlying(mode) &
                                        to_underlying(other));
}

constexpr MusicPlaybackMode& operator++(MusicPlaybackMode& mode) {
  mode = static_cast<MusicPlaybackMode>(
      (to_underlying(mode) + 1) % to_underlying(MusicPlaybackMode::COUNT));
  return mode;
}

constexpr MusicPlaybackMode operator^(MusicPlaybackMode mode,
                                      MusicPlaybackMode other) {
  return static_cast<MusicPlaybackMode>(to_underlying(mode) ^
                                        to_underlying(other));
}

constexpr MusicPlaybackMode& operator^=(MusicPlaybackMode& mode,
                                        MusicPlaybackMode other) {
  mode = mode ^ other;
  return mode;
}

class MusicMenu : public Menu {
 public:
  MusicMenu();

  void Init() override;
  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

 private:
  Widgets::Group* MainItems;

  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();

  void UpdateEntries();
  void UpdateTitles();
  void MusicButtonOnClick(Widgets::Button* target);
  void SwitchToTrack(std::optional<int> id);
  int GetNextTrackId(int id);
  void ToggleRepeatMode();
  void ToggleAllMode();
  void UpdateLooping();

  Animation TitleFade;
  Animation NowPlayingAnimation;
  Animation FromSystemMenuTransition;
  SelectPromptAnimation SelectAnimation;
  MenuTransitionAnimation MenuTransition;

  Sprite PlayModeRepeatSprite;
  Sprite PlayModeAllSprite;

  Widgets::Label CurrentlyPlayingTrackName;
  Widgets::Label CurrentlyPlayingTrackArtist;

  Widgets::Scrollbar MainScrollbar;

  Widgets::ClickArea PlayModeRepeatClickArea;
  Widgets::ClickArea PlayModeAllClickArea;

  std::optional<int> CurrentlyPlayingTrackId = std::nullopt;
  float PreviousPosition = 0.0f;

  float ScrollY = 0.0f;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;

  bool InputEnabled = false;
  MusicPlaybackMode PlaybackMode = MusicPlaybackMode::One;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto