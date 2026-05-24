#include "delusiontrigger.h"
#include <optional>
#include "../../profile/games/cclcc/delusiontrigger.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../profile/scriptvars.h"
#include "../../src/video/videosystem.h"
#include "../../profile/configsystem.h"
#include "../../background2d.h"
#include "../../renderer/window.h"
#include "../../text/dialoguepage.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
using namespace Impacto::Profile::CCLCC::DelusionTrigger;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

DelusionTrigger::DelusionTrigger() : DelusionState(ScrWork[SW_DELUSION_STATE]) {
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
  if (Profile::ConfigSystem::TriggerStopSkip) SkipModeEnabled = false;
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
  ResetDraggingPress();
  if (!Video::Players[0]->IsPlaying) {
    ScrWork[SW_DELUSION_BG_COUNTER] = 32;
  }
  return true;
}

void DelusionTrigger::Hide() {
  SetFlag(SF_DELUSIONACTIVE, 0);
  ScrWork[6344] = 0;
  ResetDraggingPress();
}

bool DelusionTrigger::CheckTransitionAnimationComplete() {
  return LastDelusionState == DelusionState;
}

bool DelusionTrigger::CheckStartTransitionComplete() {
  return LastDelusionState != 0xff;
}

void DelusionTrigger::TriggerRight() {
  if (DelusionState == DS_Neutral) {
    if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both ||
        ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly) {
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 11, false, 0.0f);
      ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
      DelusionState = DS_Negative;
    }
  } else if (DelusionState == DS_Positive) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 12, false, 0.0f);
    ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
    DelusionState = DS_Neutral;
  }
}

void DelusionTrigger::TriggerLeft() {
  if (DelusionState == DS_Neutral) {
    if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both ||
        ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly) {
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 10, false, 0.0f);
      ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
      DelusionState = DS_Positive;
    }
  } else if (DelusionState == DS_Negative) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 12, false, 0.0f);
    ScrWork[SW_DELUSION_SPIN_COUNTER] = 64;
    DelusionState = DS_Neutral;
  }
}

bool DelusionTrigger::CanUpdateDragging() const {
  return GetFlag(SF_DELUSIONACTIVE) && ScrWork[SW_DELUSION_BG_COUNTER] >= 32 &&
         !GetFlag(SF_DELUSIONSELECTED) && ScrWork[SW_SYSSUBMENUCT] == 0 &&
         ScrWork[SW_SYSMENUCT] == 0 && !Video::Players[0]->IsPlaying &&
         LastDelusionState == DelusionState;
}

void DelusionTrigger::ResetDraggingPress() {
  ActiveDragHitbox = DragHitbox::None;
  DragStartPos = glm::vec2(0.0f);
  DragHoldTime = 0.0f;
  DragPressPending = false;
  DragHoldActive = false;
}

std::pair<bool, bool> DelusionTrigger::UpdateDraggingTriggers(
    bool updateInputBlocking, float dt) {
  const bool pointerDown = PADinputMouseIsDown & PAD1A;
  if (!CanUpdateDragging()) {
    if (pointerDown && DragHoldActive && GetFlag(SF_DELUSIONACTIVE)) {
      ActiveDragHitbox = DragHitbox::None;
    } else {
      ResetDraggingPress();
    }
    return std::make_pair(false, false);
  }

  constexpr float movementBufferWindowFraction = 0.002f;
  constexpr float mouseAdvanceTime = 0.5f;
  const float movementBuffer =
      std::max(2.0f, std::min(Profile::DesignWidth, Profile::DesignHeight) *
                         movementBufferWindowFraction);
  std::optional<RectF> positiveHitbox;
  std::optional<RectF> negativeHitbox;

  if (DelusionState == DS_Neutral) {
    if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
        (ScrWork[SW_DELUSION_LIMIT] == Delusion_PosOnly)) {
      positiveHitbox =
          PositiveDelusionSprite.ScaledBounds().Translate({-800, -109});
    }
    if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
        (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
      negativeHitbox =
          NegativeDelusionSprite.ScaledBounds().Translate({1696, -109});
    }
  } else if (DelusionState == DS_Positive) {
    positiveHitbox = PositiveDelusionSprite.ScaledBounds()
                         .Scale({600, 557}, {1.8f, 1.8f})
                         .Translate({448, -109});
  } else if (DelusionState == DS_Negative) {
    negativeHitbox = NegativeDelusionSprite.ScaledBounds()
                         .Scale({424, 557}, {1.8f, 1.8f})
                         .Translate({448, -109});
  }

  const bool pointerWentDown = PADinputMouseWentDown & PAD1A;
  const bool isTouch = Input::CurrentInputDevice == Input::Device::Touch;
  const glm::vec2 curPos = isTouch ? Input::CurTouchPos : Input::CurMousePos;

  if (Input::CurrentInputDevice == Input::Device::Mouse &&
      ((positiveHitbox && positiveHitbox->ContainsPoint(Input::CurMousePos)) ||
       (negativeHitbox && negativeHitbox->ContainsPoint(Input::CurMousePos)))) {
    RequestCursor(CursorType::Pointer);
  }

  if (!pointerDown) {
    if (DragPressPending) {
      if (updateInputBlocking && !DragHoldActive &&
          DragHoldTime < mouseAdvanceTime) {
        PADinputMouseWentDown |= PAD1A;
      }
    }
    ResetDraggingPress();
    return std::make_pair(false, false);
  }

  if (ActiveDragHitbox == DragHitbox::None &&
      (pointerWentDown || DragHoldActive)) {
    if (positiveHitbox && positiveHitbox->ContainsPoint(curPos)) {
      ActiveDragHitbox = DragHitbox::Positive;
    } else if (negativeHitbox && negativeHitbox->ContainsPoint(curPos)) {
      ActiveDragHitbox = DragHitbox::Negative;
    }
    if (ActiveDragHitbox != DragHitbox::None && pointerWentDown) {
      DragHoldTime = 0.0f;
      DragPressPending = true;
      DragHoldActive = false;
    }
  }

  if (ActiveDragHitbox == DragHitbox::None) return std::make_pair(false, false);

  if (DragPressPending) DragHoldTime += dt;
  if (DragHoldTime >= mouseAdvanceTime ||
      glm::length(Input::CurMousePos - Input::PrevMousePos) >= movementBuffer) {
    DragHoldActive = true;
    RequestCursor(CursorType::Pointer);
  }

  if (updateInputBlocking) {
    PADinputButtonWentDown &= ~PAD1A;
    PADinputMouseWentDown &= ~PAD1A;
  }

  if (pointerWentDown) DragStartPos.x = curPos.x;

  if (!DragHoldActive) return std::make_pair(false, false);

  bool dragLeftTrigger = false;
  bool dragRightTrigger = false;
  if (ActiveDragHitbox == DragHitbox::Positive) {
    if (DelusionState == DS_Neutral) {
      dragLeftTrigger =
          curPos.x > DragStartPos.x + DragDelta;  // Select positive
    } else if (DelusionState == DS_Positive) {
      dragRightTrigger =
          curPos.x < DragStartPos.x - DragDelta;  // Return to neutral
    }
  } else if (ActiveDragHitbox == DragHitbox::Negative) {
    if (DelusionState == DS_Neutral) {
      dragRightTrigger =
          curPos.x < DragStartPos.x - DragDelta;  // Select negative
    } else if (DelusionState == DS_Negative) {
      dragLeftTrigger =
          curPos.x > DragStartPos.x + DragDelta;  // Return to neutral
    }
  }

  return std::make_pair(dragLeftTrigger, dragRightTrigger);
}

void DelusionTrigger::UpdateDragging(float dt) {
  const auto [leftTrigger, rightTrigger] = UpdateDraggingTriggers(true, dt);

  if (!leftTrigger && rightTrigger) {
    TriggerRight();
    ResetDraggingPress();
  }
  if (leftTrigger && !rightTrigger) {
    TriggerLeft();
    ResetDraggingPress();
  }
}

void DelusionTrigger::Update(float dt) {
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
          ScrWork[SW_BG1SURF + GetBufferId(ScrWork[SW_DELUSION_OVERLAY_BUF])];
      BgOverlaySprite = Backgrounds2D[bgOverlayBGIdx]->BgSprite;
    }

    int delusionCirclesBGIdx =
        ScrWork[SW_BG1SURF + GetBufferId(ScrWork[SW_DELUSION_CIRCLE_BUF])];
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
  if ((ScrWork[SW_MOVIE_PLAYNO] == 0xffff) ||
      (ScrWork[SW_MOVIE_PLAYNO] == 0xffff)) {
    SetFlag(SF_MOVIELOADPLAYFL, 0);
    ScrWork[SW_MOVIE_PLAYNO] = 0xffff;
    ScrWork[SW_MOVIE_LOADNO] = 0xffff;
  }

  if (LastDelusionState == DelusionState) {
    const bool leftTrigger = GetControlState(CT_DelusionTriggerL);
    const bool rightTrigger = GetControlState(CT_DelusionTriggerR);

    if (!(leftTrigger ^ rightTrigger)) return;

    if (leftTrigger) {
      TriggerLeft();
      ActiveDragHitbox = DragHitbox::None;
    } else {
      TriggerRight();
      ActiveDragHitbox = DragHitbox::None;
    }

  } else {
    ActiveDragHitbox = DragHitbox::None;
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
    const glm::vec2 offset = {-800, -109};
    const glm::vec2 origin = {600, 557};
    const glm::mat4 transformation =
        TransformationMatrix(origin, scale, origin, spinAngle, offset);
    Renderer->DrawSprite(PositiveDelusionSprite, transformation, tint);
  }

  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
    const glm::vec2 offset = {1696, -109};
    const glm::vec2 origin = {424, 557};
    const glm::mat4 transformation =
        TransformationMatrix(origin, scale, origin, spinAngle, offset);
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

  const glm::vec2 offset = {
      448 - ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6), -109};
  const glm::vec2 scale(1.8f -
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({600, 557}, scale, {600, 557}, spinAngle, offset);
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

  const glm::vec2 offset = {
      ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) + 448, -109};
  const glm::vec2 scale(1.8f -
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({424, 557}, scale, {424, 557}, spinAngle, offset);
  Renderer->DrawSprite(NegativeDelusionSprite, transformation, tint);

  if (ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) {
    const glm::vec2 offsetPositive = {
        ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) - 2048, -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {600, 557})
                                 .Translate(offsetPositive);
    Renderer->DrawSprite(PositiveDelusionSprite, dest, tint);
  }
}

void DelusionTrigger::RenderPositiveToNeutralTransition(float spinAngle,
                                                        int spinAlpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, spinAlpha / 256.0f};

  const glm::vec2 offset = {
      ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6) - 800, -109};
  const glm::vec2 scale(1.0f +
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({600, 557}, scale, {600, 557}, spinAngle, offset);
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

  const glm::vec2 offset = {
      1696 - ((ScrWork[SW_DELUSION_SPIN_COUNTER] * 1248) >> 6), -109};
  const glm::vec2 scale(1.0f +
                        ScrWork[SW_DELUSION_SPIN_COUNTER] * 0.8f / 64.0f);
  const glm::mat4 transformation =
      TransformationMatrix({424, 557}, scale, {424, 557}, spinAngle, offset);
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
    const glm::vec2 offset = {
        ((ScrWork[SW_DELUSION_BG_COUNTER] * 1248) >> 5) - 2048, -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {600, 557})
                                 .Translate(offset);
    Renderer->DrawSprite(PositiveDelusionSprite, dest, tint);
  }

  if ((ScrWork[SW_DELUSION_LIMIT] == Delusion_Both) ||
      (ScrWork[SW_DELUSION_LIMIT] == Delusion_NegOnly)) {
    const glm::vec2 offset = {
        2944 - ((ScrWork[SW_DELUSION_BG_COUNTER] * 1248) >> 5), -109};
    const CornersQuad dest = PositiveDelusionSprite.ScaledBounds()
                                 .Rotate(spinAngle, {424, 557})
                                 .Translate(offset);
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

  float spinAngle = ScrWorkAngleToRad(MtrgAng);

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
}  // namespace UI
};  // namespace Impacto
