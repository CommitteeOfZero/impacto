#include "gamespecific.h"
#include "../profile/game.h"
#include "../profile/vm.h"
#include "../profile/scene3d.h"
#include "../profile/sprites.h"
#include "../profile/ui/gamespecific.h"
#include "../profile/games/cclcc/delusiontrigger.h"
#include "../profile/games/chlcc/delusiontrigger.h"

#include "../games/chlcc/delusiontrigger.h"
#include "../games/chlcc/butterflyeffect.h"
#include "../games/chlcc/eyecatch.h"

#include "../games/cclcc/delusiontrigger.h"
#include "../games/cclcc/yesnotrigger.h"
#include "../games/cclcc/mapsystem.h"

#include "../background2d.h"

using namespace Impacto::Profile::GameSpecific;
using namespace Impacto::Profile::ScriptVars;

namespace Impacto {
namespace UI {
namespace GameSpecific {

static float CHLCCScanlineOffsetY = 0.0f;

void Init() {
  switch (Profile::GameSpecific::GameSpecificType) {
    case +GameSpecificType::CHLCC: {
      CHLCC::DelusionTrigger::GetInstance().Reset();
      CHLCC::ButterflyEffect::GetInstance();
    } break;
    case +GameSpecificType::CC: {
    } break;
    case +GameSpecificType::CCLCC: {
      CCLCC::DelusionTrigger::GetInstance();
      CCLCC::YesNoTrigger::GetInstance();
      CCLCC::MapSystem::GetInstance();
    } break;
    case +GameSpecificType::RNE:
      break;
    case +GameSpecificType::Dash:
      break;
    case +GameSpecificType::None:
      break;
  }
}

void Update(float dt) {
  switch (Profile::GameSpecific::GameSpecificType) {
    case +GameSpecificType::CHLCC: {
      CHLCC::DelusionTrigger::GetInstance().Update(dt);
      CHLCC::ButterflyEffect::GetInstance().Update(dt);
      CHLCCScanlineOffsetY = fmod(dt * 60 + CHLCCScanlineOffsetY, 300.0f);
    } break;
    case +GameSpecificType::CC: {
      UpdateCCButtonGuide(dt);
    } break;
    case +GameSpecificType::CCLCC: {
      UpdateCCButtonGuide(dt);
      CCLCC::YesNoTrigger::GetInstance().Update(dt);
      CCLCC::DelusionTrigger::GetInstance().Update(dt);
      CCLCC::MapSystem::GetInstance().Update(dt);
    } break;
    case +GameSpecificType::RNE:
      break;
    case +GameSpecificType::Dash:
      break;
    case +GameSpecificType::None:
      break;
  }
}
void RenderEarlyMain() {
  switch (Profile::GameSpecific::GameSpecificType) {
    case +GameSpecificType::CHLCC: {
      CHLCC::EyecatchEffect::GetInstance().RenderMain();
    } break;
    case +GameSpecificType::Dash:
    case +GameSpecificType::RNE:
    case +GameSpecificType::CC:
    case +GameSpecificType::CCLCC:
    case +GameSpecificType::None:
      break;
  }
}

void RenderMain() {
  switch (Profile::GameSpecific::GameSpecificType) {
    case +GameSpecificType::CHLCC: {
      CHLCC::DelusionTrigger::GetInstance().Render();
    } break;
    case +GameSpecificType::Dash: {
      /////////// DaSH hack kind of? ///////
      if (GetFlag(SF_Pokecon_Disable) || GetFlag(SF_Pokecon_Open) ||
          Renderer->Scene->MainCamera.CameraTransform.Position !=
              Profile::Scene3D::DefaultCameraPosition)
        SetFlag(SF_DATEDISPLAY, 0);
      else
        SetFlag(SF_DATEDISPLAY, 1);
    }
      [[fallthrough]];
    case +GameSpecificType::RNE: {
      if (GetFlag(SF_Pokecon_Open)) {
        SetFlag(SF_DATEDISPLAY, 0);
        // hack
        ScrWork[SW_POKECON_BOOTANIMECT] = 0;
        ScrWork[SW_POKECON_SHUTDOWNANIMECT] = 0;
        ScrWork[SW_POKECON_MENUSELANIMECT] = 0;
      }
    } break;
    case +GameSpecificType::CC:
      break;
    case +GameSpecificType::CCLCC:
      break;
    case +GameSpecificType::None:
      break;
  }
}

void RenderLayer(uint32_t layer) {
  switch (Profile::GameSpecific::GameSpecificType) {
    case +GameSpecificType::CHLCC: {
      CHLCC::EyecatchEffect::GetInstance().RenderLayer(layer);
      if (ScrWork[SW_MONITOR_SCANLINE_ENABLED] &&
          static_cast<int>(layer) == ScrWork[SW_MONITOR_SCANLINE_PRI]) {
        Renderer->DrawSprite(
            MonitorScanline,
            RectF{0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight},
            glm::vec4{glm::vec3{1.0f}, 26 / 255.0f});
        float y = (299 - CHLCCScanlineOffsetY) * 1000.0f / 300 - 200;
        Renderer->DrawQuad(RectF{0.0f, y, Profile::DesignWidth, 200},
                           glm::vec4{glm::vec3{0.0f}, 88 / 255.0f});
      }
      if (ScrWork[SW_BUTTERFLY_ALPHA] &&
          static_cast<int>(layer) == ScrWork[SW_BUTTERFLY_PRI]) {
        CHLCC::ButterflyEffect::GetInstance().Render();
      }
    } break;
    case +GameSpecificType::CC: {
    } break;
    case +GameSpecificType::CCLCC: {
      if (ScrWork[SW_MAP_PRI] == static_cast<int>(layer) &&
          ScrWork[SW_MAP_ALPHA]) {
        CCLCC::MapSystem::GetInstance().Render();
      }

      if (static_cast<uint32_t>(ScrWork[SW_DELUSION_PRI]) == layer)
        CCLCC::DelusionTrigger::GetInstance().Render();
      if (static_cast<uint32_t>(ScrWork[SW_YESNO_PRI]) == layer) {
        CCLCC::YesNoTrigger::GetInstance().Render();
      }
    } break;
    case +GameSpecificType::RNE:
      break;
    case +GameSpecificType::Dash:
      break;
    case +GameSpecificType::None:
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
}  // namespace GameSpecific
}  // namespace UI
}  // namespace Impacto