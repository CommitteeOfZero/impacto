#include "delusiontrigger.h"
#include "../../renderer/renderer.h"
#include "../../vm/interface/input.h"
#include "../../mem.h"
#include "../../profile/games/chlcc/delusiontrigger.h"
#include "../../profile/scriptvars.h"
#include "../../profile/configsystem.h"
#include "../../game.h"
#include "../../profile/vm.h"
#include "../../vm/interface/scene2d.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::DelusionTrigger;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

using enum Impacto::UI::MenuState;

DelusionTrigger::DelusionTrigger()
    : DelusionState(ScrWork[SW_DELUSION_STATE]), TextSystem(*this) {}

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
    SetFlag(SF_DELUSION_UI_ANIM_WAIT, 1);
    SetFlag(SF_DELUSION_UI_ANIMSWITCH_WAIT, 0);
  }
}
void DelusionTrigger::Hide() {
  if (State != Hidden && State != Hiding) {
    State = Hiding;
    AnimationState = 0;
    SetFlag(SF_DELUSION_UI_ANIM_WAIT, 1);
    SetFlag(SF_DELUSION_UI_ANIMSWITCH_WAIT, 0);
  }
}

void DelusionTrigger::UpdateHiding(float dt) {
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

void DelusionTrigger::UpdateShowing(float dt) {
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
        SetFlag(SF_DELUSION_UI_ANIM_WAIT, 0);
      }
    } break;
    default:
      break;
  }
}

void DelusionTrigger::PlayClickSound() {
  ScrWork[SW_SEREQNO + 0] = 19;
  Impacto::Audio::Channels[Audio::AC_SE0 + 0]->SetVolume(
      (ScrWork[SW_SEVOL + 0] / 100.0f) * 0.3f);
  Impacto::Audio::Channels[Audio::AC_SE0 + 0]->Play("se", 19, false, 0.0f);
}

void DelusionTrigger::UpdateShown(float dt) {
  MaskOffsetX = (ShakeState == 1)   ? 0
                : (ShakeState == 2) ? -16
                : (ShakeState == 3) ? 0
                : (ShakeState == 4) ? 18
                : (ShakeState == 5) ? 0
                : (ShakeState == 6) ? -20
                                    : 0;
  ShakeState = (ShakeState == 0) ? 0 : ShakeState - 1;
  bool anim = false;
  if ((PADinputButtonWentDown & PAD1L2) &&
      TextSystem.DelusionTextFade.IsStopped()) {
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
  } else if ((PADinputButtonWentDown & PAD1R2) &&
             TextSystem.DelusionTextFade.IsStopped()) {
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
      TriggerOnTint = RgbIntToFloat(0xffb0ce);
      if (SpinRate < 40) {
        SpinRate = SpinRate + 2;
        anim = true;
      }
      if (UnderlayerXRate < 2400) {
        UnderlayerXRate += 100;
        anim = true;
        if (UnderlayerXRate == 2400 && ScrWork[SW_DELUSION_POS_TXT_IDX] != 0) {
          TextSystem.DelusionSelectedLine =
              (ScrWork[SW_DELUSION_POS_TXT_IDX] - 1) * 3;
          TextSystem.Init();
        }
      }
    } else if (ScrWork[SW_DELUSION_STATE] == DS_Negative) {
      TriggerOnTint = ScrWork[SW_DELUSION_NEG_TXT_IDX]
                          ? RgbIntToFloat(0xffb0ce)
                          : RgbIntToFloat(0x2242e3);
      if (SpinRate > -40) {
        SpinRate = SpinRate - 2;
        anim = true;
      }
      if (UnderlayerXRate > -2400) {
        UnderlayerXRate -= 100;
        anim = true;
        if (UnderlayerXRate == -2400 && ScrWork[SW_DELUSION_NEG_TXT_IDX] != 0) {
          TextSystem.DelusionSelectedLine =
              (ScrWork[SW_DELUSION_NEG_TXT_IDX] - 1) * 3;
          TextSystem.Init();
        }
      }
    }
  } else {
    if ((TriggerOnTintAlpha != 0)) {
      TriggerOnTintAlpha -= 4;
    }
    if (TextSystem.DelusionTextFade.IsIn()) {
      TextSystem.DelusionTextFade.StartOut();
    }
    if (SpinRate < -5) {
      SpinRate = SpinRate + 2;
      anim = true;
    } else if (SpinRate > 5) {
      SpinRate = SpinRate - 2;
      anim = true;
    }
    if (UnderlayerXRate < -400) {
      UnderlayerXRate += 100;
      anim = true;
    } else if (UnderlayerXRate > 400) {
      UnderlayerXRate -= 100;
      anim = true;
    }
  }

  TextSystem.Update(dt);
  SetFlag(SF_DELUSION_UI_ANIM_WAIT, anim);
  SetFlag(SF_DELUSION_UI_ANIMSWITCH_WAIT, anim);
}

void DelusionTrigger::Update(float dt) {
  if (!GetFlag(SF_DELUSIONACTIVE) && State != Hidden) {
    Reset();
  }
  if (State == Showing) {
    UpdateShowing(dt);
  } else if (State == Hiding) {
    UpdateHiding(dt);
  } else if (State == Shown) {
    UpdateShown(dt);
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

  Sprite mask = ScreenMask;
  mask.Bounds.X = UnderlayerXOffset / 1000.0f;

  const RectF spriteDest = {0.0f, 0.0f, Profile::DesignWidth,
                            Profile::DesignHeight};
  const CornersQuad maskDest =
      ScaledMask.Bounds.RotateAroundCenter(ScrWorkAngleToRad(SpinAngle));

  Renderer->DrawSprite(
      mask, spriteDest,
      glm::vec4(1.0f, 1.0f, 1.0f, (BackgroundAlpha * 160) / 65536.0f));

  TextSystem.Render();
  Renderer->DrawMaskedSpriteOverlay(BackgroundSprite, ScaledMask, spriteDest,
                                    maskDest, (BackgroundAlpha * 160) >> 8, 20,
                                    glm::mat4(1.0f), glm::vec4(1.0f), true);
}

void DelusionTrigger::Load() {
  State = Shown;
  ShakeState = 0;
  MaskOffsetX = 0;
  MaskScaleFactor = 82944;

  UnderlayerXOffset = 20000;
  AnimCounter = 0;
  SpinAngle = 0;
  SpinRate = 5;
  BackgroundAlpha = 256;
  TextSystem.Clear();
  switch (DelusionState) {
    case DS_Neutral:
      TriggerOnTintAlpha = 0;
      TriggerOnTint = glm::vec4(0.0f);
      UnderlayerXRate = 400;
      break;
    case DS_Positive:
      TriggerOnTintAlpha = 104;
      TriggerOnTint = RgbIntToFloat(0xffb0ce);
      UnderlayerXRate = 2400;
      if (ScrWork[SW_DELUSION_POS_TXT_IDX] != 0) {
        TextSystem.DelusionSelectedLine =
            (ScrWork[SW_DELUSION_POS_TXT_IDX] - 1) * 3;
        TextSystem.Init();
      }
      break;
    case DS_Negative:
      TriggerOnTintAlpha = 104;
      UnderlayerXRate = -2400;
      TriggerOnTint = RgbIntToFloat(0x2242e3);
      if (ScrWork[SW_DELUSION_NEG_TXT_IDX] != 0) {
        TriggerOnTint = RgbIntToFloat(0xffb0ce);
        TextSystem.DelusionSelectedLine =
            (ScrWork[SW_DELUSION_NEG_TXT_IDX] - 1) * 3;
        TextSystem.Init();
      }
      break;
  }
  auto bufId = Vm::Interface::GetBufferId(ScrWork[SW_EFF_CAP_BUF2]);
  ScrWork[SW_CAP1FADECT + (bufId - 1) * Profile::Vm::ScrWorkBgStructSize] = 256;
}

void DelusionTrigger::Reset() {
  State = Hidden;
  SpinAngle = 0;
  SpinRate = 0;
  UnderlayerAlpha = 0;
  UnderlayerXOffset = 0;
  UnderlayerXRate = 0;
  TriggerOnTint = glm::vec4{};
  TriggerOnTintAlpha = 0;
  AnimCounter = 0;
  AnimationState = 0;
  ShakeState = 0;
  MaskScaleFactor = 65536;
  TextSystem.Clear();
}

DelusionTextSystem::DelusionTextSystem(DelusionTrigger const& ctx) : Ctx(&ctx) {
  DelusionTextFade.SetDuration(DelusionTextFadeDuration);
}

void DelusionTextSystem::Init() {
  InitLineOffsets();
  InitLines();
  DelusionTextFade.StartIn();
}

void DelusionTextSystem::InitLineOffsets() {
  // Line offsets [0,1,2,0,1,2,0,1,2,...]
  for (size_t i = 0; i < LineOffsets.size(); ++i)
    LineOffsets[i] = i / (LineOffsets.size() / GlyphLines.size());
  TextLineXOffset = 0;
  TextIndex = 0;
  std::shuffle(LineOffsets.begin(), LineOffsets.end(),
               std::mt19937{std::random_device{}()});
}

void DelusionTextSystem::InitLines() {
  using namespace Profile::CHLCC::DelusionTrigger;

  for (auto& line : GlyphLines) line.fill(nullptr);
  for (size_t lineIdx = 0; lineIdx < GlyphLines.size(); ++lineIdx) {
    const int pastScreenStartIndex = static_cast<int>(
        std::ceil(Profile::DesignWidth / DelusionScaledGlyphWidth));

    size_t charOffset = pastScreenStartIndex;
    if (lineIdx != 1) charOffset = (std::rand() % 8) + pastScreenStartIndex * 2;

    FillLine(charOffset, GlyphLines[lineIdx]);
  }
}

void DelusionTextSystem::ScrollLine(size_t lineIdx) {
  auto& line = GlyphLines[lineIdx];

  std::shift_left(line.begin(), line.end(), 1);
  line[MaxCharsPerLine - 1] = nullptr;

  // Find the last non-empty glyph position
  int lastGlyph = -1;
  for (int i = 0; i < MaxCharsPerLine; ++i) {
    if (line[i] != nullptr) lastGlyph = i;
  }

  // Start filling after the last glyph
  size_t counter = lastGlyph + 1;

  // Fill more glyphs until line full
  FillLine(counter, line);
}

void DelusionTextSystem::FillLine(size_t& outStartIdx,
                                  std::span<const Sprite*> line) {
  while (true) {
    size_t offsetIndex = LineOffsets[TextIndex] + *DelusionSelectedLine;
    size_t charCnt = DelusionTextGlyphs[offsetIndex].size();
    if (outStartIdx + charCnt >= MaxCharsPerLine) break;

    TextIndex = (TextIndex + 1) % LineOffsets.size();
    outStartIdx++;

    const auto& glyphSrc = DelusionTextGlyphs[offsetIndex];
    for (size_t c = 0; c < charCnt && outStartIdx + c < MaxCharsPerLine; ++c)
      line[outStartIdx + c] = &glyphSrc[c];

    outStartIdx += charCnt;
  }
}

void DelusionTextSystem::Update(float dt) {
  DelusionTextFade.Update(dt);
  if (DelusionTextFade.IsOut()) {
    if (DelusionSelectedLine) DelusionSelectedLine.reset();
    return;
  };

  TextLineXOffset -= DelusionTextXVelocity;

  const float wrapAmount = DelusionScaledGlyphWidth - DelusionTextXVelocity;
  if (TextLineXOffset < -wrapAmount) {
    TextLineXOffset += wrapAmount;
    for (size_t i = 0; i < GlyphLines.size(); ++i) ScrollLine(i);
  }
}

void DelusionTextSystem::Render() {
  if (DelusionTextFade.IsOut()) return;
  for (size_t i = 0; i < GlyphLines.size(); i++) {
    for (size_t j = 0; j < GlyphLines[i].size(); j++) {
      const Sprite* glyph = GlyphLines[i][j];
      if (!glyph) continue;
      const glm::vec2 translation{
          TextLineXOffset + DelusionScaledGlyphWidth * j,
          -DelusionTextLineSpacing +
              (DelusionScaledGlyphHeight + DelusionTextLineSpacing) * i};
      CornersQuad quad{{
          .TL = {-DelusionScaledGlyphWidth / 2, -DelusionScaledGlyphHeight / 2},
          .BL = {-DelusionScaledGlyphWidth / 2, DelusionScaledGlyphHeight / 2},
          .TR = {DelusionScaledGlyphWidth / 2, -DelusionScaledGlyphHeight / 2},
          .BR = {DelusionScaledGlyphWidth / 2, DelusionScaledGlyphHeight / 2},
      }};
      const float zRots[] = {
          (translation.x * 38 + translation.y * 28965) *
              std::numbers::pi_v<float> / 65535.0f,
          (translation.x * 65 + translation.y * 14708) *
              std::numbers::pi_v<float> / 65535.0f,
          (translation.x * 34 + translation.y * 5651) *
              std::numbers::pi_v<float> / 65535.0f,
          (translation.x * 52 + translation.y * 18518) *
              std::numbers::pi_v<float> / 65535.0f,
      };
      glm::vec2* const corners[] = {&quad.TopLeft, &quad.BottomLeft,
                                    &quad.TopRight, &quad.BottomRight};

      for (int cornerIndex = 0; cornerIndex < 4; ++cornerIndex) {
        glm::vec2 result =
            Rotate2D(zRots[cornerIndex]) * glm::vec2{20.0f, 20.0f};
        *corners[cornerIndex] += glm::vec2(result) + translation;
      }
      Renderer->DrawSprite(
          *glyph, quad,
          RgbIntToFloat(0xffa0e0, (DelusionTextFade.Progress * 3.0f / 10.0f)));
    }
  }
}

void DelusionTextSystem::Clear() {
  for (auto& line : GlyphLines) {
    line.fill(nullptr);
  }

  TextIndex = 0;
  TextLineXOffset = 0;
  DelusionTextFade.Stop();
  DelusionTextFade.Progress = 0.0f;
  DelusionSelectedLine.reset();
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto