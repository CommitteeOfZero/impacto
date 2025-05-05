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
  const glm::vec2 scale((ScrWork[SW_DELUSION_SPIN_COUNTER] + 64) / 64.0f);
  const glm::vec4 tint = {
      1.0f, 1.0f, 1.0f,
      ((64 - ScrWork[SW_DELUSION_SPIN_COUNTER]) * spinAlpha >> 6) / 256.0f};

  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
    const glm::vec2 topLeft = {-800, -109};
    const glm::vec2 offset = {600, 557};
    const glm::mat4 transformation =
        TransformationMatrix(offset, scale, offset, spinAngle, topLeft);
    Renderer->DrawSprite(PositiveDelusionSprite, transformation, tint);
  }

  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
    const glm::vec2 topLeft = {1696, -109};
    const glm::vec2 offset = {424, 557};
    const glm::mat4 transformation =
        TransformationMatrix(offset, scale, offset, spinAngle, topLeft);
    Renderer->DrawSprite(NegativeDelusionSprite, transformation, tint);
  }
}

void DelusionTrigger::RenderStable(float spinAngle, int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  if (DelusionState == DS_Neutral) {
    if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
        (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
      const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                   .Rotate(spinAngle, {600, 557})
                                   .Translate({-800, -109});
      Renderer->DrawSprite(PositiveDelusionSprite, dest, tint);
    }

    if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
        (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
      const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                   .Rotate(spinAngle, {424, 557})
                                   .Translate({1696, -109});
      Renderer->DrawSprite(NegativeDelusionSprite, dest, tint);
    }

  } else if (DelusionState == DS_Positive) {
    const glm::mat4 transformation = TransformationMatrix(
        {600, 557}, {1.8f, 1.8f}, {600, 557}, spinAngle, {448, -109});
    Renderer->DrawSprite(PositiveDelusionSprite, transformation, tint);

  } else if (DelusionState == DS_Negative) {
    const glm::mat4 transformation = TransformationMatrix(
        {424, 557}, {1.8f, 1.8f}, {424, 557}, spinAngle, {448, -109});
    Renderer->DrawSprite(NegativeDelusionSprite, transformation, tint);
  }
}

void DelusionTrigger::RenderNeutralToPositiveTransition(float spinAngle,
                                                        int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  const glm::vec2 topLeft = {
      448 - ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6), -109};
  const glm::vec2 scale(1.8f -
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({600, 557}, scale, {600, 557}, spinAngle, topLeft);
  Renderer->DrawSprite(PositiveDelusionSprite, transformation, tint);

  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    const glm::vec2 center = {
        2944 - ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6), -109};
    const CornersQuad dest = NegativeDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {424, 557})
                                 .Translate(center);
    Renderer->DrawSprite(NegativeDelusionSprite, dest, tint);
  }
}

void DelusionTrigger::RenderNeutralToNegativeTransition(float spinAngle,
                                                        int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  const glm::vec2 topLeft = {
      ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) + 448, -109};
  const glm::vec2 scale(1.8f -
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({424, 557}, scale, {424, 557}, spinAngle, topLeft);
  Renderer->DrawSprite(NegativeDelusionSprite, transformation, tint);

  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    const glm::vec2 topLeftPositive = {
        ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) - 2048, -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {600, 557})
                                 .Translate(topLeftPositive);
    Renderer->DrawSprite(PositiveDelusionSprite, dest, tint);
  }
}

void DelusionTrigger::RenderPositiveToNeutralTransition(float spinAngle,
                                                        int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  const glm::vec2 topLeft = {
      ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) - 800, -109};
  const glm::vec2 scale(1.0f +
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({600, 557}, scale, {600, 557}, spinAngle, topLeft);
  Renderer->DrawSprite(PositiveDelusionSprite, transformation, tint);

  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    const glm::vec2 center = {
        ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) + 1696, -109};
    const CornersQuad dest = NegativeDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {424, 557})
                                 .Translate(center);
    Renderer->DrawSprite(NegativeDelusionSprite, dest, tint);
  }
}

void DelusionTrigger::RenderNegativeToNeutralTransition(float spinAngle,
                                                        int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  const glm::vec2 topLeft = {
      1696 - ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6), -109};
  const glm::vec2 scale(1.0f +
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({424, 557}, scale, {424, 557}, spinAngle, topLeft);
  Renderer->DrawSprite(NegativeDelusionSprite, transformation, tint);

  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    const glm::vec2 center = {
        -800 - ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6), -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {600, 557})
                                 .Translate(center);
    Renderer->DrawSprite(PositiveDelusionSprite, dest, tint);
  }
}

void DelusionTrigger::RenderEndNeutralTransition(float spinAngle,
                                                 int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
    const glm::vec2 topLeft = {
        ((ScrWork[SW_DELUSION_BG_COUNTER] * 1248) >> 5) - 2048, -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {600, 557})
                                 .Translate(topLeft);
    Renderer->DrawSprite(PositiveDelusionSprite, dest, tint);
  }

  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
    const glm::vec2 topLeft = {
        2944 - ((ScrWork[SW_DELUSION_BG_COUNTER] * 1248) >> 5), -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {424, 557})
                                 .Translate(topLeft);
    Renderer->DrawSprite(NegativeDelusionSprite, dest, tint);
  }
}

void DelusionTrigger::Render() {
  if (ScrWork[SW_DELUSION_BG_COUNTER] == 0) {
    return;
  }
  const glm::vec4 mtrgSelTint = {
      1.0f, 1.0f, 1.0f,
      ((ScrWork[SW_DELUSION_BG_COUNTER] * 8) & 0xffffff) / 256.0f};
  if (ScrWork[SW_DELUSION_BG_COUNTER] < 32) {
    const glm::vec2 scale(2.0f - (ScrWork[SW_DELUSION_BG_COUNTER] / 32.0f));
    const CornersQuad dest =
        BgOverlaySprite.ScaledBounds().Scale(scale, {960, 413});
    Renderer->DrawSprite(BgOverlaySprite, dest, mtrgSelTint);
  } else {
    Renderer->DrawSprite(BgOverlaySprite, RectF{0, 0, 1920, 1080});
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