#include "yesnotrigger.h"
#include "../../profile/games/cclcc/yesnotrigger.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"
#include "../../renderer/renderer.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
using namespace Profile::CCLCC::YesNoTrigger;
using namespace Vm::Interface;
using namespace Impacto::Profile::ScriptVars;

int YesNoTrigger::Load(uint8_t* data) {
  int dataSize = 0;
  dataSize += 4;
  memcpy(&BgType, data + dataSize, sizeof(BGType));
  dataSize += 12;
  Display = data[dataSize++];
  HasStarted = data[dataSize++];
  DispSel = data[dataSize++];
  AllowInput = data[dataSize++];
  GoToNextQuestion = data[dataSize++];
  dataSize += 3;
  memcpy(&Selection, data + dataSize, sizeof(YesNoSelect));
  dataSize += 8;
  float bgSpritePosX, bgSpritePosY;
  memcpy(&bgSpritePosX, data + dataSize, sizeof(float));
  dataSize += 4;
  memcpy(&bgSpritePosY, data + dataSize, sizeof(float));
  dataSize += 4;
  BgSpritePos = glm::vec2(bgSpritePosX, bgSpritePosY);
  memcpy(&BgSpriteScale, data + dataSize, sizeof(float));
  dataSize += 16;
  memcpy(&BgTransition, data + dataSize, sizeof(float));
  dataSize += 4;
  memcpy(&State, data + dataSize, sizeof(YesNoState));
  dataSize += 4;
  // PS4 has 64-bit pointers, vita has 32-bit pointers...
  dataSize += sizeof(void*);
  memcpy(&CurArrIndex, data + dataSize, sizeof(int));
  dataSize += 4;
  memcpy(&TargetArrIndex, data + dataSize, sizeof(int));
  dataSize += 4;
  dataSize += sizeof(void*);  // buffer pointer in their struct
  dataSize += 8;
  assert(dataSize == 0x60);
  if (State != YesNoState::None && State != YesNoState::Complete) {
    switch (BgType) {
      case BGType::BG0:
        ActiveBackground = YesNoBackground0;
        DispPosArr = YesNoData1;
        ActiveBlur = YNChipBlurBg0;
        break;
      case BGType::BG1:
        ActiveBackground = YesNoBackground1;
        DispPosArr = YesNoData1;
        ActiveBlur = YNChipBlurBg1;
        break;
      case BGType::BG2:
        ActiveBackground = YesNoBackground2;
        DispPosArr = YesNoData2;
        ActiveBlur = YNChipBlurBg2;
        break;
      case BGType::BG3:
        ActiveBackground = YesNoBackground3;
        DispPosArr = YesNoData2;
        ActiveBlur = YNChipBlurBg2;
        break;
    }
  }
  return dataSize;
}

int YesNoTrigger::Save(uint8_t* data) {
  int dataSize = 0;
  dataSize += 4;
  memcpy(data + dataSize, &BgType, sizeof(BGType));
  dataSize += 12;
  data[dataSize++] = Display;
  data[dataSize++] = HasStarted;
  data[dataSize++] = DispSel;
  data[dataSize++] = AllowInput;
  data[dataSize++] = GoToNextQuestion;
  dataSize += 3;
  memcpy(data + dataSize, &Selection, sizeof(YesNoSelect));
  dataSize += 8;
  memcpy(data + dataSize, &BgSpritePos.x, sizeof(float));
  dataSize += 4;
  memcpy(data + dataSize, &BgSpritePos.y, sizeof(float));
  dataSize += 4;
  memcpy(data + dataSize, &BgSpriteScale, sizeof(float));
  dataSize += 16;
  memcpy(data + dataSize, &BgTransition, sizeof(float));
  dataSize += 4;
  memcpy(data + dataSize, &State, sizeof(YesNoState));
  dataSize += 4;
  dataSize += sizeof(void*);
  memcpy(data + dataSize, &CurArrIndex, sizeof(int));
  dataSize += 4;
  memcpy(data + dataSize, &TargetArrIndex, sizeof(int));
  dataSize += 4;
  dataSize += sizeof(void*);  // buffer pointer in their struct
  dataSize += 8;

  assert(dataSize == 0x60);
  return dataSize;
}

void YesNoTrigger::UpdateYesNoPos(float startX, float startY, float startScale,
                                  float targetX, float targetY,
                                  float targetScale, float transition) {
  float boundedScale = std::clamp(targetScale, 1.0f, 3.0f);

  float smoothTransition = cos(transition * std::numbers::pi_v<float>);
  smoothTransition = (1.0f - smoothTransition) * 0.5f;

  BgSpritePos.x = startX + smoothTransition * (targetX - startX);
  BgSpritePos.y = startY + smoothTransition * (targetY - startY);
  BgSpriteScale = startScale + smoothTransition * (boundedScale - startScale);
}

