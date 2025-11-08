#include "systemmenu.h"
#include <glm/common.hpp>
#include "../../profile/games/cclcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../profile/ui/systemmenu.h"
#include "../../profile/game.h"
#include "../../ui/widgets/cclcc/sysmenubutton.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CCLCC;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  bool targetButtonLocked = static_cast<SysMenuButton*>(target)->IsLocked;
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", targetButtonLocked ? 4 : 2, false,
                     0);
  if (targetButtonLocked) {
    // Yep, that's similar to how it's done in the binary
    // Binary checks for button state and if it's locked, PADone is modified
    // like button press never happened, and then script reads inputs
    PADinputButtonWentDown = PADinputButtonWentDown & ~PAD1A;
    PADinputMouseWentDown = PADinputMouseWentDown & ~PAD1A;
    return;
  }

  ScrWork[SW_SYSMENUCNO] = target->Id;
  // Make the Id match the save menu mode (5th button would be Quick Load
  // which is case 0)
  SaveMenuPtr->ActiveMenuType =
      SaveMenuPageType::_from_integral_nothrow(target->Id % 4);
  ChoiceMade = true;
}

void SystemMenu::UpdateInput(float dt) {
  if (!IsFocused) return;
  const auto* const prevSelected = CurrentlyFocusedElement;
  Menu::UpdateInput(dt);
  if (CurrentlyFocusedElement && prevSelected != CurrentlyFocusedElement) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }
}

SystemMenu::SystemMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.DurationIn = MoveInDuration;
  MenuTransition.DurationOut = MoveOutDuration;

  MenuFade.Direction = AnimationDirection::In;
  MenuFade.LoopMode = AnimationLoopMode::Stop;
  MenuFade.DurationIn = FadeInDuration;
  MenuFade.DurationOut = FadeOutDuration;

  ItemsFade.Direction = AnimationDirection::In;
  ItemsFade.LoopMode = AnimationLoopMode::Stop;
  ItemsFade.DurationIn = ItemsFadeInDuration;
  ItemsFade.DurationOut = ItemsFadeOutDuration;

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  MainItems = new Widgets::Group(this);

  ScreenCap.Sheet =
      SpriteSheet((float)Window->WindowWidth, (float)Window->WindowHeight);
  Texture tex;
  tex.LoadSolidColor(Window->WindowWidth, Window->WindowHeight, 0x000000);
  ScreenCap.Sheet.Texture = tex.Submit();
  ScreenCap.Bounds.Width = ScreenCap.Sheet.DesignWidth;
  ScreenCap.Bounds.Height = ScreenCap.Sheet.DesignHeight;

  for (int i = 0; i < MenuEntriesNum; i++) {
    SysMenuButton* menuButton = new SysMenuButton(
        i, MenuEntriesSprites[i], Sprite(), MenuEntriesHSprites[i],
        MenuEntriesPositions[i], MenuEntriesButtonBounds[i]);

    menuButton->OnClickHandler = onClick;
    MainItems->Add(menuButton, FDIR_DOWN);
  }
  MainItems->Children[0]->SetFocus(MainItems->Children[MenuEntriesNum - 1],
                                   FDIR_UP);
  MainItems->Children[MenuEntriesNum - 1]->SetFocus(MainItems->Children[0],
                                                    FDIR_DOWN);
}
void SystemMenu::Show() {
  if (State != Shown) {
    State = Showing;
    MenuTransition.StartIn();
    MenuFade.StartIn();
    // If the function was called due to a submenu opening directly,
    // then don't take over focus
    if (!((ScrWork[SW_SYSMENUCT] == 32 && ScrWork[SW_SYSSUBMENUCT]) ||
          ScrWork[SW_CLRALPHA])) {
      if (UI::FocusedMenu != 0) {
        LastFocusedMenu = UI::FocusedMenu;
        LastFocusedMenu->IsFocused = false;
      }
      IsFocused = true;
      UI::FocusedMenu = this;
      ItemsFade.StartIn();
      MainItems->Show();
      if (!CurrentlyFocusedElement) AdvanceFocus(FDIR_DOWN);
    }
  }
}

void SystemMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    MenuFade.StartOut();
    MenuTransition.StartOut();
    ItemsFade.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SystemMenu::Update(float dt) {
  UpdateInput(dt);

  if (State == Shown &&
      ((GetFlag(SF_TITLEMODE) || ScrWork[SW_SYSMENUCT] < 32) ||
       ScrWork[SW_SYSMENUALPHA] == 0)) {
    Hide();
    return;
  }
  if (State == Hidden && !GetFlag(SF_TITLEMODE) &&
      ((ScrWork[SW_SYSMENUCT] > 0) || ScrWork[SW_SYSMENUALPHA] > 0)) {
    Show();
    return;
  }
  if (State == Showing && ScrWork[SW_SYSMENUCT] == 32) {
    State = Shown;
    return;
  } else if (State == Hiding && MenuFade.IsOut() && MenuTransition.IsOut() &&
             ItemsFade.IsOut() && ScrWork[SW_SYSMENUCT] == 0) {
    State = Hidden;
    MainItems->Hide();
    return;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    MenuFade.Update(dt);
    if (ItemsFade.IsIn() && ScrWork[SW_SYSSUBMENUCT] > 0 && State == Shown &&
        ItemsFadeComplete) {
      ItemsFade.StartOut();
      ItemsFadeComplete = false;
    } else if (ItemsFade.IsOut() && ScrWork[SW_SYSSUBMENUCT] < 32 &&
               State == Shown && ItemsFadeComplete) {
      ItemsFade.StartIn();
      ItemsFadeComplete = false;
    }
    ItemsFade.Update(dt);
    if (!ItemsFadeComplete) {
      if (ItemsFade.IsIn() && ScrWork[SW_SYSSUBMENUCT] == 0) {
        ItemsFadeComplete = true;
      } else if (ItemsFade.IsOut() && ScrWork[SW_SYSSUBMENUCT] == 32) {
        ItemsFadeComplete = true;
      }
    }

    bool savesDisabled = GetFlag(SF_SAVEDISABLE);
    bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                       SaveSystem::Implementation->GetLockedQuickSaveCount();
    bool quickSaveLockState =
        savesDisabled || noFreeSlots || SaveSystem::HasQSavedOnCurrentLine();
    static_cast<UI::CCLCC::SysMenuButton*>(
        MainItems->Children[static_cast<size_t>(MenuItems::QuickSave)])
        ->IsLocked = quickSaveLockState;
    static_cast<UI::CCLCC::SysMenuButton*>(
        MainItems->Children[static_cast<size_t>(MenuItems::Save)])
        ->IsLocked = savesDisabled;
  }

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}

