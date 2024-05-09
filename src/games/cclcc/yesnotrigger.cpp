#include "yesnotrigger.h"
#include "../../profile/games/cclcc/yesnotrigger.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace CCLCC {
namespace YesNoTrigger {
using namespace Profile::CCLCC::YesNoTrigger;
using namespace Vm::Interface;

bool disp = false;
bool hasStarted = false;
bool allowInput = false;
bool goToNextQuestion = false;
bool dispChips = false;

int chipsAngle;
float bgTransition;
enum class YesNoSelect { NONE = -1, YES = 0, NO = 1 } selection;

float bgSpriteX;
float bgSpriteY;
float bgScale;
int arrIndex = 0;
int targetArrIndex = 0;
BGType bgType;
YesNoState state;
YesNoPositions *dispPosArr;

Sprite ActiveBackground;
bool hasInit = false;

void Init() { hasInit = Profile::CCLCC::YesNoTrigger::Configure(); }

void updateYesNoPos(float startX, float startY, float startScale, float targetX,
                    float targetY, float targetScale, float transition) {
  float boundedScale = (targetScale < 1.0)   ? 1.0f
                       : (targetScale > 3.0) ? 3.0f
                                             : targetScale;

  float smoothTransition = cos(transition * static_cast<float>(M_PI));
  smoothTransition = (1.0f - smoothTransition) * 0.5f;

  bgSpriteX = startX + smoothTransition * (targetX - startX);
  bgSpriteY = startY + smoothTransition * (targetY - startY);
  bgScale = startScale + smoothTransition * (boundedScale - startScale);
}

void Update(float dt) {
  if (!hasInit || ScrWork[2147] != 0 || ScrWork[2142] != 0 ||
      ScrWork[6433] == 0 || !hasStarted) {
    return;
  }

  chipsAngle += 400;
  if (chipsAngle < 0 || chipsAngle > 65536) {
    chipsAngle = 65536;
  }
  switch (state) {
    case YesNoState::Init:
      state = YesNoState::ZoomStart;
      break;
    case YesNoState::ZoomStart:
      bgTransition += 0.01f;
      updateYesNoPos(0.0f, 0.0f, 1.0f, dispPosArr[0].bgXPos,
                     dispPosArr[0].bgYPos, 3.0f, bgTransition);
      if (bgTransition >= 1.0f) {
        bgTransition = 1.0f;
        state = YesNoState::MainInput;
      }
      break;
    case YesNoState::MainInput:
      if (allowInput) {
        dispChips = true;
        if (PADinputButtonWentDown &
            (PAD1UP | PAD1DOWN | PAD1LEFT | PAD1RIGHT)) {
          if (selection == YesNoSelect::NONE) {
            selection = YesNoSelect::YES;
          } else if (selection == YesNoSelect::YES) {
            selection = YesNoSelect::NO;
          } else {
            selection = YesNoSelect::YES;
          }
          if (bgType == BGType::BG0 && (arrIndex == 11 || arrIndex == 12)) {
            selection = YesNoSelect::YES;
            Audio::Channels[Audio::AC_SSE]->Play("sysse", 4, false, 0.0f);
          } else {
            Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);
          }
        }
        // TODO: Mouse stuff
        if (PADinputButtonWentDown & PADcustom[5] &&
            selection != YesNoSelect::NONE) {
          state = YesNoState::PanToNext;
          arrIndex = targetArrIndex;
          if (selection == YesNoSelect::YES) {
            targetArrIndex = dispPosArr[arrIndex].nextYesIndex;
          } else {
            targetArrIndex = dispPosArr[arrIndex].nextNoIndex;
          }
          bgTransition = 0.0f;
          dispChips = false;
          allowInput = false;
          selection = YesNoSelect::NONE;
          ScrWork[6432] = to_underlying(selection);
          Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0.0f);
        }
      }
      break;
    case YesNoState::PanToNext:
      if (goToNextQuestion) {
        bgTransition += 0.03f;
        updateYesNoPos(dispPosArr[arrIndex].bgXPos, dispPosArr[arrIndex].bgYPos,
                       3.0f, dispPosArr[targetArrIndex].bgXPos,
                       dispPosArr[targetArrIndex].bgYPos, 3.0f, bgTransition);
        if (bgTransition >= 1.0f) {
          bgTransition = 1.0f;
          if (dispPosArr[targetArrIndex].nextYesIndex == 0 ||
              dispPosArr[targetArrIndex].nextNoIndex == 0) {
            state = YesNoState::Complete;
          } else {
            state = YesNoState::MainInput;
          }
          goToNextQuestion = false;
        }
      }
      break;
    default:
      break;
  }
  if (bgScale < 1.0f) {
    bgScale = 1.0f;
  } else if (bgScale > 3.0f) {
    bgScale = 3.0f;
  }
  ScrWork[6434] = targetArrIndex;
}

void Render() {
  if (!hasInit || ScrWork[6433] == 0 || disp == false) {
    return;
  }

  float bgWidth = 3840.0f / bgScale;
  float bgHeight = 2160.0f / bgScale;
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
      Rect(static_cast<int>(bgSpriteX + bgXOffset),
           static_cast<int>(bgSpriteY + bgYOffset), static_cast<int>(bgWidth),
           static_cast<int>(bgHeight));

  Renderer->DrawSprite(ActiveBackground, Rect(0, 0, 1920, 1080), bgtint, 0.0f);

  if (dispChips) {
    float chipYesX =
        (dispPosArr[targetArrIndex].chipYesXPos - ActiveBackground.Bounds.X) *
        0.5f * bgScale;
    float chipYesY =
        (dispPosArr[targetArrIndex].chipYesYPos - ActiveBackground.Bounds.Y) *
        0.5f * bgScale;

    float chipNoX =
        (dispPosArr[targetArrIndex].chipNoXPos - ActiveBackground.Bounds.X) *
        0.5f * bgScale;
    float chipNoY =
        (dispPosArr[targetArrIndex].chipNoYPos - ActiveBackground.Bounds.Y) *
        0.5f * bgScale;
    Sprite *ActiveYesChip;
    Sprite *ActiveNoChip;
    glm::vec4 chipTint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);

    if (bgType == BGType::BG0 || bgType == BGType::BG1) {
      ActiveYesChip =
          (selection == YesNoSelect::YES) ? &YN1YesChipLarge : &YN1YesChipSmall;
      ActiveNoChip =
          (selection == YesNoSelect::NO) ? &YN1NoChipLarge : &YN1NoChipSmall;

      if (selection == YesNoSelect::YES) {
        starPos = {chipYesX - 132, chipYesY - 166};
      } else if (selection == YesNoSelect::NO) {
        starPos = {chipNoX - 132, chipNoY - 166};
      }

    } else if (bgType == BGType::BG2 || bgType == BGType::BG3) {
      ActiveYesChip =
          (selection == YesNoSelect::YES) ? &YN2YesChipLarge : &YN2YesChipSmall;
      ActiveNoChip =
          (selection == YesNoSelect::NO) ? &YN2NoChipLarge : &YN2NoChipSmall;

      if (selection == YesNoSelect::YES) {
        starPos = {chipYesX - 92, chipYesY - 147};
      } else if (selection == YesNoSelect::NO) {
        starPos = {chipNoX - 92, chipNoY - 147};
      }
    }
    float yesChipWidthX = 0.5f * bgScale * ActiveYesChip->Bounds.Width;
    float yesChipWidthY = 0.5f * bgScale * ActiveYesChip->Bounds.Height;
    float noChipWidthX = 0.5f * bgScale * ActiveNoChip->Bounds.Width;
    float noChipWidthY = 0.5f * bgScale * ActiveNoChip->Bounds.Height;
    if (selection != YesNoSelect::NONE) {
      Renderer->DrawSprite(StarChip, starPos, chipTint);
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

void Start(int type, int bgBufId, int chipsBufId) {
  if (!hasInit) {
    return;
  }
  // bgBufId & chipsBufId are unused
  hasStarted = false;
  disp = false;
  state = YesNoState::Init;
  if (type == 0 || type == 1) {
    dispPosArr = YesNoData1;
  } else if (type == 2 || type == 3) {
    dispPosArr = YesNoData2;
  }

  bgType = static_cast<BGType>(type);
  if (bgType == BGType::BG0) {
    ActiveBackground = YesNoBackground0;
  } else if (bgType == BGType::BG1) {
    ActiveBackground = YesNoBackground1;
  } else if (bgType == BGType::BG2) {
    ActiveBackground = YesNoBackground2;
  } else if (bgType == BGType::BG3) {
    ActiveBackground = YesNoBackground3;
  }

  dispChips = false;
  allowInput = false;
  goToNextQuestion = false;
}

void Show() {
  if (!hasInit) {
    return;
  }
  bgTransition = 0.0f;
  targetArrIndex = 0;
  selection = YesNoSelect::NONE;
  disp = true;
  hasStarted = true;
}

void Reset() {
  if (!hasInit) {
    return;
  }
  bgTransition = 0.0f;
  float newX = dispPosArr[0].bgXPos;
  float newY = dispPosArr[0].bgYPos;
  disp = true;
  targetArrIndex = 0;
  selection = YesNoSelect::NONE;
  updateYesNoPos(0.0f, 0.0f, 1.0f, newX, newY, 3.0f, 0.0f);
}

void Hide() {
  if (!hasInit) {
    return;
  }
}

}  // namespace YesNoTrigger
}  // namespace CCLCC
}  // namespace Impacto