void YesNoTrigger::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] != 0 || ScrWork[SW_SYSMENUCT] != 0 ||
      ScrWork[6433] == 0 || !HasStarted) {
    return;
  }

  YesClickArea.UpdateInput(dt);
  NoClickArea.UpdateInput(dt);
  YesClickArea.Enabled = AllowInput;
  NoClickArea.Enabled = AllowInput;
  StarAnimation.Update(dt);
  switch (State) {
    case YesNoState::Init:
      State = YesNoState::ZoomStart;
      break;
    case YesNoState::ZoomStart:
      BgTransition += 0.01f;
      UpdateYesNoPos(0.0f, 0.0f, 1.0f, DispPosArr[0].BgPos.x,
                     DispPosArr[0].BgPos.y, 3.0f, BgTransition);
      if (BgTransition >= 1.0f) {
        BgTransition = 1.0f;
        State = YesNoState::MainInput;
      }
      break;
    case YesNoState::MainInput:
      if (AllowInput) {
        DispSel = true;
        if (Input::CurrentInputDevice == Input::Device::Mouse) {
          if (NoClickArea.Hovered) {
            // folded that way on purpose,if two areas are both hovered
            // they will be switching constantly
            if (Selection != YesNoSelect::NO) {
              Selection = YesNoSelect::NO;
              Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0.0f);
            }
          } else if (YesClickArea.Hovered && Selection != YesNoSelect::YES &&
                     !IsChoiceBlocked(YesNoSelect::YES)) {
            Selection = YesNoSelect::YES;
            Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0.0f);
          }
        }

        if (PADinputButtonWentDown &
            (PADcustom[0] | PADcustom[1] | PADcustom[2] | PADcustom[3])) {
          if (Selection == YesNoSelect::NONE) {
            Selection = YesNoSelect::YES;
          } else if (Selection == YesNoSelect::YES) {
            Selection = YesNoSelect::NO;
          } else {
            Selection = YesNoSelect::YES;
          }
          if (IsChoiceBlocked(YesNoSelect::YES)) {
            Selection = YesNoSelect::NO;
            Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0.0f);
          } else {
            Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0.0f);
          }
        }
        if (PADinputButtonWentDown & PADcustom[5] &&
            Selection != YesNoSelect::NONE) {
          ChooseSelected();
        }
      }
      break;
    case YesNoState::PanToNext:
      if (GoToNextQuestion) {
        BgTransition += 0.03f;
        UpdateYesNoPos(DispPosArr[CurArrIndex].BgPos.x,
                       DispPosArr[CurArrIndex].BgPos.y, 3.0f,
                       DispPosArr[TargetArrIndex].BgPos.x,
                       DispPosArr[TargetArrIndex].BgPos.y, 3.0f, BgTransition);
        if (BgTransition >= 1.0f) {
          BgTransition = 1.0f;
          if (DispPosArr[TargetArrIndex].NextYesIndex == 0 ||
              DispPosArr[TargetArrIndex].NextNoIndex == 0) {
            State = YesNoState::Complete;
          } else {
            State = YesNoState::MainInput;
          }
          GoToNextQuestion = false;
        }
      }
      break;
    default:
      break;
  }
  BgSpriteScale = std::clamp(BgSpriteScale, 1.0f, 3.0f);
  ScrWork[6434] = TargetArrIndex;
}

void YesNoTrigger::Render() {
  if (ScrWork[6433] == 0 || Display == false) {
    return;
  }

  const glm::vec2 bgSize = ActiveBackground.Sheet.GetDimensions();
  const float bgWidth = bgSize.x / BgSpriteScale;
  const float bgHeight = bgSize.y / BgSpriteScale;
  const float bgXOffset = (bgSize.x - bgWidth) / 2.0f;
  const float bgYOffset =
      (bgSize.y - bgHeight) / 2.0f - 46.0f * (BgSpriteScale - 1.0f) * 0.5f;
  const float alpha =
      std::clamp(static_cast<float>(ScrWork[6433] << 3), 0.0f, 255.0f);
  const glm::vec4 bgtint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);
  ActiveBackground.Bounds =
      Rect(static_cast<int>(BgSpritePos.x + bgXOffset),
           static_cast<int>(BgSpritePos.y + bgYOffset),
           static_cast<int>(bgWidth), static_cast<int>(bgHeight));

  Renderer->DrawSprite(ActiveBackground,
                       RectF(0, 0, bgSize.x * 0.5f, bgSize.y * 0.5f), bgtint);

  const glm::vec2 blurScale = BgSpriteScale * 0.5f *
                              ActiveBackground.Sheet.GetDimensions() /
                              ActiveBlur.Bounds.GetSize();
  // center after zooming
  glm::vec2 blurTranslation =
      (1.0f - BgSpriteScale) * ActiveBlur.Bounds.GetSize();
  // apply scaled BgSpritePos offset
  blurTranslation -= BgSpritePos * BgSpriteScale * 0.5f;
  // correction for "- 46.0f" from ActiveBackground
  blurTranslation.y += 46.0f * (BgSpriteScale - 1.0f) * 0.75f;

  const glm::mat4 blurTransform =
      TransformationMatrix(glm::vec2(0.0f), blurScale, glm::vec3(0.0f),
                           glm::quat(), blurTranslation);

  // rescaled to [0.5;3]
  const float scale = 0.5f + 1.25f * (BgSpriteScale - 1.0f);
  const glm::vec3 maskScale = glm::vec3(scale);
  // calc as offset from the center
  glm::vec2 maskTranslate =
      DispPosArr[TargetArrIndex].BlurMaskPos - bgSize * 0.5f;
  // correction for "- 46.0f" from ActiveBackground with scaling "scale" to
  // [0;3] to zero out 46 at BgSpriteScale == 1
  maskTranslate.y += 46.0f * 0.5f * (scale - 0.5f) * 1.2f;
  // normalize for the sprite basis
  maskTranslate *= -1.0f * scale / bgSize;

  const glm::mat4 maskTransform = TransformationMatrix(
      glm::vec2(0.5f), maskScale, glm::vec3(0.0f), glm::quat(), maskTranslate);

  // Can't simply use ActiveBlur.Bounds due to some sprites having a
  // non-zero Y coordinate, so you don't want to additionally correct for it
  // via matrix
  Renderer->DrawMaskedSpriteOverlay(
      ActiveBlur, YesNoBlurMask,
      RectF(0.0f, 0.0f, ActiveBlur.Bounds.Width, ActiveBlur.Bounds.Height),
      YesNoBlurMask.Bounds, static_cast<int>(alpha), 255, blurTransform,
      maskTransform, glm::vec4(1.0f), true, true);

  if (DispSel) {
    glm::vec2 yesChipPos = (DispPosArr[TargetArrIndex].ChipYesPos -
                            ActiveBackground.Bounds.GetPos()) *
                           0.5f * BgSpriteScale;
    glm::vec2 noChipPos = (DispPosArr[TargetArrIndex].ChipNoPos -
                           ActiveBackground.Bounds.GetPos()) *
                          0.5f * BgSpriteScale;
    Sprite* activeYesChip;
    Sprite* activeNoChip;
    glm::vec2 selectedChipSmallSize;
    const glm::vec4 chipTint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);

    if (BgType == BGType::BG0 || BgType == BGType::BG1) {
      activeYesChip =
          (Selection == YesNoSelect::YES) ? &YN1YesChipLarge : &YN1YesChipSmall;
      activeNoChip =
          (Selection == YesNoSelect::NO) ? &YN1NoChipLarge : &YN1NoChipSmall;
      selectedChipSmallSize =
          ((Selection == YesNoSelect::YES) ? YN1YesChipSmall : YN1NoChipSmall)
              .Bounds.GetSize();
    } else /* if (BgType == BGType::BG2 || BgType == BGType::BG3) */ {
      activeYesChip =
          (Selection == YesNoSelect::YES) ? &YN2YesChipLarge : &YN2YesChipSmall;
      activeNoChip =
          (Selection == YesNoSelect::NO) ? &YN2NoChipLarge : &YN2NoChipSmall;
      selectedChipSmallSize =
          ((Selection == YesNoSelect::YES) ? YN2YesChipSmall : YN2NoChipSmall)
              .Bounds.GetSize();
    }
    const glm::vec2 yesChipSize =
        0.5f * BgSpriteScale * activeYesChip->Bounds.GetSize();
    const glm::vec2 noChipSize =
        0.5f * BgSpriteScale * activeNoChip->Bounds.GetSize();
    RectF yesChipDest =
        RectF(yesChipPos.x, yesChipPos.y, yesChipSize.x, yesChipSize.y);
    RectF noChipDest =
        RectF(noChipPos.x, noChipPos.y, noChipSize.x, noChipSize.y);

    if (Selection != YesNoSelect::NONE) {
      const glm::vec2 selectedChipPos =
          (Selection == YesNoSelect::YES) ? yesChipPos : noChipPos;
      const glm::vec2 activeSmallChipCenter =
          selectedChipPos + selectedChipSmallSize * 0.5f * 0.5f * BgSpriteScale;

      if (Selection == YesNoSelect::YES) {
        yesChipDest.SetPos(activeSmallChipCenter - yesChipSize * 0.5f);
      } else {
        noChipDest.SetPos(activeSmallChipCenter - noChipSize * 0.5f);
      }

      const glm::vec2 starTopLeft =
          activeSmallChipCenter - StarChip.Bounds.GetSize() * 0.5f;
      const CornersQuad starDest =
          StarChip.ScaledBounds()
              .RotateAroundCenter(StarAnimation.Progress)
              .Translate(starTopLeft);
      Renderer->DrawSprite(StarChip, starDest, chipTint);
    }

    YesClickArea.Bounds = yesChipDest;
    NoClickArea.Bounds = noChipDest;
    Renderer->DrawSprite(*activeYesChip, yesChipDest, chipTint);
    Renderer->DrawSprite(*activeNoChip, noChipDest, chipTint);
  }
  constexpr glm::vec4 maskTint = glm::vec4(1.0f, 1.0f, 1.0f, 160 / 256.0f);
  Renderer->DrawSprite(YesNoBgOverlay,
                       RectF(0, 0, bgSize.x * 0.5f, bgSize.y * 0.5f), maskTint);
}

