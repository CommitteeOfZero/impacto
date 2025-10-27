#include "delusiontrigger.h"
#include "../../renderer/renderer.h"
#include "../../vm/interface/input.h"
#include "../../mem.h"
#include "../../profile/games/chlcc/delusiontrigger.h"
#include "../../profile/scriptvars.h"
#include "../../profile/configsystem.h"
#include "../../game.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::DelusionTrigger;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

using enum Impacto::UI::MenuState;

DelusionTrigger::DelusionTrigger()
    : TriggerOnTint(RgbIntToFloat(0xffb0ce)),
      DelusionState(ScrWork[SW_DELUSION_STATE]) {}

void DelusionTrigger::Show() {
  if (Profile::ConfigSystem::TriggerStopSkip) SkipModeEnabled = false;
  if (State != Shown && State != Showing) {
    State = Showing;
    DelusionState = DS_Neutral;
    MaskScaleFactor = 131072;
    SpinAngle = 0;
    SpinRate = 3072;
    UnderlayerAlpha = 0;
    BackgroundAlpha = 0;
    AnimationState = 0;
    AnimCounter = 0;
    TriggerOnTintAlpha = 0;
    UnderlayerXOffset = 20000;
    UnderlayerXRate = 400;
    ShakeState = 0;
    MaskOffsetX = 0;
    SetFlag(2511, 1);
    SetFlag(2512, 0);
  }
}
void DelusionTrigger::Hide() {
  if (State != Hidden && State != Hiding) {
    State = Hiding;
    AnimationState = 0;
    SetFlag(2511, 1);
    SetFlag(2512, 0);
  }
}

void DelusionTrigger::UpdateHiding() {
  if (AnimationState < 13) {
    AnimCounter++;
    switch (AnimationState) {
      case 0:
      case 4: {
        MaskScaleFactor -= 1760;
        if (AnimCounter == 5) {
          AnimationState++;
          AnimCounter = 0;
        }
      } break;
      default: {
        if (AnimCounter == 5) {
          AnimationState++;
          AnimCounter = 0;
        }
      } break;
      case 2:
      case 6: {
        MaskScaleFactor += 1760;
        if (AnimCounter == 5) {
          AnimationState++;
          AnimCounter = 0;
        }
      } break;
      case 8: {
        MaskScaleFactor -= 3072;
        if (AnimCounter == 14) {
          AnimationState++;
          AnimCounter = 0;
        }
      } break;
      case 9: {
        if (AnimCounter == 30) {
          AnimCounter = 0;
          AnimationState = (DelusionState == DS_Positive)   ? 11
                           : (DelusionState == DS_Negative) ? 12
                                                            : 10;
        }
      } break;
      case 10: {
        MaskScaleFactor += 3072;
        if (AnimCounter > 17) {
          BackgroundAlpha -= 16;
        }
        if (AnimCounter == 50) {
          AnimCounter = 0;
          AnimationState = 0;
          SetFlag(2511, 0);
          State = Hidden;
        }
      } break;
      case 11: {
        SpinRate += 24;
        MaskScaleFactor += 1536;
        if (AnimCounter > 67) {
          BackgroundAlpha -= 8;
        }
        if (AnimCounter == 100) {
          AnimCounter = 0;
          AnimationState = 0;
          SetFlag(2511, 0);
          State = Hidden;
        }
      } break;
      case 12: {
        SpinRate -= 24;
        MaskScaleFactor += 1536;
        if (AnimCounter > 67) {
          BackgroundAlpha -= 8;
        }
        if (AnimCounter == 100) {
          AnimCounter = 0;
          AnimationState = 0;
          SetFlag(2511, 0);
          State = Hidden;
        }
      } break;
    }
  }
}

void DelusionTrigger::UpdateShowing() {
  switch (AnimationState) {
    case 0: {
      SpinRate -= 24;
      MaskScaleFactor -= 736;

      if (UnderlayerAlpha < 256) {
        UnderlayerAlpha += 16;
      }
      if (BackgroundAlpha < 256) {
        BackgroundAlpha += 16;
      }

      if (SpinRate == 0) {
        AnimationState += 1;
        SpinRate = -1024;
      }
    } break;
    case 1:
    case 3:
    case 5: {
      AnimCounter += 1;
      if (AnimCounter == 5) {
        AnimationState += 1;
        SpinRate = 0;
        AnimCounter = 0;
      }
    } break;
    case 2: {
      AnimCounter += 1;
      if (AnimCounter == 15) {
        AnimationState += 1;
        AnimCounter = 0;
        SpinRate = 1024;
      }
    } break;
    case 4: {
      AnimCounter += 1;
      if (AnimCounter == 15) {
        AnimationState += 1;
        AnimCounter = 0;
        SpinRate = -1024;
      }
    } break;
    case 6: {
      AnimCounter += 1;
      if (AnimCounter == 20) {
        AnimationState += 1;
        AnimCounter = 0;
        SpinRate = 5;
      }
    } break;
    case 7: {
      MaskScaleFactor -= 1536;
      AnimCounter += 1;
      if (AnimCounter == 10) {
        AnimationState += 1;
        AnimCounter = 0;
      }
    } break;
    case 8: {
      MaskScaleFactor += 3072;
      if (MaskScaleFactor > 79999) {
        AnimationState = 0;
        AnimCounter = 0;
        SetFlag(2511, 0);
        State = Shown;
      }
    } break;
    default:
      SetFlag(2511, 0);
      State = Shown;
  }
}

