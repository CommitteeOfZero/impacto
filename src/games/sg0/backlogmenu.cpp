#include "backlogmenu.h"

#include <algorithm>

#include "../../profile/games/sg0/backlogmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/game.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::SG0::BacklogMenu;
using namespace Impacto::Profile::ScriptVars;

namespace {
constexpr int SubMenuNo = 1;
constexpr int AlphaMax = 256;
}  // namespace

void BacklogMenu::Hide() {
  Audio::Channels[Audio::AC_REV]->Stop(0.0f);
  UI::BacklogMenu<Widgets::BacklogEntry>::Hide();
}

void BacklogMenu::UpdateVisibility() {
  const int alpha =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo ? ScrWork[SW_SYSSUBMENUALPHA] : 0;

  if (alpha > LastAlpha && State != Shown && State != Showing) {
    Show();
  } else if (alpha < LastAlpha && State != Hidden && State != Hiding) {
    Hide();
  }
  LastAlpha = alpha;

  // Follow the script's fade.
  FadeAnimation.Progress = std::clamp(alpha / float(AlphaMax), 0.0f, 1.0f);

  if (State == Showing && alpha >= AlphaMax) {
    State = Shown;
    IsFocused = UI::FocusedMenu == this;
  } else if (State == Hiding && alpha <= 0) {
    State = Hidden;
    IsFocused = false;
    for (auto& entry : Entries) entry.Hide();
  }
}

void BacklogMenu::Render() {
  if (State == Hidden) return;

  const float opacity = FadeAnimation.Progress;
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, opacity);

  Renderer->DrawQuad(RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                           Profile::Game::DesignHeight),
                     glm::vec4(0.0f, 0.0f, 0.0f, ShadeOpacity * opacity));
  Renderer->DrawSprite(BacklogBackground, PanelPos, tint);

  if (CurrentlyFocusedElement &&
      RenderingBounds.Intersects(CurrentlyFocusedElement->Bounds)) {
    const RectF& b = CurrentlyFocusedElement->Bounds;
    Renderer->DrawQuad(
        RectF(HighlightBounds.X, b.Y - EntryHighlightPadding,
              HighlightBounds.Width, b.Height + 2 * EntryHighlightPadding),
        glm::vec4(HighlightColor, HighlightOpacity * opacity));
  }

  Renderer->EnableScissor();
  Renderer->SetScissorRect(RenderingBounds);
  for (auto& entry : Entries) {
    if (!entry.Bounds.Intersects(RenderingBounds)) continue;
    entry.Tint = tint;
    entry.Render();
  }
  Renderer->DisableScissor();

  MainScrollbar.Tint = tint;
  MainScrollbar.Render();
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
