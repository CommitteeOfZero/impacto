#include "scene2d.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../mem.h"
#include "../../profile/game.h"
// #include "../../window.h"
#include "../../renderer/renderer.h"

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
        float posX = (float)(ScrWork[SW_BG1POSX + ScrWorkBgStructSize * i] +
                             ScrWork[SW_BG1POSX_OFS + 10 * i]);
        float posY = (float)(ScrWork[SW_BG1POSY + ScrWorkBgStructSize * i] +
                             ScrWork[SW_BG1POSY_OFS + 10 * i]);
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

static float BaseScaleValues[] = {
    1.3f, 1.0f, 0.6f, 0.4f, 0.13f, 0.8f, 0.7f,
};

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
      float baseScale = 1.0f;
      if (ScrWork[SW_CHA1BASESIZE + ScrWorkChaStructSize * i] < 7) {
        baseScale = BaseScaleValues[ScrWork[SW_CHA1BASESIZE +
                                            ScrWorkChaStructSize * i]];
      }
      Characters2D[bufId].ScaleX =
          baseScale *
          (ScrWork[SW_CHA1SIZEX + ScrWorkChaStructSize * i] / 1000.0f);
      Characters2D[bufId].ScaleY =
          baseScale *
          (ScrWork[SW_CHA1SIZEY + ScrWorkChaStructSize * i] / 1000.0f);

      // ScrWork magic
      Characters2D[bufId].RotationX =
          ScrWork[SW_CHA1ROTX + ScrWorkChaStructSize * i] * (float)M_PI *
          (0.000030517578f);
      Characters2D[bufId].RotationY =
          ScrWork[SW_CHA1ROTY + ScrWorkChaStructSize * i] * (float)M_PI *
          (0.000030517578f);
      Characters2D[bufId].RotationZ =
          ScrWork[SW_CHA1ROTZ + ScrWorkChaStructSize * i] * (float)M_PI *
          (0.000030517578f);

      // More magic, wouldn't be Mage... I'll excuse myself
      Characters2D[bufId].OffsetY =
          Characters2D[bufId].OffsetY - 228.0f - (baseScale * 1030.0f);
    }
    Characters2D[bufId].Face = ScrWork[SW_CHA1FACE + ScrWorkChaStructSize * i]
                               << 16;

    Characters2D[bufId].Tint.a =
        (ScrWork[SW_CHA1ALPHA + Profile::Vm::ScrWorkChaStructSize * i] +
         ScrWork[SW_CHA1ALPHA_OFS + 10 * i]) /
        256.0f;
    if (ScrWork[SW_CHA1FADETYPE + Profile::Vm::ScrWorkChaStructSize * i] == 1) {
      Characters2D[bufId].Tint.a =
          ScrWork[SW_CHA1FADECT + Profile::Vm::ScrWorkChaStructSize * i] /
          256.0f;
    }
  }
}

void UpdateSpeakerPortraits() {
  for (int i = 0; i < MaxSpeakerPortraits; i++) {
    int bufId = ScrWork[SW_FACE1SURF + i];

    SpeakerPortraits[bufId].Show = GetFlag(SF_FACEEX1DISP + i);
    SpeakerPortraits[bufId].OffsetX =
        ScrWork[SW_FACEPOSX] * (Profile::DesignWidth / 1280.0f);
    SpeakerPortraits[bufId].OffsetY =
        ScrWork[SW_FACEPOSY] * (Profile::DesignHeight / 720.0f);
    SpeakerPortraits[bufId].ScaleX = 1.0f;
    SpeakerPortraits[bufId].ScaleY = 1.0f;
    SpeakerPortraits[bufId].Face = ScrWork[SW_FACEEX1FACE + 5 * i] << 16;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto