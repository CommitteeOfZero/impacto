#include "backlogmenu.h"

#include "../../profile/games/chlcc/backlogmenu.h"
#include "../../profile/games/chlcc/commonmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../ui/widgets/chlcc/backlogentry.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::BacklogMenu;
using namespace Impacto::Profile::CHLCC::CommonMenu;
using namespace Impacto::UI::Widgets::CHLCC;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::BacklogMenu;

BacklogMenu::BacklogMenu() : CommonMenu(true) {}

void BacklogMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      FromSystemMenuTransition->StartIn();
    }
    IsFocused = true;
    UI::BacklogMenu<BacklogEntry>::Show();
  }
}

void BacklogMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
      FromSystemMenuTransition->StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
    Audio::Channels[Audio::AC_REV]->Stop(0.0f);
  }
}

void BacklogMenu::Render() {
  if (State == Hidden) return;
  CommonMenu::DrawSubmenu(BackgroundColor, CircleSprite, MenuTitleText,
                          MenuTitleTextAngle);

  if (MenuTransition.Progress < 0.22f) return;

  float yOffset = 0;

  yOffset = MenuTransition.GetPageOffset().y;
  Renderer->DrawSprite(BacklogBackground, {0.0f, 0.0f + yOffset});
  CommonMenu::DrawButtonPrompt(ButtonPromptSprite, ButtonPromptPosition);

  if (MenuTransition.Progress > 0.34f) {
    Renderer->EnableScissor();
    Renderer->SetScissorRect(BacklogBackground.Bounds);
    Renderer->DrawSprite(MenuTitleText, LeftTitlePos);
    Renderer->DisableScissor();
    RenderHighlight({0.0f, yOffset});
  }

  Renderer->EnableScissor();
  Renderer->SetScissorRect(RectF(RenderingBounds).Translate({0.0f, yOffset}));
  for (auto& entry : Entries) {
    if (!entry.Bounds.Intersects(RenderingBounds)) continue;

    entry.Move({0.0f, yOffset});
    entry.Render();
    entry.Move({0.0f, -yOffset});
  }
  Renderer->DisableScissor();

  MainScrollbar.Move({0.0f, yOffset});
  MainScrollbar.Render();
  MainScrollbar.Move({0.0f, -yOffset});
}

void BacklogMenu::Update(float dt) {
  const int sysMenuCt = ScrWork[SW_SYSMENUCT];
  const int systemMenuCHG = ScrWork[SW_SYSTEMMENUCHG];

  if ((!GetFlag(SF_BACKLOGMENU) || sysMenuCt < 10000 ||
       (sysMenuCt == 10000 && systemMenuCHG != 0 && systemMenuCHG != 64)) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_BACKLOGMENU) && sysMenuCt > 0 && State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && !GetFlag(SF_BACKLOGMENU) &&
      systemMenuCHG == 0 && (sysMenuCt == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;

    for (auto& entry : Entries) {
      entry.Hide();
    }
  } else if (MenuTransition.IsIn() && sysMenuCt == 10000 &&
             (systemMenuCHG == 0 || systemMenuCHG == 64) &&
             GetFlag(SF_BACKLOGMENU) && State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    UI::BacklogMenu<BacklogEntry>::Update(dt);
    MenuTransition.Update(dt);
    FromSystemMenuTransition->Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles(MenuTitleTextRightPosition, MenuTitleTextLeftPosition, false);
  }
}
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto
