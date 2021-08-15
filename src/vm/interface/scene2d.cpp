#include "scene2d.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../mem.h"
#include "../../profile/game.h"
#include "../../window.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

void UpdateBackground2D() {
  for (int i = 0; i < MaxBackgrounds2D; i++) {
    int bufId = ScrWork[SW_BG1SURF + i];

    if (Profile::UseScreenCapEffects) {
      if (GetScriptBufferId(i) == ScrWork[SW_EFF_CAP_BUF]) {
        Backgrounds2D[bufId] = &Screencaptures[0];
      } else if (GetScriptBufferId(i) == ScrWork[SW_EFF_CAP_BUF2]) {
        Backgrounds2D[bufId] = &Screencaptures[1];
      } else if (Backgrounds2D[bufId]->IsScreencap) {
        Backgrounds2D[bufId] = &Backgrounds[bufId];
      }
    }

    Backgrounds2D[bufId]->Layer = ScrWork[SW_BG1PRI + ScrWorkBgStructSize * i];
    Backgrounds2D[bufId]->Show = GetFlag(SF_BG1DISP + i);

    // ScrWork coordinates assume everything is 1280x720 regardless of design
    // resolution, that's why that's hardcoded here
    switch (ScrWork[SW_BG1DISPMODE + ScrWorkBgStructSize * i]) {
      case 0: {
        int posX = ScrWork[SW_BG1POSX + ScrWorkBgStructSize * i] +
                   ScrWork[SW_BG1POSX_OFS + 10 * i];
        int posY = ScrWork[SW_BG1POSY + ScrWorkBgStructSize * i] +
                   ScrWork[SW_BG1POSY_OFS + 10 * i];
        Backgrounds2D[bufId]->DisplayCoords =
            glm::vec2(-(posX * (Profile::DesignWidth / 1280.0f)),
                      -(posY * (Profile::DesignHeight / 720.0f)));
        Backgrounds2D[bufId]->BgSprite.BaseScale = glm::vec2(1.0f, 1.0f);
      } break;
      case 1: {
        Backgrounds2D[bufId]->BgSprite.BaseScale =
            glm::vec2(1280.0f / (ScrWork[SW_BG1LX + ScrWorkBgStructSize * i] +
                                 ScrWork[SW_BG1LX_OFS + 10 * i]),
                      720.0f / (ScrWork[SW_BG1LY + ScrWorkBgStructSize * i] +
                                ScrWork[SW_BG1LY_OFS + 10 * i]));
        Backgrounds2D[bufId]->DisplayCoords =
            glm::vec2(-((ScrWork[SW_BG1SX + ScrWorkBgStructSize * i] +
                         ScrWork[SW_BG1SX_OFS + 10 * i]) *
                        (Profile::DesignWidth / 1280.0f)) *
                          Backgrounds2D[bufId]->BgSprite.BaseScale.x,
                      -((ScrWork[SW_BG1SY + ScrWorkBgStructSize * i] +
                         ScrWork[SW_BG1SY_OFS + 10 * i]) *
                        (Profile::DesignHeight / 720.0f)) *
                          Backgrounds2D[bufId]->BgSprite.BaseScale.y);
      } break;
      case 2: {
        float scale = (ScrWork[SW_BG1SIZE + ScrWorkBgStructSize * i] +
                       ScrWork[SW_BG1SIZE_OFS + 10 * i]) /
                      1000.0f;
        Backgrounds2D[bufId]->BgSprite.BaseScale = glm::vec2(scale, scale);
        Backgrounds2D[bufId]->DisplayCoords = glm::vec2(
            ScrWork[SW_BG1POSX + ScrWorkBgStructSize * i] *
                    (Profile::DesignWidth / 1280.0f) -
                (Backgrounds2D[bufId]->BgSprite.ScaledWidth() / 2.0f),
            ScrWork[SW_BG1POSY + ScrWorkBgStructSize * i] *
                    (Profile::DesignHeight / 720.0f) -
                (Backgrounds2D[bufId]->BgSprite.ScaledHeight() / 2.0f));
      } break;
      case 4: {
        float posX = (ScrWork[SW_BG1POSX + ScrWorkBgStructSize * i] +
                      ScrWork[SW_BG1POSX_OFS + 10 * i]) /
                     1000.0f;
        float posY = (ScrWork[SW_BG1POSY + ScrWorkBgStructSize * i] +
                      ScrWork[SW_BG1POSY_OFS + 10 * i]) /
                     1000.0f;
        Backgrounds2D[bufId]->DisplayCoords =
            glm::vec2(-(posX * (Profile::DesignWidth / 1280.0f)),
                      -(posY * (Profile::DesignHeight / 720.0f)));
        Backgrounds2D[bufId]->BgSprite.BaseScale = glm::vec2(1.0f, 1.0f);
      } break;
    }

    if (ScrWork[SW_BGLINK]) {
      LinkBuffers(ScrWork[SW_BGLINK], i, Backgrounds2D[bufId]);
    } else if (ScrWork[SW_BGLINK2]) {
      LinkBuffers(ScrWork[SW_BGLINK2], i, Backgrounds2D[bufId]);
    } else {
      for (int i = 0; i < MaxLinkedBgBuffers; i++) {
        Backgrounds2D[bufId]->Links[i].Direction = LD_Off;
        Backgrounds2D[bufId]->Links[i].LinkedBuffer = NULL;
      }
    }

    if (Backgrounds2D[bufId]->IsScreencap) {
      Backgrounds2D[bufId]->BgSprite.BaseScale *=
          glm::vec2(Profile::DesignWidth / Window::WindowWidth,
                    Profile::DesignHeight / Window::WindowHeight);
    }
  }
}