void YesNoTrigger::Start(int type, int bgBufId, int chipsBufId) {
  // bgBufId & chipsBufId are unused
  HasStarted = false;
  Display = false;
  State = YesNoState::Init;
  if (type == 0 || type == 1) {
    DispPosArr = YesNoData1;
  } else if (type == 2 || type == 3) {
    DispPosArr = YesNoData2;
  }

  BgType = static_cast<BGType>(type);
  switch (BgType) {
    case BGType::BG0: {
      ActiveBackground = YesNoBackground0;
      ActiveBlur = YNChipBlurBg0;
      break;
    }
    case BGType::BG1: {
      ActiveBackground = YesNoBackground1;
      ActiveBlur = YNChipBlurBg1;
      break;
    }
    case BGType::BG2: {
      ActiveBackground = YesNoBackground2;
      ActiveBlur = YNChipBlurBg2;
      break;
    }
    case BGType::BG3: {
      ActiveBackground = YesNoBackground3;
      ActiveBlur = YNChipBlurBg2;
      break;
    }
  }

  DispSel = false;
  AllowInput = false;
  GoToNextQuestion = false;
  StarAnimation.Direction = AnimationDirection::In;
  StarAnimation.LoopMode = AnimationLoopMode::Loop;
  StarAnimation.SkipOnSkipMode = false;
  StarAnimation.DurationIn = 0.55f;
  auto onClick = [this](auto* area) { return AreaClick(area); };
  YesClickArea =
      Widgets::ClickArea(static_cast<int>(YesNoSelect::YES), RectF(), onClick);
  NoClickArea =
      Widgets::ClickArea(static_cast<int>(YesNoSelect::NO), RectF(), onClick);
}

void YesNoTrigger::Show() {
  BgTransition = 0.0f;
  TargetArrIndex = 0;
  Selection = YesNoSelect::NONE;
  Display = true;
  HasStarted = true;
  StarAnimation.StartIn();
}

void YesNoTrigger::Reset() {
  BgTransition = 0.0f;
  float newX = DispPosArr[0].BgPos.x;
  float newY = DispPosArr[0].BgPos.y;
  Display = true;
  TargetArrIndex = 0;
  Selection = YesNoSelect::NONE;
  UpdateYesNoPos(0.0f, 0.0f, 1.0f, newX, newY, 3.0f, 0.0f);
}

void YesNoTrigger::Hide() {
  Display = false;
  HasStarted = false;
  State = YesNoState::Complete;
  StarAnimation.Stop();
}

void YesNoTrigger::AreaClick(Widgets::ClickArea* area) {
  if (IsChoiceBlocked(static_cast<YesNoSelect>(area->Id))) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0.0f);
    return;
  }
  ChooseSelected();
  area->Hovered = false;
}

bool YesNoTrigger::IsChoiceBlocked(YesNoSelect choice) {
  return BgType == BGType::BG1 && choice == YesNoSelect::YES &&
         (TargetArrIndex == 11 || TargetArrIndex == 12);
}

void YesNoTrigger::ChooseSelected() {
  State = YesNoState::PanToNext;
  CurArrIndex = TargetArrIndex;
  if (Selection == YesNoSelect::YES) {
    TargetArrIndex = DispPosArr[CurArrIndex].NextYesIndex;
  } else {
    TargetArrIndex = DispPosArr[CurArrIndex].NextNoIndex;
  }
  BgTransition = 0.0f;
  DispSel = false;
  AllowInput = false;
  Selection = YesNoSelect::NONE;
  ScrWork[6432] = to_underlying(Selection);
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0.0f);
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto
