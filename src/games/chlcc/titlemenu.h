#pragma once

#include "../../animation.h"
#include "../../character2d.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/titlebutton.h"
#include "introsequence.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

enum class TitleDispCtState : uint8_t {
  IntroAnimation = 0,
  PressStart = 1,
  EmptyBackground = 2,
  MainEntriesFading = 3,
  MainEntriesControl = 4,
  // 5 and 6 go unused
  LoadSubEntriesFading = 7,
  LoadSubEntriesControl = 8,
  ExtraSubEntriesFading = 9,
  ExtraSubEntriesControl = 10,
  SystemSubEntriesFading = 11,
  SystemSubEntriesControl = 12,
};

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

  Animation PressToStartAnimation;
  Animation PrimaryFadeAnimation;
  Animation SecondaryFadeAnimation;
  Animation ItemsFadeInAnimation;
  Animation SecondaryItemsFadeInAnimation;
  Animation SpinningCircleAnimation;
  Animation SpinningCircleFlashingAnimation;

  void MenuButtonOnClick(Widgets::Button* target);
  void SecondaryButtonOnClick(Widgets::Button* target);

  void DrawTitleMenuBackGraphics() const;
  void DrawSpinningCircle(float alpha) const;

 private:
  Widgets::Group* MainItems;
  Widgets::CHLCC::TitleButton* Start;
  Widgets::CHLCC::TitleButton* Load;
  Widgets::CHLCC::TitleButton* Extra;
  Widgets::CHLCC::TitleButton* System;
  Widgets::CHLCC::TitleButton* Exit;

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

  Impacto::CHLCC::IntroSequence IntroSequence;

  void ResetIntroSequence();
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto