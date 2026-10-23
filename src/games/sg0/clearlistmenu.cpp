#include "clearlistmenu.h"

#include <algorithm>

#include "../../profile/games/sg0/clearlistmenu.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;

namespace {
constexpr int SubMenuNo = 7;
}  // namespace

// The script handles Back itself; the focus only keeps the title's items
// from taking the key presses meanwhile.
void ClearListMenu::Show() {
  if (State == Shown || State == Showing) return;
  State = Showing;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
  IsFocused = true;
}

void ClearListMenu::Hide() {
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

float ClearListMenu::Alpha() const {
  if (ScrWork[SW_SYSSUBMENUNO] != SubMenuNo) return 0.0f;
  return std::clamp(ScrWork[SW_SYSSUBMENUCT] / static_cast<float>(CountMax),
                    0.0f, 1.0f);
}

void ClearListMenu::Update(float dt) {
  const float alpha = Alpha();
  if (alpha > 0.0f && State != Shown) {
    Show();
    if (alpha >= 1.0f) State = Shown;
  } else if (alpha <= 0.0f && State != Hidden) {
    Hide();
    State = Hidden;
  }
}

void ClearListMenu::Render() {
  if (State == Hidden) return;
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, Alpha());
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), tint);
  for (int i = 0; i < EndingCount; i++) {
    if (!GetFlag(EndingFlags[i])) continue;
    Renderer->DrawSprite(TubeSprites[i], TubePositions[i], tint);
    Renderer->DrawSprite(LabelSprites[i], LabelPositions[i], tint);
  }
  Renderer->DrawSprite(GuideSprite, GuidePos, tint);
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
