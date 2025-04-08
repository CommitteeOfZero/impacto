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
    : DelusionTriggerBase(ScrWork[SW_DELUSION_STATE], UiState::Hidden) {
  PositiveDelusionSprite.Bounds.X = 1024;
  PositiveDelusionSprite.Bounds.Y = 0;
  PositiveDelusionSprite.Bounds.Width = 1024;
  PositiveDelusionSprite.Bounds.Height = 1024;

  NegativeDelusionSprite.Bounds.X = 0;
  NegativeDelusionSprite.Bounds.Y = 0;
  NegativeDelusionSprite.Bounds.Width = 1024;
  NegativeDelusionSprite.Bounds.Height = 1024;
}

bool DelusionTrigger::Show(int bgOverlayBgBufferId, int circlesBgBufferId,
                           int availableDelusions) {
  if (GetFlag(SF_MOVIEPLAY) && GetFlag(SF_MOVIE_DRAWWAIT)) {
    return false;
  }

  ScrWork[SW_DELUSION_OVERLAY_BUF] = bgOverlayBgBufferId;
  ScrWork[SW_DELUSION_CIRCLE_BUF] = circlesBgBufferId;

  DelusionState = DS_Neutral;
  ScrWork[SW_DELUSION_BG_COUNTER] = 0;
  LastDelusionState = 0xff;
  ScrWork[SW_DELUSION_SPIN_COUNTER] = 0x40;
  ScrWork[SW_DELUSION_LIMIT] = availableDelusions;
  SetFlag(SF_DELUSIONACTIVE, 1);
  ScrWork[6344] = 48;
  SetFlag(SF_DELUSIONSELECTED, 0);
  ScrWork[6418] = 960;
  if (!Video::Players[0]->IsPlaying) {
    ScrWork[SW_DELUSION_BG_COUNTER] = 32;
  }
  return true;
}

void DelusionTrigger::Hide() {
  SetFlag(SF_DELUSIONACTIVE, 0);
  ScrWork[6344] = 0;
}

bool DelusionTrigger::CheckTransitionAnimationComplete() {
  return LastDelusionState == DelusionState;
}

bool DelusionTrigger::CheckStartTransitionComplete() {
  return LastDelusionState != 0xff;
}

void DelusionTrigger::Update(float dt) {
  auto onRightTrigger = [this]() {
    if (DelusionState == DS_Neutral) {
      if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both ||
          ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly) {
        Audio::Channels[Audio::AC_SSE]->Play("sysse", 11, false, 0.0f);
        ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
        DelusionState = DS_Negative;
      }
    } else if (DelusionState == DS_Positive) {
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 12, false, 0.0f);
      ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
      DelusionState = DS_Neutral;
    }
  };

  auto onLeftTrigger = [this]() {
    if (DelusionState == DS_Neutral) {
      if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both ||
          ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly) {
        Audio::Channels[Audio::AC_SSE]->Play("sysse", 10, false, 0.0f);
        ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
        DelusionState = DS_Positive;
      }
    } else if (DelusionState == DS_Negative) {
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 12, false, 0.0f);
      ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
      DelusionState = DS_Neutral;
    }
  };

  if (!GetFlag(SF_DELUSIONACTIVE)) {
    if ((GetFlag(SF_MOVIEPLAY) && GetFlag(SF_MOVIE_DRAWWAIT)) ||
        ScrWork[SW_DELUSION_BG_COUNTER] == 0) {
      if (BgOverlaySprite.Sheet.Texture) BgOverlaySprite = Sprite{};
      if (PositiveDelusionSprite.Sheet.Texture)
        PositiveDelusionSprite.Sheet = SpriteSheet{};
      if (NegativeDelusionSprite.Sheet.Texture)
        NegativeDelusionSprite.Sheet = SpriteSheet{};
      return;
    }
    if (GetFlag(SF_MESALLSKIP)) {
      ScrWork[SW_DELUSION_BG_COUNTER] = 0;
    }
    if (ScrWork[SW_DELUSION_BG_COUNTER] > 0) ScrWork[SW_DELUSION_BG_COUNTER]--;
    return;
  } else if (ScrWork[SW_DELUSION_BG_COUNTER]) {
    MtrgAlphaCt = (MtrgAlphaCt + 1) & 0x1f;
    MtrgAng = (MtrgAng + 100) & 0xffff;

    if (BgOverlaySprite.Sheet.Texture == 0) {
      int bgOverlayBGIdx =
          ScrWork[SW_BG1SURF +
                  Vm::Interface::GetBufferId(ScrWork[SW_DELUSION_OVERLAY_BUF])];
      BgOverlaySprite = Backgrounds2D[bgOverlayBGIdx]->BgSprite;
    }

    int delusionCirclesBGIdx =
        ScrWork[SW_BG1SURF +
                Vm::Interface::GetBufferId(ScrWork[SW_DELUSION_CIRCLE_BUF])];
    if (PositiveDelusionSprite.Sheet.Texture == 0) {
      PositiveDelusionSprite.Sheet =
          Backgrounds2D[delusionCirclesBGIdx]->BgSprite.Sheet;
    }
    if (NegativeDelusionSprite.Sheet.Texture == 0) {
      NegativeDelusionSprite.Sheet =
          Backgrounds2D[delusionCirclesBGIdx]->BgSprite.Sheet;
    }
  }

  if (ScrWork[SW_DELUSION_BG_COUNTER] < 32) {
    ScrWork[SW_DELUSION_BG_COUNTER]++;
    return;
  }
  if (GetFlag(SF_DELUSIONSELECTED) || ScrWork[SW_SYSSUBMENUCT] != 0 ||
      ScrWork[SW_SYSMENUCT] != 0) {
    return;
  }

  if (PADcustom[14] & PADinputButtonWentDown || (GetFlag(SF_MESALLSKIP)) != 0) {
    Video::Players[0]->Stop();
  }
  if (LastDelusionState == 0xff && GetFlag(SF_MOVIEPLAY) &&
      ScrWork[SW_MOVIETOTALFRAME] - 13 < ScrWork[SW_MOVIEFRAME] &&
      LastDelusionState != DelusionState &&
      ScrWork[SW_DELUSION_SPIN_COUNTER] == 0) {
    if (ScrWork[SW_DELUSION_SPIN_COUNTER] != 0) {
      ScrWork[SW_DELUSION_SPIN_COUNTER] -= 2;
    }
    if (ScrWork[SW_DELUSION_SPIN_COUNTER] == 0) {
      LastDelusionState = DelusionState;
    }
  }
  if (Video::Players[0]->IsPlaying) return;
  if ((ScrWork[6333] == 0xffff) || (ScrWork[6333] == 0xffff)) {
    SetFlag(2486, 0);
    ScrWork[6333] = 0xffff;
    ScrWork[6336] = 0xffff;
  }
  auto leftTrigger = GetControlState(CT_DelusionTriggerL);
  auto rightTrigger = GetControlState(CT_DelusionTriggerR);

  if (LastDelusionState == DelusionState) {
    if (!leftTrigger && !rightTrigger) return;
    if (!leftTrigger && rightTrigger) {
      onRightTrigger();
    }
    if (leftTrigger && !rightTrigger) {
      onLeftTrigger();
    }
  } else {
    if (ScrWork[SW_DELUSION_SPIN_COUNTER] != 0) {
      ScrWork[SW_DELUSION_SPIN_COUNTER] -= 2;
    }
    if (ScrWork[SW_DELUSION_SPIN_COUNTER] == 0) {
      LastDelusionState = DelusionState;
    }
  }
}

void DelusionTrigger::RenderStartTransition(float spinAngle, int spinAlpha) {
  float scale = (ScrWork[SW_DELUSION_SPIN_COUNTER] + 64) / 64.0f;
  float transparency =
      ((64 - ScrWork[SW_DELUSION_SPIN_COUNTER]) * spinAlpha >> 6) / 256.0f;
  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
    glm::vec2 topLeft = {-800.0f, -109.0f};
    glm::vec2 offset = {600.0f, 557.0f};
    Renderer->DrawSpriteOffset(PositiveDelusionSprite, topLeft, offset,
                               {1.0f, 1.0f, 1.0f, transparency}, {scale, scale},
                               spinAngle);
  }
  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
    glm::vec2 topLeft = {1696.0f, -109.0f};
    glm::vec2 offset = {424.0f, 557.0f};
    Renderer->DrawSpriteOffset(NegativeDelusionSprite, topLeft, offset,
                               {1.0f, 1.0f, 1.0f, transparency}, {scale, scale},
                               spinAngle);
  }
}

void DelusionTrigger::RenderStable(float spinAngle, int spinAlpha) {
  if (DelusionState == DS_Neutral) {
    if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
        (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
      Renderer->DrawSpriteOffset(
          PositiveDelusionSprite, {-800, -109}, {600, 557},
          {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1.0, 1.0}, spinAngle);
    }
    if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
        (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
      Renderer->DrawSpriteOffset(
          NegativeDelusionSprite, {1696.0f, -109.0f}, {424, 557},
          {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1.0, 1.0}, spinAngle);
    }
  } else if (DelusionState == DS_Positive) {
    Renderer->DrawSpriteOffset(
        PositiveDelusionSprite, glm::vec2{448, -109.0f}, glm::vec2{600, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1.8, 1.8}, spinAngle);
  } else if (DelusionState == DS_Negative) {
    Renderer->DrawSpriteOffset(
        NegativeDelusionSprite, glm::vec2{448, -109.0f}, glm::vec2{424, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1.8, 1.8}, spinAngle);
  }
}

void DelusionTrigger::RenderNeutralToPositiveTransition(float spinAngle,
                                                        int spinAlpha) {
  float topLeftX =
      448.0f - (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6);
  Renderer->DrawSpriteOffset(
      PositiveDelusionSprite, glm::vec2{topLeftX, -109.0f}, {600, 557},
      {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
      {1.8 - ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8 / 64,
       1.8 - ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8 / 64},
      spinAngle);
  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    float centerX =
        2944.0f - (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6);
    Renderer->DrawSpriteOffset(
        NegativeDelusionSprite, glm::vec2{centerX, -109.0f},
        glm::vec2{424.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderNeutralToNegativeTransition(float spinAngle,
                                                        int spinAlpha) {
  float topLeftX =
      (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) + 448.0f;
  Renderer->DrawSpriteOffset(
      NegativeDelusionSprite, glm::vec2{topLeftX, -109.0f}, {424, 557},
      {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f},
      {1.8 - ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8 / 64,
       1.8 - ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8 / 64},
      spinAngle);
  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    float topLeftX =
        (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) - 2048.0f;
    Renderer->DrawSpriteOffset(
        PositiveDelusionSprite, glm::vec2{topLeftX, -109.0f},
        glm::vec2{600.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderPositiveToNeutralTransition(float spinAngle,
                                                        int spinAlpha) {
  float scale = 1.0f + ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f;

  float topLeftX =
      (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) - 800.0f;
  Renderer->DrawSpriteOffset(
      PositiveDelusionSprite, glm::vec2{topLeftX, -109.0f}, {600, 557},
      {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {scale, scale}, spinAngle);
  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    float centerX =
        (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) + 1696.0f;
    Renderer->DrawSpriteOffset(
        NegativeDelusionSprite, glm::vec2{centerX, -109.0f},
        glm::vec2{424.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderNegativeToNeutralTransition(float spinAngle,
                                                        int spinAlpha) {
  float scale =
      1.0f + (float)(ScrWork[SW_DELUSION_SPIN_COUNTER]) * 0.8f / 64.0f;

  float topLeftX =
      1696.0f - (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6);
  Renderer->DrawSpriteOffset(
      NegativeDelusionSprite, glm::vec2{topLeftX, -109.0f}, {424, 557},
      {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {scale, scale}, spinAngle);
  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    float centerX =
        -800.0f - (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6);
    Renderer->DrawSpriteOffset(
        PositiveDelusionSprite, glm::vec2{centerX, -109.0f},
        glm::vec2{600.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::RenderEndNeutralTransition(float spinAngle,
                                                 int spinAlpha) {
  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
    float topLeftX =
        (float)((ScrWork[SW_DELUSION_BG_COUNTER] * 1248) >> 5) - 2048.0f;
    Renderer->DrawSpriteOffset(
        PositiveDelusionSprite, glm::vec2{topLeftX, -109.0f},
        glm::vec2{600.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
    float topLeftX =
        2944.0f - (float)((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 5);
    Renderer->DrawSpriteOffset(
        NegativeDelusionSprite, glm::vec2{topLeftX, -109.0f},
        glm::vec2{424.0f, 557.0f},
        {1.0f, 1.0f, 1.0f, (float)spinAlpha / 256.0f}, {1, 1}, spinAngle);
  }
}

void DelusionTrigger::Render() {
  if (ScrWork[SW_DELUSION_BG_COUNTER] == 0) {
    return;
  }
  float mtrgSelAlpha =
      ((ScrWork[SW_DELUSION_BG_COUNTER] * 8) & 0xffffff) / 256.0f;
  if (ScrWork[SW_DELUSION_BG_COUNTER] < 32) {
    float scale = 2.0f - (ScrWork[SW_DELUSION_BG_COUNTER] / 32.0f);
    Renderer->DrawSpriteOffset(
        BgOverlaySprite, glm::vec2{0.0f, 0.0f}, glm::vec2{960.0f, 413.0f},
        glm::vec4{1.0f, 1.0f, 1.0f, mtrgSelAlpha}, {scale, scale});
  } else {
    Renderer->DrawSprite(BgOverlaySprite, {0, 0, 1920, 1080});
  }

  float spinAngle = MtrgAng / 65535.0f * 2.0f * float(M_PI);

  int spinAlpha = (MtrgAlphaCt < 16) ? MtrgAlphaCt : 32 - MtrgAlphaCt;
  spinAlpha = (spinAlpha * 192 >> 4) + 64;

  if (GetFlag(SF_DELUSIONACTIVE)) {
    if (LastDelusionState == DelusionState) {
      RenderStable(spinAngle, spinAlpha);
    } else if (LastDelusionState == 0xff) {
      RenderStartTransition(spinAngle, spinAlpha);
    } else if (LastDelusionState == DS_Neutral) {
      if (DelusionState == DS_Negative) {
        RenderNeutralToNegativeTransition(spinAngle, spinAlpha);
      } else if (DelusionState == DS_Positive) {
        RenderNeutralToPositiveTransition(spinAngle, spinAlpha);
      }

    } else if (LastDelusionState == DS_Positive) {
      RenderPositiveToNeutralTransition(spinAngle, spinAlpha);
    } else if (LastDelusionState == DS_Negative) {
      RenderNegativeToNeutralTransition(spinAngle, spinAlpha);
    }
  } else if (DelusionState == DS_Neutral) {
    RenderEndNeutralTransition(spinAngle, spinAlpha);
  }
}

};  // namespace CCLCC
};  // namespace Impacto