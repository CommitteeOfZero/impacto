#pragma once
#include "../../animation.h"
#include "../../spritesheet.h"

#include <array>
#include <enum.h>

namespace Impacto {
namespace CCLCC {
class YesNoTrigger {
 public:
  enum class BGType { BG0, BG1, BG2, BG3 };
  enum class YesNoSelect { NONE = -1, YES = 0, NO = 1 };
  enum class YesNoState {
    None,
    Init,
    ZoomStart,
    MainInput,
    PanToNext,
    Complete = 6
  };
  struct YesNoPositions {
    glm::vec2 BgPos;
    int NextYesIndex;
    int NextNoIndex;
    glm::vec2 ChipYesPos;
    glm::vec2 ChipNoPos;
    glm::vec2 BubblePos;  // unused
  };

  void static Init();
  void Start(int bgType, int bgBufId, int chipsBufId);
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
  void Reset();
  void Load(uint8_t* data, int& dataSize);
  void Save(uint8_t* data, int& dataSize);

  YesNoState State = YesNoState::None;
  bool Display = false;
  bool HasStarted = false;
  bool AllowInput = false;
  bool GoToNextQuestion = false;
  bool DispSel = false;

  int StarAngle = 0;
  float BgTransition = 0.0f;
  YesNoSelect Selection = YesNoSelect::NONE;
  Sprite ActiveBackground;
  YesNoPositions* DispPosArr = nullptr;
  BGType BgType;
  glm::vec2 BgSpritePos;
  float BgSpriteScale = 1.0f;
  int CurArrIndex = 0;
  int TargetArrIndex = 0;

  static YesNoTrigger* YesNoTriggerPtr;

 private:
  void UpdateYesNoPos(float startX, float startY, float startScale,
                      float targetX, float targetY, float targetScale,
                      float transition);
};

}  // namespace CCLCC
}  // namespace Impacto