void LinkBuffers(int linkCode, int currentBufferId,
                 Background2D* currentBuffer) {
  int srcBufId = (linkCode >> 8) & 0xFF;
  if (srcBufId == GetScriptBufferId(currentBufferId)) {
    int dir = (linkCode >> 16) & 0xFF;
    for (int i = 0; i < MaxLinkedBgBuffers; i++) {
      int childBufId = (linkCode >> (i * 24)) & 0xFF;
      if (childBufId != 0) {
        childBufId = GetBufferId(childBufId);
        Background2D* childBuf =
            Backgrounds2D[ScrWork[SW_BG1SURF + childBufId]];
        childBuf->BgSprite.BaseScale = currentBuffer->BgSprite.BaseScale;
        currentBuffer->Links[i].LinkedBuffer = childBuf;
        currentBuffer->Links[i].Direction = (LinkDirection)dir;
        switch (dir) {
          case LD_Up3: {
            float offset = i == 0 ? 0.0f
                                  : currentBuffer->Links[i - 1]
                                        .LinkedBuffer->BgSprite.ScaledHeight();
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y -
                              currentBuffer->BgSprite.ScaledHeight() - offset);
          } break;
          case LD_Up: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y -
                              currentBuffer->BgSprite.ScaledHeight());
          } break;
          case LD_Down3: {
            float offset = i == 0 ? 0.0f
                                  : currentBuffer->Links[i - 1]
                                        .LinkedBuffer->BgSprite.ScaledHeight();
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y +
                              currentBuffer->BgSprite.ScaledHeight() + offset);
          } break;
          case LD_Down: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y +
                              currentBuffer->BgSprite.ScaledHeight());
          } break;
          case LD_Left: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x -
                              currentBuffer->BgSprite.ScaledWidth(),
                          currentBuffer->DisplayCoords.y);
          } break;
          case LD_Right: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x +
                              currentBuffer->BgSprite.ScaledWidth(),
                          currentBuffer->DisplayCoords.y);
          } break;
        }
      }
    }
  }
}

void UpdateCharacter2D() {
  for (int i = 0; i < MaxCharacters2D; i++) {
    if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW) {
      // If I don't do this it tries to access a label with an index of 65535,
      // which is... not good. I have no idea why this happens, the script code
      // does actually seem to do this on purpose, so... HACK (for now)
      if (ScrWork[SW_CHA1NO + ScrWorkChaStructSize * i] == 65535)
        ScrWork[SW_CHA1NO + ScrWorkChaStructSize * i] = 0;
    }
    int bufId = ScrWork[SW_CHA1SURF + i];
    Characters2D[bufId].Layer = ScrWork[SW_CHA1PRI + ScrWorkChaStructSize * i];
    Characters2D[bufId].Show = GetFlag(SF_CHA1DISP + i);
    Characters2D[bufId].OffsetX =
        (ScrWork[SW_CHA1POSX + ScrWorkChaStructSize * i] +
         ScrWork[SW_CHA1POSX_OFS + 10 * i]) *
        (Profile::DesignWidth / 1280.0f);
    Characters2D[bufId].OffsetY =
        (ScrWork[SW_CHA1POSY + ScrWorkChaStructSize * i] +
         ScrWork[SW_CHA1POSY_OFS + 10 * i]) *
        (Profile::DesignHeight / 720.0f);
    if (Profile::Vm::GameInstructionSet == +InstructionSet::MO8) {
      Characters2D[bufId].OffsetY = -Characters2D[bufId].OffsetY;
    }
    Characters2D[bufId].Face = ScrWork[SW_CHA1FACE + ScrWorkChaStructSize * i]
                               << 16;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto