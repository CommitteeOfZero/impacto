#include "scene2d.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../mem.h"
#include "../../profile/game.h"
// #include "../../window.h"
#include "../../renderer/renderer.h"
#include "../../voicetable.h"
#include "../../text.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

void UpdateBackground2D() {
  for (int i = 0; i < Backgrounds.size(); i++) {
    int bufId = ScrWork[SW_BG1SURF + i];

    if (Profile::UseScreenCapEffects) {
      if (GetScriptBufferId(i) == ScrWork[SW_EFF_CAP_BUF]) {
        Backgrounds2D[bufId] = &Screencaptures[0];
      } else if (GetScriptBufferId(i) == ScrWork[SW_EFF_CAP_BUF2]) {
        Backgrounds2D[bufId] = &Screencaptures[1];
      } else if (Backgrounds2D[bufId]->BgSprite.Sheet.IsScreenCap) {
        Backgrounds2D[bufId] = &Backgrounds[bufId];
      }
    }

    const size_t structOffset = ScrWorkBgStructSize * i;
    const size_t structOfsOffset = ScrWorkBgOffsetStructSize * i;

    Backgrounds2D[bufId]->Layers = {ScrWork[SW_BG1PRI + structOffset],
                                    ScrWork[SW_BG1PRI2 + structOffset]};
    Backgrounds2D[bufId]->Show = GetFlag(SF_BG1DISP + i);

    // ScrWork coordinates assume everything is 1280x720 regardless of design
    // resolution, that's why that's hardcoded here
    switch (ScrWork[SW_BG1DISPMODE + structOffset]) {
      case 0: {
        float posX = (float)(ScrWork[SW_BG1POSX + structOffset] +
                             ScrWork[SW_BG1POSX_OFS + structOfsOffset]);
        float posY = (float)(ScrWork[SW_BG1POSY + structOffset] +
                             ScrWork[SW_BG1POSY_OFS + structOfsOffset]);
        posX *= Profile::DesignWidth / 1280.0f;
        posY *= Profile::DesignHeight / 720.0f;

        // Ha ha, why don't we make all positioning do random things per game,
        // wouldn't that be cool
        if (GameInstructionSet == +InstructionSet::MO8) {
          posX += (Backgrounds2D[bufId]->BgSprite.Bounds.Width -
                   Profile::DesignWidth) *
                  0.5f;
          posY += (Backgrounds2D[bufId]->BgSprite.Bounds.Height -
                   Profile::DesignHeight) *
                  0.5f;
        }

        Backgrounds2D[bufId]->DisplayCoords = glm::vec2(-posX, -posY);
        Backgrounds2D[bufId]->BgSprite.BaseScale = glm::vec2(1.0f, 1.0f);
      } break;
      case 1: {
        Backgrounds2D[bufId]->BgSprite.BaseScale =
            glm::vec2(1280.0f / (ScrWork[SW_BG1LX + structOffset] +
                                 ScrWork[SW_BG1LX_OFS + structOfsOffset]),
                      720.0f / (ScrWork[SW_BG1LY + structOffset] +
                                ScrWork[SW_BG1LY_OFS + structOfsOffset]));
        Backgrounds2D[bufId]->DisplayCoords =
            glm::vec2(-((ScrWork[SW_BG1SX + structOffset] +
                         ScrWork[SW_BG1SX_OFS + structOfsOffset]) *
                        (Profile::DesignWidth / 1280.0f)) *
                          Backgrounds2D[bufId]->BgSprite.BaseScale.x,
                      -((ScrWork[SW_BG1SY + structOffset] +
                         ScrWork[SW_BG1SY_OFS + structOfsOffset]) *
                        (Profile::DesignHeight / 720.0f)) *
                          Backgrounds2D[bufId]->BgSprite.BaseScale.y);
      } break;
      case 2: {
        float scale = (ScrWork[SW_BG1SIZE + structOffset] +
                       ScrWork[SW_BG1SIZE_OFS + structOfsOffset]) /
                      1000.0f;
        float posX = (float)(ScrWork[SW_BG1POSX + structOffset] +
                             ScrWork[SW_BG1POSX_OFS + structOfsOffset]);
        float posY = (float)(ScrWork[SW_BG1POSY + structOffset] +
                             ScrWork[SW_BG1POSY_OFS + structOfsOffset]);
        posX = posX * (Profile::DesignWidth / 1280.0f) -
               (Backgrounds2D[bufId]->BgSprite.ScaledWidth() / 2.0f);
        posY = posY * (Profile::DesignHeight / 720.0f) -
               (Backgrounds2D[bufId]->BgSprite.ScaledHeight() / 2.0f);
        Backgrounds2D[bufId]->BgSprite.BaseScale = glm::vec2(scale, scale);
        Backgrounds2D[bufId]->DisplayCoords = glm::vec2(posX, posY);
      } break;
      case 4: {
        float posX = (ScrWork[SW_BG1POSX + structOffset] +
                      ScrWork[SW_BG1POSX_OFS + structOfsOffset]) /
                     1000.0f;
        float posY = (ScrWork[SW_BG1POSY + structOffset] +
                      ScrWork[SW_BG1POSY_OFS + structOfsOffset]) /
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
        Backgrounds2D[bufId]->Links[i].Direction = LinkDirection::Off;
        Backgrounds2D[bufId]->Links[i].LinkedBuffer = NULL;
      }
    }

    if (Backgrounds2D[bufId]->BgSprite.Sheet.IsScreenCap) {
      Backgrounds2D[bufId]->BgSprite.BaseScale *=
          glm::vec2(Profile::DesignWidth / Window->WindowWidth,
                    Profile::DesignHeight / Window->WindowHeight);
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
          case (int)LinkDirection::Up3: {
            float offset = i == 0 ? 0.0f
                                  : currentBuffer->Links[i - 1]
                                        .LinkedBuffer->BgSprite.ScaledHeight();
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y -
                              currentBuffer->BgSprite.ScaledHeight() - offset);
          } break;
          case (int)LinkDirection::Up: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y -
                              currentBuffer->BgSprite.ScaledHeight());
          } break;
          case (int)LinkDirection::Down3: {
            float offset = i == 0 ? 0.0f
                                  : currentBuffer->Links[i - 1]
                                        .LinkedBuffer->BgSprite.ScaledHeight();
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y +
                              currentBuffer->BgSprite.ScaledHeight() + offset);
          } break;
          case (int)LinkDirection::Down: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x,
                          currentBuffer->DisplayCoords.y +
                              currentBuffer->BgSprite.ScaledHeight());
          } break;
          case (int)LinkDirection::Left: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->DisplayCoords.x -
                              currentBuffer->BgSprite.ScaledWidth(),
                          currentBuffer->DisplayCoords.y);
          } break;
          case (int)LinkDirection::Right: {
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

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto