#pragma once

#include "commonmenu.h"

#include "../../ui/optionsmenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class OptionsMenu : public UI::OptionsMenu, public CommonMenu {
 public:
  OptionsMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;
  void ResetToDefault() override;

 private:
  void RenderPage(size_t pageId, glm::vec2 offset);
  void GoToPage(size_t pageNumber) override;

  void UpdatePageInput(float dt) override;
  void UpdateValues() override;

  void UpdatePageShowAnimation(float dt);
  void UpdatePageTransitionAnimation(float dt);
  void UpdateSelectedLabel(float dt);

  void UpdateVisibility() override;

  size_t PreviousPage = 0;

  Animation PageTransitionAnimation;
  glm::vec2 PageTransitionComingOffset{0.0f, 0.0f};
  glm::vec2 PageTransitionGoingOffset{0.0f, 0.0f};

  Animation SelectedAnimation;
  glm::vec2 SelectedLabelPos;

  enum class PageType { Text = 0, Sound = 1, Voice = 2 };
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto