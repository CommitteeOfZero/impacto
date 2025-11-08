#pragma once
#include "../../animation.h"
#include "../../spritesheet.h"

#include <array>
#include <enum.h>
#include "../../ui/widgets/clickarea.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
class YesNoTrigger {
 public:
  enum class BGType : int8_t { BG0, BG1, BG2, BG3 };
  enum class YesNoSelect : int8_t { NONE = -1, YES = 0, NO = 1 };
  enum class YesNoState : int8_t {
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
    glm::vec2 BlurMaskPos;
  };

  void Start(int bgType, int bgBufId, int chipsBufId);
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
  void Reset();
  int Load(uint8_t* data);
  int Save(uint8_t* data);

  Widgets::ClickArea YesClickArea;
  Widgets::ClickArea NoClickArea;
  Sprite ActiveBackground;
  Sprite ActiveBlur;
  Animation StarAnimation;
  std::span<YesNoPositions> DispPosArr;
  glm::vec2 BgSpritePos;
  float BgSpriteScale = 1.0f;
  float BgTransition = 0.0f;

  int CurArrIndex = 0;
  int TargetArrIndex = 0;

  bool Display = false;
  bool HasStarted = false;
  bool AllowInput = false;
  bool GoToNextQuestion = false;
  bool DispSel = false;

  YesNoState State = YesNoState::None;
  YesNoSelect Selection = YesNoSelect::NONE;
  BGType BgType;

  static YesNoTrigger& GetInstance() {
    static YesNoTrigger impl;
    return impl;
  }

 private:
  void UpdateYesNoPos(float startX, float startY, float startScale,
                      float targetX, float targetY, float targetScale,
                      float transition);
  void AreaClick(Widgets::ClickArea* clickArea);
  bool IsChoiceBlocked(YesNoSelect choice);
  void ChooseSelected();
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto