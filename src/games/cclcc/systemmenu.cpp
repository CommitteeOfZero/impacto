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
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CCLCC;
using namespace Impacto::Input;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
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
  auto newMenuType = magic_enum::enum_cast<SaveMenuPageType>(target->Id % 4);
  if (newMenuType) {
    SaveMenuPtr->ActiveMenuType = newMenuType;
  }
  ChoiceMade = true;
}

void SystemMenu::UpdateInput(float dt) {
  if (!IsFocused) return;
  const auto* const prevSelected = CurrentlyFocusedElement;
  Menu::UpdateInput(dt);
  MainItems->UpdateInput(dt);
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
      if (LastFocusedButtonId && *LastFocusedButtonId < MenuEntriesNum) {
        CurrentlyFocusedElement = MainItems->Children[*LastFocusedButtonId];
        CurrentlyFocusedElement->HasFocus = true;
      } else if (!CurrentlyFocusedElement) {
        AdvanceFocus(FDIR_DOWN);
      }
    }
  }
}

void SystemMenu::Hide() {
  if (State != Hidden) {
    if (CurrentlyFocusedElement) {
      auto* btn = static_cast<Widgets::Button*>(CurrentlyFocusedElement);
      if (btn) {
        LastFocusedButtonId = btn->Id;
      }
    }
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

    if ((CurrentInputDevice == Device::Mouse ||
         CurrentInputDevice == Device::Touch) &&
        ((PADinputMouseWentDown & PAD1A))) {
      bool noButtonsHovered = true;
      for (auto child : MainItems->Children) {
        auto button = static_cast<UI::CCLCC::SysMenuButton*>(child);
        if (button->Hovered) {
          noButtonsHovered = false;
          break;
        }
      }

      if (noButtonsHovered) {
        PADinputMouseWentDown = PADinputMouseWentDown & ~PAD1A;
        PADinputButtonWentDown = PADinputButtonWentDown & ~PAD1A;
      }
    }
  }
}

static auto GenerateMatrix(CornersQuad const& corners) {
  auto get2DIndex = [](int x, int y) { return x + (GridColCount + 1) * y; };
  constexpr int xVerticesCount = GridColCount + 1;
  constexpr int yVerticesCount = GridRowCount + 1;
  std::array<float, xVerticesCount> topRowX;
  std::array<float, xVerticesCount> bottomRowX;
  const float topRowWidthDelta =
      (corners.TopRight.x - corners.TopLeft.x) / GridColCount;
  const float bottomRowWidthDelta =
      (corners.BottomRight.x - corners.BottomLeft.x) / GridColCount;
  for (int i = 0; i < xVerticesCount; i++) {
    topRowX[i] = corners.TopLeft.x + topRowWidthDelta * i;
    bottomRowX[i] = corners.BottomLeft.x + bottomRowWidthDelta * i;
  }
  std::array<float, yVerticesCount> leftColY;
  std::array<float, yVerticesCount> rightColY;
  const float leftColHeightDelta =
      (corners.BottomLeft.y - corners.TopLeft.y) / GridRowCount;
  const float rightColHeightDelta =
      (corners.BottomRight.y - corners.TopRight.y) / GridRowCount;
  for (int i = 0; i < yVerticesCount; i++) {
    leftColY[i] = corners.TopLeft.y + leftColHeightDelta * i;
    rightColY[i] = corners.TopRight.y + rightColHeightDelta * i;
  }

  std::array<float, xVerticesCount> xDeltas;
  for (int col = 0; col < std::ssize(xDeltas); col++) {
    xDeltas[col] = (bottomRowX[col] - topRowX[col]) / GridRowCount;
  }

  std::array<float, yVerticesCount> yDeltas;
  for (int row = 0; row < std::ssize(yDeltas); row++) {
    yDeltas[row] = (rightColY[row] - leftColY[row]) / GridColCount;
  }

  std::array<glm::vec2, yVerticesCount * xVerticesCount> matrix;
  for (int row = 0; row < yVerticesCount; row++) {
    for (int col = 0; col < xVerticesCount; col++) {
      glm::vec2 pos = {topRowX[col] + xDeltas[col] * row,
                       leftColY[row] + yDeltas[row] * col};
      matrix[get2DIndex(col, row)] = pos;
    }
  }
  return matrix;
}

static glm::vec2 TransformImageVertex(const glm::vec2 vertex,
                                      const glm::quat rotation,
                                      const float scale,
                                      const glm::vec2 origin) {
  glm::mat4 transformation = TransformationMatrix(
      origin, {scale, scale}, {origin, 0.0f}, rotation, -origin);

  glm::vec4 transformedVertex = {vertex, 0.0f, 1.0f};
  transformedVertex = transformation * transformedVertex;
  const float perspective = (transformedVertex.z / 2000.0f) + 1.0f;
  transformedVertex *= perspective;
  transformedVertex += glm::vec4(origin, 0.0f, 0.0f);

  return transformedVertex;
}

static void TransformImage(CornersQuad const& sprCorners,
                           CornersQuad const& destCorners, glm::quat rotation,
                           float scale, glm::vec2 origin, glm::vec2 sheetBounds,
                           SystemMenu::GridVertices& vertices) {
  auto spriteVertices = GenerateMatrix(sprCorners);
  auto displayVertices = GenerateMatrix(destCorners);

  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i] = VertexBufferSprites{
        .Position =
            TransformImageVertex(displayVertices[i], rotation, scale, origin),
        .UV = spriteVertices[i] / sheetBounds,
        .Tint = glm::vec4(1.0f),
    };
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
    RectF bgSpriteBounds = SystemMenuBG.Bounds;
    bgSpriteBounds.Translate(
        glm::vec2(BGPosition.x - 0.5f * bgOffset, BGPosition.y));

    const float scale =
        (1000.0f - (ScrWork[SW_SYSMENUCT] * 400.0f / 32.0f)) / 1000.0f;
    const glm::quat rotation = ScrWorkAnglesToQuaternion(
        ((int)(ScrWork[SW_SYSMENUCT] * AngleMultiplier.x)),
        ((int)(ScrWork[SW_SYSMENUCT] * AngleMultiplier.y)),
        ((int)(ScrWork[SW_SYSMENUCT] * AngleMultiplier.z)));

    CornersQuad bgDisp = {
        BGDispOffsetTopLeft,
        BGDispOffsetBottomLeft,
        BGDispOffsetTopRight,
        BGDispOffsetBottomRight,
    };
    TransformImage(bgSpriteBounds, bgDisp, rotation, scale, BGTranslationOffset,
                   SystemMenuBG.Sheet.GetDimensions(), Vertices);

    static constexpr std::array<uint16_t, GridColCount * GridRowCount * 6>
        indices = []() {
          std::array<uint16_t, GridColCount * GridRowCount * 6> result{};
          constexpr uint16_t width = GridColCount + 1;
          constexpr uint16_t height = GridRowCount + 1;
          size_t index = 0;
          for (uint16_t y = 0; y < height - 1; y++) {
            for (uint16_t x = 0; x < width - 1; x++) {
              int v0 = y * width + x;
              int v1 = y * width + (x + 1);
              int v2 = (y + 1) * width + x;
              int v3 = (y + 1) * width + (x + 1);

              // First triangle
              for (auto v : {v1, v0, v2}) {
                result[index++] = static_cast<uint16_t>(v);
              }
              // Second triangle
              for (auto v : {v3, v1, v2}) {
                result[index++] = static_cast<uint16_t>(v);
              }
            }
          }
          return result;
        }();
    Renderer->DrawPrimitives(SystemMenuBG.Sheet, ShaderProgramType::Sprite,
                             Vertices, indices, glm::vec2(0.0f), false, true);

    if (!GetFlag(SF_SYSTEMMENUDIRECT)) {
      CornersQuad frameDisp = {
          glm::vec2{bgOffset, 0} + FrameOffsetTopLeft,
          glm::vec2{bgOffset, 0} + FrameOffsetBottomLeft,
          glm::vec2{bgOffset, 0} + FrameOffsetTopRight,
          glm::vec2{bgOffset, 0} + FrameOffsetBottomRight,
      };
      frameDisp.Transform([&](glm::vec2 corner) {
        return TransformImageVertex(corner, rotation, scale,
                                    BGTranslationOffset);
      });
      Renderer->DrawSprite(SystemMenuFrame, frameDisp);

      CornersQuad screenCapDisp = {
          glm::vec2{bgOffset + 0, 0},
          glm::vec2{bgOffset + 0, Profile::DesignHeight},
          glm::vec2{bgOffset + Profile::DesignWidth, 0},
          glm::vec2{bgOffset + Profile::DesignWidth, Profile::DesignHeight},
      };
      screenCapDisp.Transform([&](glm::vec2 corner) {
        return TransformImageVertex(corner, rotation, scale,
                                    BGTranslationOffset);
      });
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