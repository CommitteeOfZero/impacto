#pragma once

#include "../../ui/optionsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/chlcc/optionsentry.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets::CHLCC;

class OptionsMenu : public UI::OptionsMenu {
 public:
  OptionsMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

 private:
  void RenderPage(size_t pageId, glm::vec2 offset);
  void GoToPage(size_t pageNumber) override;

  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();

  void UpdateTitles();
  void UpdatePageShowAnimation(float dt);
  void UpdatePageTransitionAnimation(float dt);
  void UpdateSelectedLabel(float dt);

  void UpdateVisibility() override;

  std::unique_ptr<Widgets::Group> CreateTextPage(
      const std::function<void(OptionsEntry*)>& highlight);
  std::unique_ptr<Widgets::Group> CreateSoundPage(
      const std::function<void(OptionsEntry*)>& highlight);
  std::unique_ptr<Widgets::Group> CreateVoicePage(
      const std::function<void(OptionsEntry*)>& highlight);

  Animation TitleFade;
  Animation FromSystemMenuTransition;

  Animation ShowPageAnimation;
  glm::vec2 ShowPageOffset{0.0f, 0.0f};

  size_t PreviousPage = 0;

  Animation PageTransitionAnimation;
  glm::vec2 PageTransitionComingOffset{0.0f, 0.0f};
  glm::vec2 PageTransitionGoingOffset{0.0f, 0.0f};

  Animation SelectedAnimation;
  glm::vec2 SelectedLabelPos;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;

  enum class PageType { Text = 0, Sound = 1, Voice = 2 };
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto