#pragma once
#include "../../animation.h"
#include "../../spritesheet.h"

#include <array>
#include <enum.h>

namespace Impacto {
namespace CCLCC {
namespace YesNoTrigger {

struct YesNoPositions {
  float bgXPos;
  float bgYPos;
  int nextYesIndex;
  int nextNoIndex;
  float chipYesXPos;
  float chipYesYPos;
  float chipNoXPos;
  float chipNoYPos;
  float bubbleXPos;
  float bubbleYPos;
};

extern bool allowInput;
extern bool goToNextQuestion;

void Init();
void Start(int bgType, int bgBufId, int chipsBufId);
void Show();
void Hide();
void Update(float dt);
void Render();
void Reset();

enum class YesNoState { Init, ZoomStart, MainInput, PanToNext, Complete };
extern YesNoState state;

enum class BGType { BG0, BG1, BG2, BG3 };

void updateYesNoPos(float startX, float startY, float startScale, float targetX,
                    float targetY, float targetScale, float transition);

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
};
};  // namespace YesNoTrigger

}  // namespace CCLCC
}  // namespace Impacto