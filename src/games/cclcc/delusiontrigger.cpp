#include "delusiontrigger.h"
#include "../../profile/games/cclcc/delusiontrigger.h"
#include "../../profile/hud/delusiontrigger.h"
#include "../../vm/interface/scene2d.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptvars.h"
#include "../../src/video/videosystem.h"

namespace Impacto {
namespace CCLCC {
using namespace Impacto::Profile::CCLCC::DelusionTrigger;
using namespace Impacto::Profile::DelusionTrigger;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

DelusionTrigger::DelusionTrigger()
    : DelusionTriggerBase(ScrWork[6412], UiState::Hidden) {}

bool DelusionTrigger::Show(int bgMtrgSelBufferId, int bgMtrgNegaPosiBufferId,
                           int param3) {
  // TODO, scrwork 6413 (mtrgSelCounter) & scrwork 6415 (mtrgNegaPosiCounter) to
  // use delta time

  ScrWork[6410] = bgMtrgSelBufferId;
  ScrWork[6411] = bgMtrgNegaPosiBufferId;
  DelusionState = DS_Neutral;
  ScrWork[6413] = 0;
  LastDelusionState = 0xff;
  ScrWork[6415] = 0x40;
  ScrWork[6414] = param3;
  SetFlag(2820, 1);
  ScrWork[6344] = 48;
  SetFlag(2821, 0);
  ScrWork[6418] = 960;
  if (!Video::Players[0]->IsPlaying) {
    ScrWork[6413] = 32;
  }
  return true;
}

void DelusionTrigger::Hide() {
  SetFlag(2820, 0);
  ScrWork[6344] = 0;
}

bool DelusionTrigger::CheckTransitionAnimationComplete() {
  return LastDelusionState == DelusionState;
}

bool DelusionTrigger::CheckStartTransitionComplete() {
  return LastDelusionState != 0xff;
}

void DelusionTrigger::Update(float dt) {
  if (!GetFlag(2820)) {
    if ((Video::Players[0]->IsPlaying && GetFlag(1236)) || ScrWork[6413] == 0) {
      return;
    }
    if (GetFlag(SF_MESALLSKIP)) {
      ScrWork[6413] = 0;
    }
    if (ScrWork[6413] > 0) ScrWork[6413]--;
  } else {
    if (ScrWork[6413] < 32) {
      ScrWork[6413]++;
    } else {
      if (GetFlag(2821) || ScrWork[SW_SYSSUBMENUCT] != 0 ||
          ScrWork[SW_SYSMENUCT] != 0) {
        return;
      }

      if (PADcustom[14] & PADinputButtonWentDown ||
          (GetFlag(SF_MESALLSKIP)) != 0) {
        Video::Players[0]->Stop();
      }
      if (LastDelusionState == 0xff && Video::Players[0]->IsPlaying &&
          ScrWork[6345] - 13 < ScrWork[6343] &&
          LastDelusionState != DelusionState && ScrWork[6415] == 0) {
        if (ScrWork[6415] != 0) {
          ScrWork[6415] -= 2;
        }
        if (ScrWork[6415] == 0) {
          LastDelusionState = DelusionState;
        }
      }
      if (!Video::Players[0]->IsPlaying) {
        if ((ScrWork[6333] == 0xffff) || (ScrWork[6333] == 0xffff)) {
          Video::VideoShutdown();
          SetFlag(2486, 0);
          ScrWork[6333] = 0xffff;
          ScrWork[6336] = 0xffff;
        }
        if (LastDelusionState == DelusionState) {
          if (!(PADinputButtonWentDown & PAD1L2)) {
            if (PADinputButtonWentDown & PAD1R2) {
              if (DelusionState == DS_Neutral) {
                if (ScrWork[6414] == 0 || ScrWork[6414] == 2) {
                  Audio::Channels[Audio::AC_SSE]->Play("sysse", 11, false,
                                                       0.0f);
                  ScrWork[6415] = 64;
                  DelusionState = DS_Negative;
                }
              } else if (DelusionState == DS_Positive) {
                Audio::Channels[Audio::AC_SSE]->Play("sysse", 12, false, 0.0f);
                ScrWork[6415] = 64;
                DelusionState = DS_Neutral;
              }
            }
          } else if (DelusionState ==
                     DS_Neutral) {  //(PADinputButtonWentDown & PAD1L2)
            if (ScrWork[6414] == 0 || ScrWork[6414] == 1) {
              Audio::Channels[Audio::AC_SSE]->Play("sysse", 10, false, 0.0f);
              ScrWork[6415] = 64;
              DelusionState = DS_Positive;
            }
          } else if (DelusionState ==
                     DS_Negative) {  //(PADinputButtonWentDown & PAD1L2)
            Audio::Channels[Audio::AC_SSE]->Play("sysse", 12, false, 0.0f);
            ScrWork[6415] = 64;
            DelusionState = DS_Neutral;
          }
        } else {
          if (ScrWork[6415] != 0) {
            ScrWork[6415] -= 2;
          }
          if (ScrWork[6415] == 0) {
            LastDelusionState = DelusionState;
          }
        }
      }
    }
  }
}

void DelusionTrigger::RenderStartTransition(Sprite& bgSprite, float spinAngle,
                                            int spinAlpha) {
  float scale = (ScrWork[6415] + 64) / 64.0f;
  float transparency = ((64 - ScrWork[6415]) * spinAlpha >> 6) / 256.0f;
  if ((ScrWork[6414] == 0) || (ScrWork[6414] == 1)) {
    bgSprite.Bounds.X = 1024;
    glm::vec2 topLeft = {-800.0f, -109.0f};
    glm::vec2 offset = {600.0f, 557.0f};
    Renderer->DrawSpriteOffset(bgSprite, topLeft, offset,
                               {1.0f, 1.0f, 1.0f, transparency}, {scale, scale},
                               spinAngle);
  }
  if ((ScrWork[6414] == 0) || (ScrWork[6414] == 2)) {
    bgSprite.Bounds.X = 0;
    glm::vec2 topLeft = {1696.0f, -109.0f};
    glm::vec2 offset = {424.0f, 557.0f};
    Renderer->DrawSpriteOffset(bgSprite, topLeft, offset,
                               {1.0f, 1.0f, 1.0f, transparency}, {scale, scale},
                               spinAngle);
  }
}

void DelusionTrigger::RenderStable(Sprite& bgSprite, float spinAngle,
                                   int spinAlpha) {
  if (DelusionState == DS_Neutral) {
    if ((ScrWork[6414] == 0) || (ScrWork[6414] == 1)) {
      bgSprite.Bounds.X = 1024;
      Renderer->DrawSpriteOffset(bgSprite, {-800, -109}, {600, 557},
                                 {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
                                 {1.0, 1.0}, spinAngle);
    }
    if ((ScrWork[6414] == 0) || (ScrWork[6414] == 2)) {
      bgSprite.Bounds.X = 0;
      Renderer->DrawSpriteOffset(bgSprite, {1696.0f, -109.0f}, {424, 557},
                                 {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
                                 {1.0, 1.0}, spinAngle);
    }
  } else if (DelusionState == DS_Positive) {
    bgSprite.Bounds.X = 1024;
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{448, -109.0f}, glm::vec2{600, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1.8, 1.8}, spinAngle);
  } else if (DelusionState == DS_Negative) {
    bgSprite.Bounds.X = 0;
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{448, -109.0f}, glm::vec2{424, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1.8, 1.8}, spinAngle);
  }
}

void DelusionTrigger::RenderNeutralToPositiveTransition(Sprite& bgSprite,
                                                        float spinAngle,
                                                        int spinAlpha) {
  bgSprite.Bounds.X = 1024;
  float topLeftX = 448.0f - (float)((ScrWork[6415] * 1248) >> 6);
  Renderer->DrawSpriteOffset(
      bgSprite, glm::vec2{topLeftX, -109.0f}, {600, 557},
      {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
      {1.8 - ScrWork[6415] * 0.8 / 64, 1.8 - ScrWork[6415] * 0.8 / 64},
      spinAngle);
  if (ScrWork[6414] == 0) {
    bgSprite.Bounds.X = 0;
    float centerX = 2944.0f - (float)((ScrWork[6415] * 1248) >> 6);
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{centerX, -109.0f}, glm::vec2{424.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderNeutralToNegativeTransition(Sprite& bgSprite,
                                                        float spinAngle,
                                                        int spinAlpha) {
  bgSprite.Bounds.X = 0;
  float topLeftX = (float)((ScrWork[6415] * 1248) >> 6) + 448.0f;
  Renderer->DrawSpriteOffset(
      bgSprite, glm::vec2{topLeftX, -109.0f}, {424, 557},
      {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
      {1.8 - ScrWork[6415] * 0.8 / 64, 1.8 - ScrWork[6415] * 0.8 / 64},
      spinAngle);
  if (ScrWork[6414] == 0) {
    bgSprite.Bounds.X = 1024;
    float topLeftX = (float)((ScrWork[6415] * 1248) >> 6) - 2048.0f;
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{topLeftX, -109.0f}, glm::vec2{600.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderPositiveToNeutralTransition(Sprite& bgSprite,
                                                        float spinAngle,
                                                        int spinAlpha) {
  float scale = 1.0f + ScrWork[6415] * 0.8f / 64.0f;
  bgSprite.Bounds.X = 1024;

  float topLeftX = (float)((ScrWork[6415] * 1248) >> 6) - 800.0f;
  Renderer->DrawSpriteOffset(bgSprite, glm::vec2{topLeftX, -109.0f}, {600, 557},
                             {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
                             {scale, scale}, spinAngle);
  if (ScrWork[6414] == 0) {
    bgSprite.Bounds.X = 0;
    float centerX = (float)((ScrWork[6415] * 1248) >> 6) + 1696.0f;
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{centerX, -109.0f}, glm::vec2{424.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderNegativeToNeutralTransition(Sprite& bgSprite,
                                                        float spinAngle,
                                                        int spinAlpha) {
  float scale = 1.0f + (float)(ScrWork[6415]) * 0.8f / 64.0f;
  bgSprite.Bounds.X = 0;

  float topLeftX = 1696.0f - (float)((ScrWork[6415] * 1248) >> 6);
  Renderer->DrawSpriteOffset(bgSprite, glm::vec2{topLeftX, -109.0f}, {424, 557},
                             {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
                             {scale, scale}, spinAngle);
  if (ScrWork[6414] == 0) {
    bgSprite.Bounds.X = 1024;
    float centerX = -800.0f - (float)((ScrWork[6415] * 1248) >> 6);
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{centerX, -109.0f}, glm::vec2{600.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderEndNeutralTransition(Sprite& bgSprite,
                                                 float spinAngle,
                                                 int spinAlpha) {
  if ((ScrWork[6414] == 0) || (ScrWork[6414] == 1)) {
    bgSprite.Bounds.X = 1024;
    float topLeftX = (float)((ScrWork[6413] * 1248) >> 5) - 2048.0f;
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{topLeftX, -109.0f}, glm::vec2{600.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
  if ((ScrWork[6414] == 0) || (ScrWork[6414] == 2)) {
    bgSprite.Bounds.X = 0;
    float topLeftX = 2944.0f - (float)((ScrWork[6415] * 1248) >> 5);
    Renderer->DrawSpriteOffset(
        bgSprite, glm::vec2{topLeftX, -109.0f}, glm::vec2{424.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::Render() {
  if (ScrWork[6413] == 0) {
    return;
  }
  MtrgAlphaCt = (MtrgAlphaCt + 1) & 0x1f;
  MtrgAng = (MtrgAng + 100) & 0xffff;
  int mtrgSelActualBufferId = Vm::Interface::GetBufferId(ScrWork[6410]);
  int bgMtrgSelIdx = ScrWork[SW_BG1SURF + mtrgSelActualBufferId];
  float mtrgSelAlpha = ((ScrWork[6413] * 8) & 0xffffff) / 256.0f;
  if (ScrWork[6413] < 32) {
    float scale = 2.0f - (ScrWork[6413] / 32.0f);
    Renderer->DrawSpriteOffset(Backgrounds2D[bgMtrgSelIdx]->BgSprite,
                               glm::vec2{0.0f, 0.0f}, glm::vec2{960.0f, 413.0f},
                               glm::vec4{1.0f, 1.0f, 1.0f, mtrgSelAlpha},
                               {scale, scale});
  } else {
    Renderer->DrawSprite(Backgrounds2D[bgMtrgSelIdx]->BgSprite,
                         {0, 0, 1920, 1080});
  }

  float spinAngle = MtrgAng / 65535.0f * 2.0f * float(M_PI);
  int mtrgNegaPosiBufId = Vm::Interface::GetBufferId(ScrWork[6411]);
  int bgMtrgNegaPosiIdx = ScrWork[SW_BG1SURF + mtrgNegaPosiBufId];
  Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite.Bounds.Width = 1024;
  Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite.Bounds.Height = 1024;

  int spinAlpha = (MtrgAlphaCt < 16) ? MtrgAlphaCt : 32 - MtrgAlphaCt;
  spinAlpha = (spinAlpha * 192 >> 4) + 64;

  if (GetFlag(2820)) {
    if (LastDelusionState == DelusionState) {
      RenderStable(Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite, spinAngle,
                   spinAlpha);
    } else if (LastDelusionState == 0xff) {
      RenderStartTransition(Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite,
                            spinAngle, spinAlpha);
    } else if (LastDelusionState == DS_Neutral) {
      if (DelusionState == DS_Negative) {
        RenderNeutralToNegativeTransition(
            Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite, spinAngle, spinAlpha);
      } else if (DelusionState == DS_Positive) {
        RenderNeutralToPositiveTransition(
            Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite, spinAngle, spinAlpha);
      }

    } else if (LastDelusionState == DS_Positive) {
      RenderPositiveToNeutralTransition(
          Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite, spinAngle, spinAlpha);
    } else if (LastDelusionState == DS_Negative) {
      RenderNegativeToNeutralTransition(
          Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite, spinAngle, spinAlpha);
    }
  } else if (DelusionState == DS_Neutral) {
    RenderEndNeutralTransition(Backgrounds2D[bgMtrgNegaPosiIdx]->BgSprite,
                               spinAngle, spinAlpha);
  }
}

};  // namespace CCLCC
};  // namespace Impacto