void DelusionTrigger::PlayClickSound() {
  ScrWork[SW_SEREQNO + 0] = 19;
  Impacto::Audio::Channels[Audio::AC_SE0 + 0]->SetVolume(
      (ScrWork[SW_SEVOL + 0] / 100.0f) * 0.3f);
  Impacto::Audio::Channels[Audio::AC_SE0 + 0]->Play("se", 19, false, 0.0f);
}

void DelusionTrigger::UpdateShown() {
  MaskOffsetX = (ShakeState == 1)   ? 0
                : (ShakeState == 2) ? -16
                : (ShakeState == 3) ? 0
                : (ShakeState == 4) ? 18
                : (ShakeState == 5) ? 0
                : (ShakeState == 6) ? -20
                                    : 0;
  ShakeState = (ShakeState == 0) ? 0 : ShakeState - 1;
  if (PADinputButtonWentDown & PAD1L2) {
    switch (ScrWork[SW_DELUSION_STATE]) {
      case DS_Neutral:
        ScrWork[SW_DELUSION_STATE] = DS_Positive;
        PlayClickSound();
        ShakeState = 6;
        break;
      case DS_Negative:
        ScrWork[SW_DELUSION_STATE] = DS_Neutral;
        break;
      case DS_Positive:
      default:
        break;
    }
  } else if (PADinputButtonWentDown & PAD1R2) {
    switch (ScrWork[SW_DELUSION_STATE]) {
      case DS_Neutral:
        ScrWork[SW_DELUSION_STATE] = DS_Negative;
        PlayClickSound();
        ShakeState = 6;
        break;
      case DS_Positive:
        ScrWork[SW_DELUSION_STATE] = DS_Neutral;
        break;
      case DS_Negative:
      default:
        break;
    }
  }

  if (ScrWork[SW_DELUSION_STATE] != DS_Neutral) {
    if (TriggerOnTintAlpha < 104) {
      TriggerOnTintAlpha = TriggerOnTintAlpha + 4;
    }
    if (ScrWork[SW_DELUSION_STATE] == DS_Positive) {
      if (SpinRate < 40) {
        SpinRate = SpinRate + 2;
      }
      if (UnderlayerXRate < 2400) {
        UnderlayerXRate += 100;
      }
    } else if (ScrWork[SW_DELUSION_STATE] == DS_Negative) {
      if (SpinRate > -40) {
        SpinRate = SpinRate - 2;
      }
      if (UnderlayerXRate > -2400) {
        UnderlayerXRate -= 100;
      }
    }
  } else {
    if ((TriggerOnTintAlpha != 0)) {
      TriggerOnTintAlpha -= 4;
    }
    if (SpinRate < -5) {
      SpinRate = SpinRate + 2;
    } else if (SpinRate > 5) {
      SpinRate = SpinRate - 2;
    }
    if (UnderlayerXRate < -400) {
      UnderlayerXRate += 100;
    } else if (UnderlayerXRate > 400) {
      UnderlayerXRate -= 100;
    }
  }
  SetFlag(2512, 0);
  SetFlag(2511, 0);
}

void DelusionTrigger::Update(float dt) {
  if (State == Showing) {
    UpdateShowing();
  } else if (State == Hiding) {
    UpdateHiding();
  } else if (State == Shown) {
    UpdateShown();
  }

  if (State != Hidden) {
    for (UnderlayerXOffset = UnderlayerXOffset + UnderlayerXRate;
         UnderlayerXOffset < 0x2711;
         UnderlayerXOffset = UnderlayerXOffset + 10000) {
    }
    for (; 29999 < UnderlayerXOffset;
         UnderlayerXOffset = UnderlayerXOffset + -10000) {
    }
    SpinAngle = ((SpinAngle + SpinRate) & 0xffff);
  }
}

void DelusionTrigger::Render() {
  if (State == Hidden) return;
  Sprite ScaledMask = BackgroundSpriteMask;
  // float scalingFactor = 1;
  constexpr float aspect_ratio = 1280.0f / 720.0f;

  float newWidth = BackgroundSpriteMask.Bounds.Width * 65535.0f /
                   MaskScaleFactor * aspect_ratio * 0.7f;
  float newHeight =
      BackgroundSpriteMask.Bounds.Height * 65535.0f / MaskScaleFactor * 0.7f;

  float deltaWidth = newWidth - BackgroundSpriteMask.Bounds.Width;
  float deltaHeight = newHeight - BackgroundSpriteMask.Bounds.Height;

  ScaledMask.Bounds.Width = newWidth;
  ScaledMask.Bounds.Height = newHeight;

  ScaledMask.Bounds.X =
      MaskOffsetX + BackgroundSpriteMask.Bounds.X - deltaWidth / 2.0f;
  ScaledMask.Bounds.Y = BackgroundSpriteMask.Bounds.Y - deltaHeight / 2.0f;

  TriggerOnTint[3] = TriggerOnTintAlpha * BackgroundAlpha / 65536.0f;
  Renderer->DrawQuad(
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      TriggerOnTint);

  ScreenMask.Bounds.X = UnderlayerXOffset / 1000.0f;
  Renderer->DrawSprite(
      ScreenMask,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(1.0f, 1.0f, 1.0f, (BackgroundAlpha * 160) / 65536.0));

  const RectF spriteDest = {0.0f, 0.0f, Profile::DesignWidth,
                            Profile::DesignHeight};
  const CornersQuad maskDest =
      ScaledMask.Bounds.RotateAroundCenter(ScrWorkAngleToRad(SpinAngle));
  Renderer->DrawMaskedSpriteOverlay(BackgroundSprite, ScaledMask, spriteDest,
                                    maskDest, (BackgroundAlpha * 160) >> 8, 20,
                                    glm::mat4(1.0f), glm::vec4(1.0f), true);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto