#include "systemmenu.h"
#include <algorithm>
#include <array>
#include <glm/common.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <numeric>
#include <optional>
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
  MenuTransition.Direction = 1;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = MoveInDuration;
  MenuTransition.DurationOut = MoveOutDuration;

  MenuFade.Direction = 1.0f;
  MenuFade.LoopMode = ALM_Stop;
  MenuFade.DurationIn = FadeInDuration;
  MenuFade.DurationOut = FadeOutDuration;

  ItemsFade.Direction = 1.0f;
  ItemsFade.LoopMode = ALM_Stop;
  ItemsFade.DurationIn = ItemsFadeInDuration;
  ItemsFade.DurationOut = ItemsFadeOutDuration;

  auto onClick =
      std::bind(&SystemMenu::MenuButtonOnClick, this, std::placeholders::_1);

  MainItems = new Widgets::Group(this);

  ScreenCap.Sheet = SpriteSheet(Window->WindowHeight, Window->WindowHeight);
  Texture tex;
  tex.Init(TexFmt_RGBA, Window->WindowHeight, Window->WindowHeight);
  tex.LoadCheckerboard();
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
  if (State == Hidden &&
      ((ScrWork[SW_SYSMENUCT] > 0) || ScrWork[SW_SYSMENUALPHA] > 0)) {
    Show();
    return;
  }
  if (State == Showing && ScrWork[SW_SYSMENUCT] == 32) {
    State = Shown;
    return;
  } else if (State == Hiding && ScrWork[SW_SYSMENUCT] == 0) {
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

std::array<glm::vec2, 4> transformImage(
    std::array<glm::vec2, 4> const& srcRect, float angleX, float angleY,
    float angleZ, float scale, std::optional<glm::vec2> offset = std::nullopt) {
  glm::mat4 modelMatrix{1.0f};
  std::array<glm::vec2, 4> result{};

  // Default to center if offset not provided
  glm::vec2 translate = offset.value_or(
      std::accumulate(srcRect.begin(), srcRect.end(), glm::vec2{0.0f}) * 0.25f);

  modelMatrix = glm::translate(modelMatrix, glm::vec3{translate, 0});

  modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, 1.0f));
  modelMatrix = glm::rotate(modelMatrix, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
  modelMatrix = glm::rotate(modelMatrix, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
  modelMatrix = glm::rotate(modelMatrix, angleZ, glm::vec3(0.0f, 0.0f, 1.0f));
  modelMatrix = glm::translate(modelMatrix, -glm::vec3{translate, 0});
  std::array<glm::vec4, 4> temp;
  float fov = 60.0f;
  float aspectRatio = 16.0f / 9.0f;
  float nearLimit = 0.1f;
  float farLimit = 1000.0f;

  glm::mat4 projection =
      glm::perspective(glm::radians(fov), aspectRatio, nearLimit, farLimit);
  glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 500.0f),     // Camera position
                               glm::vec3(0, 0, -1.0f),      // Look at point
                               glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
  );
  glm::mat4 viewProjection = projection * view;

  for (size_t i = 0; i < srcRect.size(); i++) {
    temp[i] = glm::vec4{srcRect[i], 0, 1};
    temp[i] = modelMatrix * temp[i];
    temp[i] = viewProjection * temp[i];
    // temp[i] /= temp[i].w;

    result[i] = temp[i];
  }

  return result;
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

    Renderer->CaptureScreencap(ScreenCap);

    Sprite repositionedBG = SystemMenuBG;
    glm::vec2 bgOffset;
    bgOffset.x = (ScrWork[SW_SYSSUBMENUCT] * 3000.0 * 0.03125 * 0.5);
    repositionedBG.Bounds.X = (BGPosition.x - 0.5 * bgOffset.x) - 600;
    repositionedBG.Bounds.Width = 1860;
    repositionedBG.Bounds.Y = BGPosition.y - 165;
    repositionedBG.Bounds.Height = 1205;

    glm::vec2 offset = {1452, 395};
    float scale = (1000.0f - (ScrWork[SW_SYSMENUCT] * 400 / 32)) / 1000.0f;
    float angleX = glm::pi<float>() *
                   (ScrWork[SW_SYSMENUCT] * -196608.0f / 5760.0f) / (2 << 15);
    float angleY = glm::pi<float>() *
                   (ScrWork[SW_SYSMENUCT] * -131072.0f / 5760.0f) / (2 << 15);
    float angleZ = glm::pi<float>() *
                   (ScrWork[SW_SYSMENUCT] * 163840.0f / 5760.0f) / (2 << 15);

    std::array<glm::vec2, 4> bgStart = {
        glm::vec2{-1200, 2080},
        glm::vec2{-1200, -330},
        glm::vec2{2520, -330},
        glm::vec2{2520, 2080},
    };
    std::array<glm::vec2, 4> frameStart = {
        glm::vec2{bgOffset.x - 144, 1252},
        glm::vec2{bgOffset.x - 144, -131},
        glm::vec2{bgOffset.x + 2108, -131},
        glm::vec2{bgOffset.x + 2108, 1252},
    };

    std::array<glm::vec2, 4> screenCapStart = {
        glm::vec2{0, 0},
        glm::vec2{0, Profile::DesignHeight},
        glm::vec2{Profile::DesignWidth, Profile::DesignHeight},
        glm::vec2{Profile::DesignWidth, 0},
    };

    Renderer->DrawSprite(
        repositionedBG,
        transformImage(bgStart, angleX, angleY, angleZ, scale, offset),
        {glm::vec4{1}, glm::vec4{1}, glm::vec4{1}, glm::vec4{1}},
        MenuFade.Progress / 30);

    glm::vec4 frameColor{1};
    Renderer->DrawSprite(
        SystemMenuFrame,
        transformImage(frameStart, angleX, angleY, angleZ, scale, offset),
        {frameColor, frameColor, frameColor, frameColor});

    Renderer->DrawSprite(
        ScreenCap,
        transformImage(screenCapStart, angleX, angleY, angleZ, scale, offset),
        {glm::vec4{1}, glm::vec4{1}, glm::vec4{1}, glm::vec4{1}});

    Renderer->DrawSprite(SystemMenuMask,
                         {0, 0, Profile::DesignWidth, Profile::DesignHeight},
                         {tint, MenuFade.Progress});

    MainItems->Tint =
        glm::vec4(tint, glm::smoothstep(0.0f, 1.0f, ItemsFade.Progress));
    MainItems->Render();
  }
}

void SystemMenu::InitPosition() {
  BGPosition = {CALCrnd(390) + 1350, CALCrnd(295) + 165};
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto