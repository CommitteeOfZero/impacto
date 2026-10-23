#include "systemmenu.h"

#include <algorithm>

#include "../../profile/games/sg0/systemmenu.h"
#include "../../profile/ui/systemmenu.h"
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

using namespace Impacto::Profile::SG0::SystemMenu;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

SystemMenu::SystemMenu() {
  const RectF viewport = Window->GetViewport();
  ScreenCap.Sheet = SpriteSheet(viewport.Width, viewport.Height);
  Texture tex;
  tex.LoadSolidColor(static_cast<int>(viewport.Width),
                     static_cast<int>(viewport.Height), 0x000000);
  ScreenCap.Sheet.Texture = tex.Submit();
  ScreenCap.Bounds.SetSize(viewport.GetSize());
}

static RectF ButtonRect(int id, float scale = 1.0f) {
  const Sprite& sprite = MenuEntriesSprites[id];
  const glm::vec2 size =
      glm::vec2(sprite.ScaledWidth(), sprite.ScaledHeight()) * scale;
  return RectF(EntryCenters[id].x - size.x / 2.0f,
               EntryCenters[id].y - size.y / 2.0f, size.x, size.y);
}

void SystemMenu::Init() {
  // Capture the game screen for the monitor before the menu covers it.
  SetFlag(SF_SYSTEMMENUCAPTURE, true);
}

void SystemMenu::Show() {
  if (State == Shown || State == Showing) return;
  State = Showing;
  // F2 / F5-F7 open their screen "directly": the script raises SW_SYSMENUCT
  // with SF_SYSTEMMENUDIRECT set, and the menu itself stays out of sight.
  OpenedAsDirect = GetFlag(SF_SYSTEMMENUDIRECT);
  if (OpenedAsDirect) return;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
}

void SystemMenu::Hide() {
  if (State == Hidden || State == Hiding) return;
  State = Hiding;
  if (OpenedAsDirect) return;
  IsFocused = false;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
    LastFocusedMenu = nullptr;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

void SystemMenu::UpdateInput(float dt) {
  // Submenus (save, backlog, ...) take input while they are open.
  if (!IsFocused || ScrWork[SW_SYSSUBMENUCT] > 0) return;

  const int count = static_cast<int>(NavOrder.size());
  int next = Cursor;
  if (PADinputButtonRepeatAccelDown & (PAD1UP | PAD1LEFT)) {
    next = (Cursor + count - 1) % count;
  } else if (PADinputButtonRepeatAccelDown & (PAD1DOWN | PAD1RIGHT)) {
    next = (Cursor + 1) % count;
  }

  for (int i = 0; i < count; i++) {
    if (!ButtonRect(NavOrder[i]).ContainsPoint(Input::CurMousePos)) continue;
    if (Input::PrevMousePos != Input::CurMousePos) next = i;
  }
  if (next != Cursor) {
    Cursor = next;
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }

  const bool mouseOnCursor =
      ButtonRect(NavOrder[Cursor]).ContainsPoint(Input::CurMousePos);
  if (PADinputButtonWentDown & PAD1A ||
      (Input::MouseButtonWentDown[SDL_BUTTON_LEFT] && mouseOnCursor)) {
    ScrWork[SW_SYSMENUCNO] = NavOrder[Cursor];
    ChoiceMade = true;
  }
  // A click outside every button must not reach the script as a confirm.
  if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT] && !mouseOnCursor) {
    PADinputButtonWentDown &= ~PAD1A;
    PADinputMouseWentDown &= ~PAD1A;
  }
}

void SystemMenu::Update(float dt) {
  const int count = ScrWork[SW_SYSMENUCT];
  if (count > LastSysMenuCt && count > 0 && !GetFlag(SF_TITLEMODE)) {
    Show();
  } else if (count < LastSysMenuCt && count < CountMax) {
    Hide();
  }
  LastSysMenuCt = count;

  if (State == Showing && count >= CountMax) {
    State = Shown;
    IsFocused = UI::FocusedMenu == this;
  } else if (State == Hiding && count <= 0) {
    State = Hidden;
  }

  if (State == Shown && !OpenedAsDirect) UpdateInput(dt);
}

void SystemMenu::Render() {
  if (State == Hidden || OpenedAsDirect || GetFlag(SF_TITLEMODE)) return;

  const float alpha = std::clamp(
      ScrWork[SW_SYSMENUCT] / static_cast<float>(CountMax), 0.0f, 1.0f);
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, alpha);

  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), tint);
  Renderer->DrawSprite(ScreenCap, MonitorScreen, tint);
  Renderer->DrawSprite(GlassSprite, MonitorScreen,
                       glm::vec4(1.0f, 1.0f, 1.0f, alpha * GlassOpacity));
  Renderer->DrawSprite(RingSprite, RingPos, tint);

  // Hide the buttons while a submenu covers the screen.
  if (ScrWork[SW_SYSSUBMENUCT] > 0) return;
  const int focused = NavOrder[Cursor];
  for (int id = 0; id < MenuEntriesNum; id++) {
    const bool isFocused = id == focused && State == Shown;
    if (isFocused) {
      const Sprite& glow = MenuEntriesHSprites[id];
      const glm::vec2 size =
          glm::vec2(glow.ScaledWidth(), glow.ScaledHeight()) * FocusScale;
      Renderer->DrawSprite(
          glow,
          RectF(EntryCenters[id].x - size.x / 2.0f,
                EntryCenters[id].y - size.y / 2.0f, size.x, size.y),
          tint);
    }
    const float brightness = isFocused ? 1.0f : UnfocusedBrightness;
    Renderer->DrawSprite(MenuEntriesSprites[id],
                         ButtonRect(id, isFocused ? FocusScale : 1.0f),
                         glm::vec4(glm::vec3(brightness), alpha));
  }
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
