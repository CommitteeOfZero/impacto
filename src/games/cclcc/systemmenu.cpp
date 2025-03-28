#include "systemmenu.h"
#include <glm/common.hpp>
#include "../../profile/games/cclcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
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
  ScrWork[SW_SYSMENUCNO] = target->Id;
  // Make the Id match the save menu mode (5th button would be Quick Load which
  // is case 0)
  ScrWork[SW_SAVEMENUMODE] = target->Id % 4;
  ChoiceMade = true;
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

  ScreenCap.Sheet = SpriteSheet(Window->WindowWidth, Window->WindowHeight);
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
    if (!(ScrWork[SW_SYSMENUCT] == 32 && ScrWork[SW_SYSSUBMENUCT] ||
          ScrWork[2151])) {
      if (UI::FocusedMenu != 0) {
        LastFocusedMenu = UI::FocusedMenu;
        LastFocusedMenu->IsFocused = false;
      }
      IsFocused = true;
      UI::FocusedMenu = this;
      ItemsFade.StartIn();
      MainItems->Show();
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
  UpdateInput();

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
  }

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}

auto GenerateMatrix(CornersQuad const& corners, glm::vec2 offset) {
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
  for (int col = 0; col < xDeltas.size(); col++) {
    xDeltas[col] = (bottomRowX[col] - topRowX[col]) / GridRowCount;
  }

  std::array<float, yVerticesCount> yDeltas;
  for (int row = 0; row < yDeltas.size(); row++) {
    yDeltas[row] = (rightColY[row] - leftColY[row]) / GridColCount;
  }

  std::array<glm::vec2, yVerticesCount * xVerticesCount> matrix;
  for (int row = 0; row < yVerticesCount; row++) {
    for (int col = 0; col < xVerticesCount; col++) {
      glm::vec2 pos = {topRowX[col] + xDeltas[col] * row - offset.x,
                       leftColY[row] + yDeltas[row] * col - offset.y};
      matrix[get2DIndex(col, row)] = pos;
    }
  }
  return matrix;
}

void TransformImage(CornersQuad const& sprCorners,
                    CornersQuad const& destCorners, float angleX, float angleY,
                    float angleZ, float scale, glm::vec2 dispOffset,
                    SystemMenu::GridVertices& outSpriteVertices,
                    SystemMenu::GridVertices& outDisplayVertices) {
  glm::mat4 modelMatrix{1.0f};

  modelMatrix = glm::rotate(modelMatrix, angleZ, glm::vec3(0.0f, 0.0f, 1.0f));
  modelMatrix = glm::rotate(modelMatrix, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
  modelMatrix = glm::rotate(modelMatrix, angleY, glm::vec3(0.0f, 1.0f, 0.0f));

  outSpriteVertices = GenerateMatrix(sprCorners, {0, 0});
  outDisplayVertices = GenerateMatrix(destCorners, dispOffset);
  for (auto& dispVertex : outDisplayVertices) {
    glm::vec4 destPos{dispVertex, 0, 1};
    destPos *= scale;
    destPos = modelMatrix * destPos;
    const float perspective = (destPos.z / 2000.0f) + 1.0f;
    dispVertex = destPos;
    dispVertex *= perspective;
    dispVertex += dispOffset;
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
    Sprite repositionedBG = SystemMenuBG;
    float bgOffset = (ScrWork[SW_SYSSUBMENUCT] * 3000.0 * 0.03125 * 0.5);
    repositionedBG.Bounds.X = (BGPosition.x - 0.5 * bgOffset) - 600;
    repositionedBG.Bounds.Width = 1860;
    repositionedBG.Bounds.Y = BGPosition.y - 165;
    repositionedBG.Bounds.Height = 1205;

    const glm::vec2 offset = {1452, 395};
    const float scale =
        (1000.0f - (ScrWork[SW_SYSMENUCT] * 400 / 32)) / 1000.0f;
    const float angleX = glm::pi<float>() *
                         (ScrWork[SW_SYSMENUCT] * -196608.0f / 5760.0f) /
                         (2 << 14);
    const float angleY = glm::pi<float>() *
                         (ScrWork[SW_SYSMENUCT] * -131072.0f / 5760.0f) /
                         (2 << 14);
    const float angleZ = glm::pi<float>() *
                         (ScrWork[SW_SYSMENUCT] * 163840.0f / 5760.0f) /
                         (2 << 14);

    CornersQuad bgDisp = {
        glm::vec2{-1200, -330},
        glm::vec2{-1200, 2080},
        glm::vec2{2520, -330},
        glm::vec2{2520, 2080},
    };

    CornersQuad frameDisp = {
        glm::vec2{bgOffset - 144, -131},
        glm::vec2{bgOffset - 144, 1252},
        glm::vec2{bgOffset + 2108, -131},
        glm::vec2{bgOffset + 2108, 1252},
    };

    CornersQuad screenCapDisp = {
        glm::vec2{bgOffset + 0, 0},
        glm::vec2{bgOffset + 0, Profile::DesignHeight},
        glm::vec2{bgOffset + Profile::DesignWidth, 0},
        glm::vec2{bgOffset + Profile::DesignWidth, Profile::DesignHeight},
    };

    TransformImage(repositionedBG.Bounds, bgDisp, angleX, angleY, angleZ, scale,
                   offset, SpriteGridVertices, DisplayGridVertices);
    Renderer->DrawVertices(repositionedBG.Sheet, SpriteGridVertices,
                           DisplayGridVertices, 21, 11);

    TransformImage(SystemMenuFrame.Bounds, frameDisp, angleX, angleY, angleZ,
                   scale, offset, SpriteGridVertices, DisplayGridVertices);
    Renderer->DrawVertices(SystemMenuFrame.Sheet, SpriteGridVertices,
                           DisplayGridVertices, 21, 11);
    TransformImage(ScreenCap.Bounds, screenCapDisp, angleX, angleY, angleZ,
                   scale, offset, SpriteGridVertices, DisplayGridVertices);
    Renderer->DrawVertices(ScreenCap.Sheet, SpriteGridVertices,
                           DisplayGridVertices, 21, 11, glm::vec4{1}, false,
                           true);
    Renderer->DrawSprite(SystemMenuMask,
                         {0, 0, Profile::DesignWidth, Profile::DesignHeight},
                         glm::vec4{tint, alpha});

    MainItems->Tint =
        glm::vec4(tint, glm::smoothstep(0.0f, 1.0f, ItemsFade.Progress));
    MainItems->Render();
  }
}

void SystemMenu::Init() {
  BGPosition = {CALCrnd(390) + 1350, CALCrnd(295) + 165};
  SetFlag(SF_SYSTEMMENUCAPTURE, true);
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto