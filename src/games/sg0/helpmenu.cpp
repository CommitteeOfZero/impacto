#include "helpmenu.h"

#include <algorithm>

#include "../../profile/games/sg0/helpmenu.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::HelpMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

namespace {
constexpr int SubMenuNo = 11;
constexpr int SF_HELP_CLOSED = 1830;
}  // namespace

void HelpMenu::Show() {
  if (State == Shown || State == Showing) return;
  State = Showing;
  Page = 0;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
}

void HelpMenu::Hide() {
  if (State == Hidden || State == Hiding) return;
  State = Hiding;
  IsFocused = false;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
    LastFocusedMenu = nullptr;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

void HelpMenu::UpdateInput(float dt) {
  if (!IsFocused || GetFlag(SF_HELP_CLOSED)) return;
  const int pages = static_cast<int>(PageSprites.size());
  int next = Page;
  if (PADinputButtonWentDown & (PAD1LEFT | PAD1L1) ||
      Input::MouseWheelDeltaY > 0) {
    next = std::max(Page - 1, 0);
  } else if (PADinputButtonWentDown & (PAD1RIGHT | PAD1R1 | PAD1A) ||
             Input::MouseWheelDeltaY < 0 ||
             Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
    next = std::min(Page + 1, pages - 1);
  }
  if (next != Page) {
    Page = next;
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }
  if (PADinputButtonWentDown & PAD1B ||
      Input::MouseButtonWentDown[SDL_BUTTON_RIGHT]) {
    SetFlag(SF_HELP_CLOSED, true);
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 3, false, 0);
  }
}

void HelpMenu::Update(float dt) {
  const int count =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo ? ScrWork[SW_SYSSUBMENUCT] : 0;
  if (count > LastSubMenuCount && count > 0) {
    Show();
  } else if (count < LastSubMenuCount) {
    Hide();
  }
  LastSubMenuCount = count;

  if (State == Showing && count >= CountMax) {
    State = Shown;
    IsFocused = UI::FocusedMenu == this;
  } else if (State == Hiding && count <= 0) {
    State = Hidden;
  }
  if (State == Shown) UpdateInput(dt);
}

void HelpMenu::Render() {
  if (State == Hidden || PageSprites.empty()) return;
  const float alpha =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo
          ? std::clamp(ScrWork[SW_SYSSUBMENUCT] / static_cast<float>(CountMax),
                       0.0f, 1.0f)
          : 0.0f;
  Renderer->DrawSprite(PageSprites[Page], glm::vec2(0.0f),
                       glm::vec4(1.0f, 1.0f, 1.0f, alpha));
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
