#include "backlogmenu.h"

#include "../../profile/game.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/games/cc/backlogmenu.h"
#include "../../profile/scriptvars.h"
#include "../../audio/audiosystem.h"

#include "../cclcc/systemmenu.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::CC::BacklogMenu;
using namespace Impacto::UI::Widgets::CC;
using namespace Impacto::Profile::ScriptVars;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  UI::BacklogMenu::MenuButtonOnClick(target);

  if (!target->AudioId.has_value())
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0.0f);
}

void BacklogMenu::Show() {
  if (State == Hidden) {
    CommonMenu::OnShow(FadeInDuration, FadeOutDuration, FadeAnimation);
  }

  UI::BacklogMenu::Show();
}

void BacklogMenu::Hide() {
  Audio::Channels[Audio::AC_REV]->Stop(0.0f);
  UI::BacklogMenu::Hide();
}

void BacklogMenu::UpdateVisibility() {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 1) {
    Show();
  }

  if (FadeAnimation.IsIn() && ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.IsOut() &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;

    MainItems->Hide();
  }
}

void BacklogMenu::Update(float dt) {
  float prevScrollPos = *MainScrollbar->Value;
  UI::BacklogMenu::Update(dt);
  if (IsFocused && prevScrollPos != *MainScrollbar->Value) {
    if (auto* menu = dynamic_cast<UI::CCLCC::SystemMenu*>(UI::SystemMenuPtr)) {
      menu->BGPosition.y += (prevScrollPos - *MainScrollbar->Value) * 0.5f;
    }
  }
}

void BacklogMenu::Render() {
  if (State == Hidden) return;

  const float opacity = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  const glm::vec4 transition(1.0f, 1.0f, 1.0f, opacity);

  MainItems->Tint = transition;
  MainScrollbar->Tint = transition;

  const float backgroundY =
      static_cast<float>(fmod(PageY - EntryYPadding - RenderingBounds.Y,
                              BacklogBackground.Bounds.Height));

  if (OpenedAsDirect) CommonMenu::DrawBgSprite<false>(State, FadeAnimation);

  for (float yPos = backgroundY; yPos <= Profile::DesignHeight;
       yPos += BacklogBackground.Bounds.Height) {
    Renderer->DrawSprite(BacklogBackground, {0.0f, yPos}, transition);
  }

  RenderHighlight();
  Renderer->DrawSprite(BacklogHeaderSprite, BacklogHeaderPosition, transition);
  MainItems->Render();
  MainScrollbar->Render();

  if (ScrWork[SW_SYSSUBMENUNO] == 1) {
    CommonMenu::DrawOverlay();
    if (GetFlag(SF_TITLEMODE)) {
      CommonMenu::DrawSmoke(Profile::CCLCC::SystemMenu::SmokeOpacityNormal *
                            (1.0f - FadeAnimation.Progress));
    }
  }

  Renderer->DrawSprite(BacklogControlsSprite, BacklogControlsPosition,
                       transition);
}

}  // namespace CC
}  // namespace UI
}  // namespace Impacto