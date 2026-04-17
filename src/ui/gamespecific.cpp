#include "gamespecific.h"
#include "../profile/game.h"
#include "../profile/vm.h"
#include "../profile/scene3d.h"
#include "../profile/sprites.h"
#include "../profile/ui/gamespecific.h"
#include "../profile/games/cclcc/delusiontrigger.h"
#include "../profile/games/chlcc/delusiontrigger.h"

#include "../games/chlcc/delusiontrigger.h"
#include "../effects/chlcc/butterflyeffect.h"
#include "../effects/chlcc/bubbleseffect.h"
#include "../effects/chlcc/eyecatch.h"

#include "../games/cclcc/delusiontrigger.h"
#include "../games/cclcc/yesnotrigger.h"
#include "../games/cclcc/mapsystem.h"

#include "../background2d.h"
#include "../inputsystem.h"
#include "../audio/audiosystem.h"
#include "../vm/interface/input.h"

using namespace Impacto::Profile::GameSpecific;
using namespace Impacto::Profile::ScriptVars;

namespace Impacto {
namespace UI {
namespace GameSpecific {

static float CHLCCScanlineOffsetY = 0.0f;
static void UpdateCCAtChanScrollbar();

void Init() {
  switch (Profile::GameSpecific::GameSpecificType) {
    case GameSpecificType::CHLCC: {
      CHLCC::DelusionTrigger::GetInstance().Reset();
      CHLCC::ButterflyEffect::GetInstance().Init();
      CHLCC::BubblesEffect::GetInstance().Init();
    } break;
    case GameSpecificType::CC: {
    } break;
    case GameSpecificType::CCLCC: {
      CCLCC::DelusionTrigger::GetInstance();
      CCLCC::YesNoTrigger::GetInstance();
      CCLCC::MapSystem::GetInstance();
    } break;
    case GameSpecificType::RNE:
      break;
    case GameSpecificType::Dash:
      break;
    case GameSpecificType::None:
      break;
  }
}

// Update that is run before and independently of ScrWork[SW_GAMESTATE] & 5 &&
// !GetFlag(SF_GAMEPAUSE)
void NonGameplayUpdate(float dt) {
  switch (Profile::GameSpecific::GameSpecificType) {
    case GameSpecificType::CHLCC: {
      if (UI ::TitleMenuPtr) UI::TitleMenuPtr->Update(dt);
    } break;
    case GameSpecificType::Dash:
    case GameSpecificType::RNE:
    case GameSpecificType::CC:
    case GameSpecificType::CCLCC:
    case GameSpecificType::None:
      break;
  }
  return;
}

void Update(float dt) {
  switch (Profile::GameSpecific::GameSpecificType) {
    case GameSpecificType::CHLCC: {
      CHLCC::DelusionTrigger::GetInstance().Update(dt);
      CHLCC::ButterflyEffect::GetInstance().Update(dt);
      CHLCC::BubblesEffect::GetInstance().Update(dt);
      CHLCCScanlineOffsetY = fmod(dt * 60 + CHLCCScanlineOffsetY, 300.0f);
    } break;
    case GameSpecificType::CC: {
      UpdateCCButtonGuide(dt);
      UpdateCCAtChanScrollbar();
    } break;
    case GameSpecificType::CCLCC: {
      UpdateCCButtonGuide(dt);
      UpdateCCAtChanScrollbar();
      CCLCC::YesNoTrigger::GetInstance().Update(dt);
      CCLCC::DelusionTrigger::GetInstance().Update(dt);
      CCLCC::MapSystem::GetInstance().Update(dt);
    } break;
    case GameSpecificType::RNE:
      break;
    case GameSpecificType::Dash:
      break;
    case GameSpecificType::None:
      break;
  }
}
void RenderEarlyMain() {
  switch (Profile::GameSpecific::GameSpecificType) {
    case GameSpecificType::CHLCC: {
      CHLCC::EyecatchEffect::GetInstance().RenderMain();
    } break;
    case GameSpecificType::Dash:
    case GameSpecificType::RNE:
    case GameSpecificType::CC:
    case GameSpecificType::CCLCC:
    case GameSpecificType::None:
      break;
  }
}

void RenderMain() {
  switch (Profile::GameSpecific::GameSpecificType) {
    case GameSpecificType::CHLCC: {
      CHLCC::DelusionTrigger::GetInstance().Render();
    } break;
    case GameSpecificType::Dash: {
      /////////// DaSH hack kind of? ///////
      if (GetFlag(SF_Pokecon_Disable) || GetFlag(SF_Pokecon_Open) ||
          Renderer->Scene->MainCamera.CameraTransform.Position !=
              Profile::Scene3D::DefaultCameraPosition)
        SetFlag(SF_DATEDISPLAY, 0);
      else
        SetFlag(SF_DATEDISPLAY, 1);
    }
      [[fallthrough]];
    case GameSpecificType::RNE: {
      if (GetFlag(SF_Pokecon_Open)) {
        SetFlag(SF_DATEDISPLAY, 0);
        // hack
        ScrWork[SW_POKECON_BOOTANIMECT] = 0;
        ScrWork[SW_POKECON_SHUTDOWNANIMECT] = 0;
        ScrWork[SW_POKECON_MENUSELANIMECT] = 0;
      }
    } break;
    case GameSpecificType::CC:
      break;
    case GameSpecificType::CCLCC:
      break;
    case GameSpecificType::None:
      break;
  }
}

void RenderLayer(uint32_t layer) {
  int layerInt = static_cast<int>(layer);
  switch (Profile::GameSpecific::GameSpecificType) {
    case GameSpecificType::CHLCC: {
      CHLCC::EyecatchEffect::GetInstance().RenderLayer(layer);
      if (ScrWork[SW_MONITOR_SCANLINE_ENABLED] &&
          layerInt == ScrWork[SW_MONITOR_SCANLINE_PRI]) {
        Renderer->DrawSprite(
            MonitorScanline,
            RectF{0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight},
            glm::vec4{glm::vec3{1.0f}, 26 / 255.0f});
        float y = (299 - CHLCCScanlineOffsetY) * 1000.0f / 300 - 200;
        Renderer->DrawQuad(RectF{0.0f, y, Profile::DesignWidth, 200},
                           glm::vec4{glm::vec3{0.0f}, 88 / 255.0f});
      }
      if (ScrWork[SW_BUTTERFLY_ALPHA] &&
          layerInt == ScrWork[SW_BUTTERFLY_PRI]) {
        CHLCC::ButterflyEffect::GetInstance().Render();
      }
      if (ScrWork[SW_BUBBLES_ALPHA] && layerInt == ScrWork[SW_BUBBLES_PRI]) {
        CHLCC::BubblesEffect::GetInstance().Render();
      }

      if (GetFlag(SF_TITLEMODE) && ScrWork[SW_TITLE_PRI] == layerInt) {
        if (UI ::TitleMenuPtr) UI::TitleMenuPtr->Render();
      }

    } break;
    case GameSpecificType::CC: {
    } break;
    case GameSpecificType::CCLCC: {
      if (ScrWork[SW_MAP_PRI] == layerInt && ScrWork[SW_MAP_ALPHA]) {
        CCLCC::MapSystem::GetInstance().Render();
      }

      if (ScrWork[SW_DELUSION_PRI] == layerInt)
        CCLCC::DelusionTrigger::GetInstance().Render();
      if (ScrWork[SW_YESNO_PRI] == layerInt) {
        CCLCC::YesNoTrigger::GetInstance().Render();
      }
    } break;
    case GameSpecificType::RNE:
      break;
    case GameSpecificType::Dash:
      break;
    case GameSpecificType::None:
      break;
  }
}

void RenderCCButtonGuide() {
  if (!GetFlag(SF_UIHIDDEN) && !GetFlag(2487)) {
    if (ScrWork[SW_UI_BTNGUIDE_PROG] != 0) {
      Sprite guideSprite =
          (*UIButtonGuideSprites)[ScrWork[SW_UI_BTNGUIDE_TYPE] - 1];
      float guideXWidth =
          (ScrWork[SW_UI_BTNGUIDE_PROG] * Profile::DesignWidth) / 32.0f;
      guideSprite.Bounds.Width = guideXWidth;
      Renderer->DrawSprite(guideSprite,
                           glm::vec2{0.0f, UIButtonGuideEndDisp->Y});
      if (guideXWidth < Profile::DesignWidth) {
        Sprite guideSprite2 = guideSprite;
        std::array<glm::vec4, 4> tints = {
            glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
        };
        guideSprite2.Bounds.X = guideXWidth;
        guideSprite2.Bounds.Width = 60;

        CornersQuad dest = *UIButtonGuideEndDisp;
        dest.Translate(glm::vec2{guideXWidth, 0});
        Renderer->DrawSprite(guideSprite2, dest, glm::mat4(1.0f), tints,
                             glm::vec3(0.0f), false);
      }
    }
  }
}

void UpdateCCButtonGuide([[maybe_unused]] float dt) {
  if (ScrWork[SW_UI_BTNGUIDE_REQ] == 0) {
    if (ScrWork[SW_UI_BTNGUIDE_PROG] == 0) {
      ScrWork[SW_UI_BTNGUIDE_TYPE] = 0;
    } else {
      ScrWork[SW_UI_BTNGUIDE_PROG]--;
    }
  } else {
    ScrWork[SW_UI_BTNGUIDE_TYPE] = ScrWork[SW_UI_BTNGUIDE_REQ];
    if (ScrWork[SW_UI_BTNGUIDE_PROG] < 32) {
      ScrWork[SW_UI_BTNGUIDE_PROG]++;
    }
  }
}

static void UpdateCCAtChanScrollbar() {
  if (ScrWork[SW_UI_BTNGUIDE_TYPE] == 1) {
    static int dragOffset = 0;
    bool mouseHover = false;

    const RectF thumbBounds{
        (float)-ScrWork[4580] * 1.5f - 2.0f,
        (float)-ScrWork[4581] * 1.5f - 2.0f,
        22.0f,
        112.0f,
    };

    if (Input::CurrentInputDevice == Input::Device::Mouse) {
      ScrWork[SW_BG1POSY] -= Input::MouseWheelDeltaY * 16;

      mouseHover = thumbBounds.ContainsPoint(Input::CurMousePos);
      if (mouseHover) {
        if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
          Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0.0f);
          dragOffset = (int)Input::CurMousePos.y - 189 - ScrWork[SW_BG1POSY];
          Vm::Interface::PADinputMouseWentDown &= ~Vm::Interface::PAD1A;
        }
        RequestCursor(CursorType::Pointer);
      }

      if (ActiveCursorType == CursorType::Pointer) {
        if (Input::MouseButtonIsDown[SDL_BUTTON_LEFT]) {
          ScrWork[SW_BG1POSY] = (int)Input::CurMousePos.y - 189 - dragOffset;
          RequestCursor(CursorType::Pointer);
        } else if (!mouseHover) {
          RequestCursor(CursorType::Default);
          dragOffset = 0;
        }
      }
    } else {
      RequestCursor(CursorType::Default);
      dragOffset = 0;
    }
    if (ScrWork[SW_BG1POSY] < -45) {
      ScrWork[SW_BG1POSY] = -45;
    }
    if (ScrWork[1504] < ScrWork[SW_BG1POSY]) {
      ScrWork[SW_BG1POSY] = ScrWork[1504];
    }
  }
}
}  // namespace GameSpecific
}  // namespace UI
}  // namespace Impacto