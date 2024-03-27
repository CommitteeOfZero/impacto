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

bool DelusionTrigger::Show(int param_1, int param_2, int param_3) {
  if (Video::Players[0]->IsPlaying && GetFlag(1236)) {
    return false;
  }
  ScrWork[6410] = param_1;
  ScrWork[6411] = param_2;
  ScrWork[6412] = 0;
  ScrWork[6413] = 0;
  ScrWork[6416] = 0xff;
  ScrWork[6415] = 0x40;
  ScrWork[6414] = param_3;
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
      if (GetFlag(2821) || ScrWork[2147] != 0 || ScrWork[2142] != 0) {
        return;
      }

      if (PADcustom[14] & PADinputButtonWentDown ||
          (GetFlag(SF_MESALLSKIP)) != 0) {
        Video::Players[0]->Stop();
      }
      if (ScrWork[6416] == 0xff && Video::Players[0]->IsPlaying &&
          ScrWork[6345] - 13 < ScrWork[6343] &&
          ScrWork[6416] != DelusionState && ScrWork[6415] == 0) {
        if (ScrWork[6415] != 0) {
          ScrWork[6415] -= 2;
        }
        if (ScrWork[6415] == 0) {
          ScrWork[6416] = DelusionState;
        }
      }
      if (!Video::Players[0]->IsPlaying) {
        if ((ScrWork[6333] == 0xffff) || (ScrWork[6333] == 0xffff)) {
          Video::VideoShutdown();
          SetFlag(2486, 0);
          ScrWork[6333] = 0xffff;
          ScrWork[6336] = 0xffff;
        }
        if (ScrWork[6416] == DelusionState) {
          if (!(PADinputButtonWentDown & PAD1L2)) {
            if (PADinputButtonWentDown & PAD1R2) {
              if (DelusionState == 0) {
                if (ScrWork[6414] == 0 || ScrWork[6414] == 2) {
                  Audio::Channels[Audio::AC_SSE]->Play("sysse", 11, false,
                                                       0.0f);
                  ScrWork[6415] = 64;
                  DelusionState = 2;
                }
              } else if (DelusionState == 1) {
                Audio::Channels[Audio::AC_SSE]->Play("sysse", 12, false, 0.0f);
                ScrWork[6415] = 64;
                DelusionState = 0;
              }
            }
          } else if (DelusionState == 0) {
            if (ScrWork[6414] == 0 || ScrWork[6414] == 1) {
              Audio::Channels[Audio::AC_SSE]->Play("sysse", 10, false, 0.0f);
              ScrWork[6415] = 64;
              DelusionState = 1;
            }
          } else if (DelusionState == 2) {
            Audio::Channels[Audio::AC_SSE]->Play("sysse", 12, false, 0.0f);
            ScrWork[6415] = 64;
            DelusionState = 0;
          }
        } else {
          if (ScrWork[6415] != 0) {
            ScrWork[6415] -= 2;
          }
          if (ScrWork[6415] == 0) {
            ScrWork[6416] = DelusionState;
          }
        }
      }
    }
  }
}

void DelusionTrigger::Render() {
  if (ScrWork[6413] == 0) {
    return;
  }
  MtrgAlphaCt = (MtrgAlphaCt + 1) & 0x1f;
  MtrgAng = (MtrgAng + 100) & 0xffff;
  int actualBufId = Vm::Interface::GetBufferId(ScrWork[6410]);
  int bgBufId = ScrWork[SW_BG1SURF + actualBufId];
  if (ScrWork[6413] < 32) {
    double scale = (ScrWork[6413] * -0.03125 + 2.0);
    Renderer->DrawSpriteOffset(
        Backgrounds2D[bgBufId]->BgSprite, glm::vec2{0.0f, 0.0f},
        glm::vec2{960.0f, 413.0f},
        glm::vec4{1.0f, 1.0f, 1.0f, (ScrWork[6413] * 8 & 0x7fffff8) / 256.0f},
        {scale, scale});
  } else {
    Renderer->DrawSprite(Backgrounds2D[bgBufId]->BgSprite, {0, 0, 1920, 1080});
  }

  float spinAngle = MtrgAng / 65535.0f * 2.0f * float(M_PI);
  actualBufId = Vm::Interface::GetBufferId(ScrWork[6411]);
  bgBufId = ScrWork[SW_BG1SURF + actualBufId];
  Backgrounds2D[bgBufId]->BgSprite.Bounds.Width = 1024;
  Backgrounds2D[bgBufId]->BgSprite.Bounds.Height = 1024;

  int spinAlpha = (MtrgAlphaCt < 16) ? MtrgAlphaCt : 32 - MtrgAlphaCt;
  spinAlpha = (spinAlpha * 192 >> 4) + 64;

  int scale = 0x100;
  double xPos, yPos;

  if (!GetFlag(2820)) {
    if (DelusionState != 0) return;
    if (ScrWork[6414] < 2) {
      Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
      Renderer->DrawSprite(
          Backgrounds2D[bgBufId]->BgSprite,
          glm::vec2{static_cast<float>(((ScrWork[6413] * 1248) >> 5) - 2048),
                    -109.0f},
          {1.0f, 1.0f, 1.0f, static_cast<float>(spinAlpha) / 255.0f},
          {1.0f, 1.0f}, spinAngle);
    }
    if ((ScrWork[6414] | 2) != 2) return;
    Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
    Renderer->DrawSprite(
        Backgrounds2D[bgBufId]->BgSprite,
        glm::vec2{static_cast<float>(2944 - ((ScrWork[6413] * 1248) >> 5)),
                  -109.0f},
        {1.0f, 1.0f, 1.0f, static_cast<float>(spinAlpha) / 255.0f},
        {1.0f, 1.0f}, spinAngle);
  } else {
    if (ScrWork[6416] == DelusionState) {
      if (DelusionState == 0) {
        if ((ScrWork[6414] == 0) || (ScrWork[6414] == 1)) {
          Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
          Renderer->DrawSpriteOffset(
              Backgrounds2D[bgBufId]->BgSprite, {-800, -109}, {600, 557},
              {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1.0, 1.0},
              spinAngle);
        }
        if ((ScrWork[6414] == 0) || (ScrWork[6414] == 2)) {
          Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
          Renderer->DrawSpriteOffset(
              Backgrounds2D[bgBufId]->BgSprite, {1696.0f, -109.0f}, {424, 557},
              {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1.0, 1.0},
              spinAngle);
        }
      } else if (DelusionState == 1) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{448, -109.0f},
            glm::vec2{600, 557.0f},
            {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1.8, 1.8},
            spinAngle);
      } else if (DelusionState == 2) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{448, -109.0f},
            glm::vec2{424, 557.0f},
            {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1.8, 1.8},
            spinAngle);
      }
    } else if (ScrWork[6416] == 0xff) {
      if ((ScrWork[6414] == 0) || (ScrWork[6414] == 1)) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{-800, -109.0f},
            glm::vec2{600, 557.0f},
            {1.0f, 1.0f, 1.0f,
             ((64 - ScrWork[6415]) * spinAlpha >> 6) / 256.0f},
            {(ScrWork[6415] + 64) / 64.0f, (ScrWork[6415] + 64) / 64.0f},
            spinAngle);
      }
      if ((ScrWork[6414] == 0) || (ScrWork[6414] == 2)) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{1696, -109},
            glm::vec2{424, 557},
            {1.0f, 1.0f, 1.0f,
             ((64 - ScrWork[6415]) * spinAlpha >> 6) / 256.0f},
            {(ScrWork[6415] * 4 + 256) / 256.0f,
             (ScrWork[6415] * 4 + 256) / 256.0f},
            spinAngle);
      }
    } else if (ScrWork[6416] == 0) {
      if (DelusionState == 2) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
        float topLeftX = ((ScrWork[6415] * 1248) >> 6) + 448;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{topLeftX, -109.0f},
            {424, 557}, {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f},
            {1.8 - ScrWork[6415] * 0.8 / 64, 1.8 - ScrWork[6415] * 0.8 / 64},
            spinAngle);
        if (ScrWork[6414] == 0) {
          Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
          float centerX = ((ScrWork[6415] * 1248) >> 6) - 2048;
          Renderer->DrawSpriteOffset(
              Backgrounds2D[bgBufId]->BgSprite, glm::vec2{centerX, -109.0f},
              glm::vec2{600.0f, 557.0f},
              {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1, 1}, spinAngle);
        }
      }
      if (DelusionState == 1) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
        float topLeftX = 448 - ((ScrWork[6415] * 1248) >> 6);
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{topLeftX, -109.0f},
            {600, 557}, {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f},
            {1.8 - ScrWork[6415] * 0.8 / 64, 1.8 - ScrWork[6415] * 0.8 / 64},
            spinAngle);
        if (ScrWork[6414] == 0) {
          Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
          float centerX = 2944 - ((ScrWork[6415] * 1248) >> 6);
          Renderer->DrawSpriteOffset(
              Backgrounds2D[bgBufId]->BgSprite, glm::vec2{centerX, -109.0f},
              glm::vec2{424.0f, 557.0f},
              {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1, 1}, spinAngle);
        }
      }
    } else if (ScrWork[6416] == 1) {
      float scale = 1.0 + ScrWork[6415] * 0.8 / 64;
      Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;

      float topLeftX = ((ScrWork[6415] * 1248) >> 6) - 800;
      Renderer->DrawSpriteOffset(Backgrounds2D[bgBufId]->BgSprite,
                                 glm::vec2{topLeftX, -109.0f}, {600, 557},
                                 {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f},
                                 {scale, scale}, spinAngle);
      if ((ScrWork[6414] == 0)) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
        float centerX = ((ScrWork[6415] * 1248) >> 6) + 1696;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{centerX, -109.0f},
            glm::vec2{424.0f, 557.0f},
            {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1, 1}, spinAngle);
      }
    } else if (ScrWork[6416] == 2) {
      float scale = 1.0 + ScrWork[6415] * 0.8 / 64;
      Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;

      float topLeftX = 1696 - ((ScrWork[6415] * 1248) >> 6);
      Renderer->DrawSpriteOffset(Backgrounds2D[bgBufId]->BgSprite,
                                 glm::vec2{topLeftX, -109.0f}, {424, 557},
                                 {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f},
                                 {scale, scale}, spinAngle);
      if ((ScrWork[6414] == 0)) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
        float centerX = -800 - ((ScrWork[6415] * 1248) >> 6);
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{centerX, -109.0f},
            glm::vec2{600.0f, 557.0f},
            {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1, 1}, spinAngle);
      }
    } else if (DelusionState == 0) {
      if ((ScrWork[6414] == 0) || (ScrWork[6414] == 1)) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 1024;
        float centerX = ((ScrWork[6415] * 1248) >> 6) - 2048;
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{centerX, -109.0f},
            glm::vec2{600.0f, 557.0f},
            {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1, 1}, spinAngle);
      }
      if ((ScrWork[6414] == 0) || (ScrWork[6414] == 2)) {
        Backgrounds2D[bgBufId]->BgSprite.Bounds.X = 0;
        float centerX = 2944 - ((ScrWork[6415] * 1248) >> 6);
        Renderer->DrawSpriteOffset(
            Backgrounds2D[bgBufId]->BgSprite, glm::vec2{centerX, -109.0f},
            glm::vec2{424.0f, 557.0f},
            {1.0f, 1.0f, 1.0f, (float)spinAlpha / 255.0f}, {1, 1}, spinAngle);
      }
    }
  }
}
};  // namespace CCLCC
};  // namespace Impacto