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
  for (int bgId = 0; bgId < Backgrounds.size(); bgId++) {
    int bufId = ScrWork[SW_BG1SURF + bgId];

    if (Profile::UseScreenCapEffects) {
      if (GetScriptBufferId(bgId) == ScrWork[SW_EFF_CAP_BUF]) {
        Backgrounds2D[bufId] = &Screencaptures[0];
      } else if (GetScriptBufferId(bgId) == ScrWork[SW_EFF_CAP_BUF2]) {
        Backgrounds2D[bufId] = &Screencaptures[1];
      } else if (Backgrounds2D[bufId]->BgSprite.Sheet.IsScreenCap) {
        Backgrounds2D[bufId] = &Backgrounds[bufId];
      }
    }

    Backgrounds2D[bufId]->UpdateState(bgId);
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
                glm::vec2(currentBuffer->Position.x,
                          currentBuffer->Position.y -
                              currentBuffer->BgSprite.ScaledHeight() - offset);
          } break;
          case (int)LinkDirection::Up: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->Position.x,
                          currentBuffer->Position.y -
                              currentBuffer->BgSprite.ScaledHeight());
          } break;
          case (int)LinkDirection::Down3: {
            float offset = i == 0 ? 0.0f
                                  : currentBuffer->Links[i - 1]
                                        .LinkedBuffer->BgSprite.ScaledHeight();
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->Position.x,
                          currentBuffer->Position.y +
                              currentBuffer->BgSprite.ScaledHeight() + offset);
          } break;
          case (int)LinkDirection::Down: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->Position.x,
                          currentBuffer->Position.y +
                              currentBuffer->BgSprite.ScaledHeight());
          } break;
          case (int)LinkDirection::Left: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->Position.x -
                              currentBuffer->BgSprite.ScaledWidth(),
                          currentBuffer->Position.y);
          } break;
          case (int)LinkDirection::Right: {
            currentBuffer->Links[i].DisplayCoords =
                glm::vec2(currentBuffer->Position.x +
                              currentBuffer->BgSprite.ScaledWidth(),
                          currentBuffer->Position.y);
          } break;
        }
      }
    }
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto