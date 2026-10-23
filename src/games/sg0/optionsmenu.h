#pragma once

#include <vector>

#include "../../ui/optionsmenu.h"
#include "../../profile/games/sg0/optionsmenu.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 config (config.dds): BASIC / TEXT / SOUND / VOICE pages of
// rows, each a label with either choice words or a MIN-MAX slider, bound to
// Impacto's config values. _Startup_win opens it with SW_SYSSUBMENUNO 5 and
// SW_SYSSUBMENUCT counting 0..32, runs Option(1) every frame until flag 1830
// (closed) is set, then saves; Delete asks to restore the defaults (Option(4)).
class OptionsMenu : public UI::OptionsMenu {
 public:
  OptionsMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

 protected:
  void UpdateVisibility() override {}

 private:
  using Row = Profile::SG0::OptionsMenu::Row;

  std::vector<const Row*> PageRows(int page) const;
  float RowY(const Row& row, int indexOnPage) const;
  float SliderScale() const;
  RectF SliderRect(float y) const;

  int GetChoice(const Row& row) const;
  void SetChoice(const Row& row, int choice);
  float GetSlider(const Row& row) const;
  void SetSlider(const Row& row, float value);
  bool IsMuted(const Row& row) const;

  int Page = 0;
  int Cursor = 0;
  int LastSubMenuCount = 0;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