void SystemMenu::Render() {
  if (State != Hidden && !GetFlag(SF_TITLEMODE)) {
    if (MenuTransition.IsIn()) {
    }
    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha = MenuFade.Progress;
    // Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f,
    // 720.0f),
    //                      glm::vec4(tint, alpha));

    float bgOffset = (ScrWork[SW_SYSSUBMENUCT] * 3000.0f * 0.03125f);
    Sprite offsetSystemMenuBG(SystemMenuBG);
    offsetSystemMenuBG.Bounds.Translate(BGPosition.x - 0.5f * bgOffset,
                                        BGPosition.y);

    const float scale =
        (1000.0f - (ScrWork[SW_SYSMENUCT] * 400 / 32)) / 1000.0f;
    const glm::quat rotation = ScrWorkAnglesToQuaternion(
        ((int)(ScrWork[SW_SYSMENUCT] * AngleMultiplier.x)),
        ((int)(ScrWork[SW_SYSMENUCT] * AngleMultiplier.y)),
        ((int)(ScrWork[SW_SYSMENUCT] * AngleMultiplier.z)));

    const auto transformation = [rotation, scale](glm::vec2 vertex) {
      glm::mat4 transformation = TransformationMatrix(
          BGTranslationOffset, {scale, scale}, {BGTranslationOffset, 0.0f},
          rotation, -BGTranslationOffset);

      glm::vec4 transformedVertex = {vertex, 0.0f, 1.0f};
      transformedVertex = transformation * transformedVertex;
      const float perspective = (transformedVertex.z / 2000.0f) + 1.0f;
      transformedVertex *= perspective;
      transformedVertex += glm::vec4(BGTranslationOffset, 0.0f, 0.0f);

      return glm::vec2(transformedVertex);
    };

    const CornersQuad bgDisp =
        CornersQuad{BGDispOffsetTopLeft, BGDispOffsetBottomLeft,
                    BGDispOffsetTopRight, BGDispOffsetBottomRight}
            .Transform(transformation);
    Renderer->DrawSprite(offsetSystemMenuBG, bgDisp, glm::mat4(1.0f),
                         std::array{glm::vec4(1.0f), glm::vec4(1.0f),
                                    glm::vec4(1.0f), glm::vec4(1.0f)},
                         glm::vec3(0.0f), false, false, true);

    if (!GetFlag(SF_SYSTEMMENUDIRECT)) {
      const CornersQuad frameDisp =
          CornersQuad{glm::vec2{bgOffset, 0} + FrameOffsetTopLeft,
                      glm::vec2{bgOffset, 0} + FrameOffsetBottomLeft,
                      glm::vec2{bgOffset, 0} + FrameOffsetTopRight,
                      glm::vec2{bgOffset, 0} + FrameOffsetBottomRight}
              .Transform(transformation);
      Renderer->DrawSprite(SystemMenuFrame, frameDisp);

      const CornersQuad screenCapDisp = CornersQuad{
          glm::vec2{bgOffset + 0, 0},
          glm::vec2{bgOffset + 0, Profile::DesignHeight},
          glm::vec2{bgOffset + Profile::DesignWidth, 0},
          glm::vec2{bgOffset + Profile::DesignWidth,
                    Profile::DesignHeight}}.Transform(transformation);
      Renderer->DrawSprite(ScreenCap, screenCapDisp, glm::vec4(1.0f),
                           glm::vec3(0.0f), false, true);
    }
    Renderer->DrawSprite(
        SystemMenuMask,
        RectF{0, 0, Profile::DesignWidth, Profile::DesignHeight},
        glm::vec4{tint, alpha});

    MainItems->Tint =
        glm::vec4(tint, glm::smoothstep(0.0f, 1.0f, ItemsFade.Progress));
    MainItems->Render();
  }
}

void SystemMenu::Init() {
  BGPosition = {CALCrnd((int)BGRandPosRange.x), CALCrnd((int)BGRandPosRange.y)};
  SetFlag(SF_SYSTEMMENUCAPTURE, true);

  bool backlogLockState =
      GetFlag(SF_BACKLOG_NOLOG) || GetFlag(SF_MESREVDISABLE);
  static_cast<SysMenuButton*>(
      MainItems->Children[static_cast<size_t>(MenuItems::Backlog)])
      ->IsLocked = backlogLockState;

  // these flag need to be recalculated after loading a game
  bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                     SaveSystem::Implementation->GetLockedQuickSaveCount();
  SetFlag(SF_SAVEALLPROTECTED, noFreeSlots);
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto