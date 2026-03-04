#include "saveicondisplay.h"

#include "defaultsaveiconanimation.h"
#include "../games/chlcc/animations/saveicon.h"

#include "../profile/hud/saveicon.h"

namespace Impacto {
namespace SaveIconDisplay {
using namespace Impacto::Profile::SaveIcon;

static glm::vec2 Position;

static std::variant<UI::DefaultSaveIconAnimation, UI::CHLCC::SaveIconAnimation>
    IconAnimation;
static Animation Timer;

static bool IsTimed = false;

void Init() {
  Configure();
  switch (SaveIconCurrentType) {
    default:
    case (+SaveIconType::Default): {
      IconAnimation = UI::DefaultSaveIconAnimation(
          ForegroundAnimation, BackgroundSprite, BackgroundOffset,
          BackgroundMaxAlpha, FadeInDuration, FadeOutDuration);
      break;
    }
    case (+SaveIconType::CHLCC): {
      IconAnimation = UI::CHLCC::SaveIconAnimation(
          ActiveAnimationDuration, FadeInDuration, FadeOutDuration,
          std::span<Sprite, CHLCC_SAVE_ICON_SPRITES>(SaveIconSprites));
      break;
    }
  }
}

void Hide() {
  std::visit([](auto& anim) { anim.Hide(); }, IconAnimation);
  IsTimed = false;
}

void Show() { ShowAt(DefaultPosition); }
void ShowAt(glm::vec2 pos) {
  Position = pos;
  std::visit([](auto& anim) { anim.Show(); }, IconAnimation);
}
void ShowFor(float seconds) {
  Timer.LoopMode = AnimationLoopMode::Stop;
  Timer.SetDuration(seconds);
  IsTimed = true;
  Timer.StartIn(true);
  Show();
}

void Update(float dt) {
  Timer.Update(dt);
  if (IsTimed && Timer.IsIn()) Hide();
  std::visit([dt](auto& anim) { anim.Update(dt); }, IconAnimation);
}

void Render() {
  std::visit([](auto& anim) { anim.Render(Position); }, IconAnimation);
}

}  // namespace SaveIconDisplay
}  // namespace Impacto