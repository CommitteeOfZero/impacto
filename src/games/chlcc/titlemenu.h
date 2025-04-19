#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/titlebutton.h"
#include "../../sequencedanimation.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class TitleMenu : public Menu {
 public:
  TitleMenu();
  ~TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  Animation PressToStartAnimation;
  Animation PrimaryFadeAnimation;
  Animation SecondaryFadeAnimation;
  Animation ItemsFadeInAnimation;
  Animation SecondaryItemsFadeInAnimation;
  Animation SpinningCircleAnimation;
  Animation SpinningCircleFlashingAnimation;

  SequencedAnimation IntroAnimation;
  Animation IntroPanningAnimation;
  Animation IntroStarBounceAnimation;
  Animation IntroExplodingStarAnimation;
  Animation IntroExplodingStarRotationAnimation;
  Animation IntroFallingStarsAnimation;
  Animation IntroFallingStarsRotationAnimation;
  Animation IntroLogoFadeAnimation;
  Animation IntroLCCLogoAnimation;
  Animation IntroLogoStarHighlightAnimation;
  Animation IntroDelusionADVAnimation;
  Animation IntroDelusionADVHighlightAnimation;
  Animation IntroSeiraAnimation;
  Animation IntroLogoPopOutAnimation;
  Animation IntroCopyrightAnimation;

  void MenuButtonOnClick(Widgets::Button* target);
  void SecondaryButtonOnClick(Widgets::Button* target);

  void DrawIntroAnimation();
  void DrawIntroBackground() const;
  void DrawFallingStars() const;
  void DrawSpinningCircle() const;
  void DrawTitleMenuBackGraphics() const;

 private:
  Widgets::Group* MainItems;
  Widgets::CHLCC::TitleButton* Start;
  Widgets::CHLCC::TitleButton* Load;
  Widgets::CHLCC::TitleButton* Extra;
  Widgets::CHLCC::TitleButton* System;

  Widgets::Group* LoadItems;
  Widgets::CHLCC::TitleButton* SubLoad;
  Widgets::CHLCC::TitleButton* QuickLoad;

  Widgets::Group* CurrentExtraItems;
  Widgets::Group* LockedExtraItems;
  Widgets::Group* UnlockedExtraItems;
  Widgets::CHLCC::TitleButton* ClearList;
  Widgets::CHLCC::TitleButton* CGLibrary;
  Widgets::CHLCC::TitleButton* SoundLibrary;
  Widgets::CHLCC::TitleButton* MovieLibrary;
  Widgets::CHLCC::TitleButton* Tips;
  Widgets::CHLCC::TitleButton* Trophy;

  Widgets::Group* SystemItems;
  Widgets::CHLCC::TitleButton* Config;
  Widgets::CHLCC::TitleButton* SystemSave;

  std::array<std::pair<glm::vec2, float>, 20> IntroFallingStarSeeds;
  Sprite IntroFallingStarsMask;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto