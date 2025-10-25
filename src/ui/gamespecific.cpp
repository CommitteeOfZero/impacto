#include "gamespecific.h"
#include "../profile/vm.h"
#include "../profile/ui/gamespecific.h"
#include "../profile/games/cclcc/delusiontrigger.h"

#include "../profile/games/chlcc/delusiontrigger.h"

using namespace Impacto::Profile::GameSpecific;
using namespace Impacto::Profile::ScriptVars;

namespace Impacto {
namespace UI {
namespace GameSpecific {

void Init() {
  if (Profile::GameSpecific::GameSpecificType == +GameSpecificType::CHLCC) {
    CHLCC::DelusionTrigger::Implementation =
        std::make_unique<CHLCC::DelusionTrigger>();
  } else if (Profile::GameSpecific::GameSpecificType ==
             +GameSpecificType::CCLCC) {
    CCLCC::DelusionTrigger::Implementation =
        std::make_unique<CCLCC::DelusionTrigger>();
    CCLCC::YesNoTrigger::Implementation =
        std::make_unique<CCLCC::YesNoTrigger>();
    CCLCC::MapSystem::Implementation = std::make_unique<CCLCC::MapSystem>();
  }
}

void Update(float dt) {
  if (Profile::GameSpecific::GameSpecificType == +GameSpecificType::CHLCC) {
    CHLCC::DelusionTrigger::Implementation->Update(dt);
  } else if (Profile::GameSpecific::GameSpecificType ==
             +GameSpecificType::CCLCC) {
    CCLCC::YesNoTrigger::Implementation->Update(dt);
    CCLCC::DelusionTrigger::Implementation->Update(dt);
    CCLCC::MapSystem::Implementation->Update(dt);
  }
}

void RenderLayer(uint32_t layer) {
  if (Profile::GameSpecific::GameSpecificType == +GameSpecificType::CHLCC) {
    CHLCC::DelusionTrigger::Implementation->Render();
  } else if (Profile::GameSpecific::GameSpecificType ==
             +GameSpecificType::CCLCC) {
    if (ScrWork[SW_MAP_PRI] == static_cast<int>(layer) &&
        ScrWork[SW_MAP_ALPHA]) {
      CCLCC::MapSystem::Implementation->Render();
    }

    if (static_cast<uint32_t>(ScrWork[SW_DELUSION_PRI]) == layer)
      CCLCC::DelusionTrigger::Implementation->Render();
    if (CCLCC::YesNoTrigger::Implementation &&
        static_cast<uint32_t>(ScrWork[SW_YESNO_PRI]) == layer) {
      CCLCC::YesNoTrigger::Implementation->Render();
    }
  }
}

void RenderCCButtonGuide() {
  if (!GetFlag(SF_UIHIDDEN) && !GetFlag(2487)) {
    if (ScrWork[SW_UI_BTNGUIDE_PROG] != 0) {
      Sprite guideSprite =
          (*UIButtonGuideSprites)[ScrWork[SW_UI_BTNGUIDE_TYPE] - 1];
      float guideXWidth = (ScrWork[SW_UI_BTNGUIDE_PROG] * 1920.0f) / 32.0f;
      guideSprite.Bounds.Width = guideXWidth;
      Renderer->DrawSprite(guideSprite, glm::vec2{0.0f, 988.0f});
      if (guideXWidth < 1920) {
        Sprite guideSprite2 = guideSprite;
        std::array<glm::vec4, 4> tints = {
            glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
        };
        guideSprite2.Bounds.X = guideXWidth;
        guideSprite2.Bounds.Width = 60;

        CornersQuad dest = {
            glm::vec2{guideXWidth, 988.0f},        // top left
            glm::vec2{guideXWidth, 1042.0f},       // bottom left
            glm::vec2{guideXWidth + 60, 988.0f},   // top right
            glm::vec2{guideXWidth + 60, 1042.0f},  // bottom right
        };
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