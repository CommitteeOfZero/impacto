#include "yesnotrigger.h"
#include "../../profile/games/cclcc/yesnotrigger.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace CCLCC {
using namespace Profile::CCLCC::YesNoTrigger;
using namespace Vm::Interface;

YesNoTrigger *YesNoTrigger::YesNoTriggerPtr = nullptr;

void YesNoTrigger::Init() {
  if (Profile::CCLCC::YesNoTrigger::Configure()) {
    YesNoTriggerPtr = new YesNoTrigger();
  }
}

void YesNoTrigger::Load(uint8_t *data, int &dataSize) { 
  dataSize = 0; 
  
}

void YesNoTrigger::Save(uint8_t *data, int &dataSize) { dataSize = 0; }

void YesNoTrigger::UpdateYesNoPos(float startX, float startY, float startScale,
                                  float targetX, float targetY,
                                  float targetScale, float transition) {
  float boundedScale = (targetScale < 1.0)   ? 1.0f
                       : (targetScale > 3.0) ? 3.0f
                                             : targetScale;

  float smoothTransition = cos(transition * static_cast<float>(M_PI));
  smoothTransition = (1.0f - smoothTransition) * 0.5f;

  BgSpritePos.x = startX + smoothTransition * (targetX - startX);
  BgSpritePos.y = startY + smoothTransition * (targetY - startY);
  BgSpriteScale = startScale + smoothTransition * (boundedScale - startScale);
}

void YesNoTrigger::Update(float dt) {
  if (ScrWork[2147] != 0 || ScrWork[2142] != 0 || ScrWork[6433] == 0 ||
      !HasStarted) {
    return;
  }

  StarAngle += 400;
  if (StarAngle < 0 || StarAngle > 65536) {
    StarAngle = 0;
  }
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
        if (PADinputButtonWentDown &
            (PAD1UP | PAD1DOWN | PAD1LEFT | PAD1RIGHT)) {
          if (Selection == YesNoSelect::NONE) {
            Selection = YesNoSelect::YES;
          } else if (Selection == YesNoSelect::YES) {
            Selection = YesNoSelect::NO;
          } else {
            Selection = YesNoSelect::YES;
          }
          if (BgType == BGType::BG0 &&
              (CurArrIndex == 11 || CurArrIndex == 12)) {
            Selection = YesNoSelect::YES;
            Audio::Channels[Audio::AC_SSE]->Play("sysse", 4, false, 0.0f);
          } else {
            Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);
          }
        }
        // TODO: Mouse stuff
        if (PADinputButtonWentDown & PADcustom[5] &&
            Selection != YesNoSelect::NONE) {
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
          Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0.0f);
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
  if (BgSpriteScale < 1.0f) {
    BgSpriteScale = 1.0f;
  } else if (BgSpriteScale > 3.0f) {
    BgSpriteScale = 3.0f;
  }
  ScrWork[6434] = TargetArrIndex;
}

void YesNoTrigger::Render() {
  if (ScrWork[6433] == 0 || Display == false) {
    return;
  }

  float bgWidth = 3840.0f / BgSpriteScale;
  float bgHeight = 2160.0f / BgSpriteScale;
  float bgXOffset = (3840.0f - bgWidth) / 2.0f;
  float bgYOffset = (2160.0f - bgHeight) / 2.0f - 46.0f;
  float alpha = static_cast<float>(ScrWork[6433] << 3);
  if (alpha > 255) {
    alpha = 255;
  } else if (alpha < 0) {
    alpha = 0;
  }
  glm::vec4 bgtint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);
  glm::vec2 starPos;
  ActiveBackground.Bounds =
      Rect(static_cast<int>(BgSpritePos.x + bgXOffset),
           static_cast<int>(BgSpritePos.y + bgYOffset),
           static_cast<int>(bgWidth), static_cast<int>(bgHeight));

  Renderer->DrawSprite(ActiveBackground, Rect(0, 0, 1920, 1080), bgtint, 0.0f);

  if (DispSel) {
    float chipYesX =
        (DispPosArr[TargetArrIndex].ChipYesPos.x - ActiveBackground.Bounds.X) *
        0.5f * BgSpriteScale;
    float chipYesY =
        (DispPosArr[TargetArrIndex].ChipYesPos.y - ActiveBackground.Bounds.Y) *
        0.5f * BgSpriteScale;

    float chipNoX =
        (DispPosArr[TargetArrIndex].ChipNoPos.x - ActiveBackground.Bounds.X) *
        0.5f * BgSpriteScale;
    float chipNoY =
        (DispPosArr[TargetArrIndex].ChipNoPos.y - ActiveBackground.Bounds.Y) *
        0.5f * BgSpriteScale;
    Sprite *ActiveYesChip;
    Sprite *ActiveNoChip;
    glm::vec4 chipTint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);

    if (BgType == BGType::BG0 || BgType == BGType::BG1) {
      ActiveYesChip =
          (Selection == YesNoSelect::YES) ? &YN1YesChipLarge : &YN1YesChipSmall;
      ActiveNoChip =
          (Selection == YesNoSelect::NO) ? &YN1NoChipLarge : &YN1NoChipSmall;

      if (Selection == YesNoSelect::YES) {
        starPos = {chipYesX - 132, chipYesY - 166};
      } else if (Selection == YesNoSelect::NO) {
        starPos = {chipNoX - 132, chipNoY - 166};
      }

    } else if (BgType == BGType::BG2 || BgType == BGType::BG3) {
      ActiveYesChip =
          (Selection == YesNoSelect::YES) ? &YN2YesChipLarge : &YN2YesChipSmall;
      ActiveNoChip =
          (Selection == YesNoSelect::NO) ? &YN2NoChipLarge : &YN2NoChipSmall;

      if (Selection == YesNoSelect::YES) {
        starPos = {chipYesX - 92, chipYesY - 147};
      } else if (Selection == YesNoSelect::NO) {
        starPos = {chipNoX - 92, chipNoY - 147};
      }
    }
    float yesChipWidthX = 0.5f * BgSpriteScale * ActiveYesChip->Bounds.Width;
    float yesChipWidthY = 0.5f * BgSpriteScale * ActiveYesChip->Bounds.Height;
    float noChipWidthX = 0.5f * BgSpriteScale * ActiveNoChip->Bounds.Width;
    float noChipWidthY = 0.5f * BgSpriteScale * ActiveNoChip->Bounds.Height;
    if (Selection != YesNoSelect::NONE) {
      Renderer->DrawSprite(StarChip, starPos, chipTint, {1.0, 1.0},
                           StarAngle / 65536.0f);
    }
    Renderer->DrawSprite(*ActiveYesChip,
                         Rect(chipYesX, chipYesY, yesChipWidthX, yesChipWidthY),
                         chipTint);
    Renderer->DrawSprite(*ActiveNoChip,
                         Rect(chipNoX, chipNoY, noChipWidthX, noChipWidthY),
                         chipTint);
  }
  glm::vec4 maskTint = glm::vec4(1.0f, 1.0f, 1.0f, 160 / 256.0f);
  Renderer->DrawSprite(YesNoBgMask, Rect(0, 0, 1920, 1080), maskTint, 0.0f);
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
  if (BgType == BGType::BG0) {
    ActiveBackground = YesNoBackground0;
  } else if (BgType == BGType::BG1) {
    ActiveBackground = YesNoBackground1;
  } else if (BgType == BGType::BG2) {
    ActiveBackground = YesNoBackground2;
  } else if (BgType == BGType::BG3) {
    ActiveBackground = YesNoBackground3;
  }

  DispSel = false;
  AllowInput = false;
  GoToNextQuestion = false;
}

void YesNoTrigger::Show() {
  BgTransition = 0.0f;
  TargetArrIndex = 0;
  Selection = YesNoSelect::NONE;
  Display = true;
  HasStarted = true;
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
}

}  // namespace CCLCC
}  // namespace Impacto
