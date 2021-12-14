#include "albummenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/albummenu.h"
#include "../../../text.h"
#include "../../../game.h"
#include "../../../window.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace AlbumMenu {

Sprite BackgroundSprite;

Sprite CharacterButtonSprites[CharacterButtonCount];
Sprite HighlightedCharacterButtonSprites[CharacterButtonCount];
glm::vec2 InitialButtonPosition;
float ButtonOddX;
float ButtonEvenX;
glm::vec2 ButtonMargin;
float HighlightAnimationDuration;
int YunoButtonIdx;
int SuzuButtonIdx;
Sprite CharacterPortraits[CharacterPortraitCount];
glm::vec2 PortraitPosition;

float FadeInDuration;
float FadeOutDuration;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  GetMemberSpriteArray(CharacterButtonSprites, CharacterButtonCount,
                       "CharacterButtons");
  GetMemberSpriteArray(HighlightedCharacterButtonSprites, CharacterButtonCount,
                       "HighlightedCharacterButtons");
  InitialButtonPosition = EnsureGetMemberVec2("InitialButtonPosition");
  ButtonOddX = EnsureGetMemberFloat("ButtonOddX");
  ButtonEvenX = EnsureGetMemberFloat("ButtonEvenX");
  ButtonMargin = EnsureGetMemberVec2("ButtonMargin");
  HighlightAnimationDuration =
      EnsureGetMemberFloat("HighlightAnimationDuration");
  YunoButtonIdx = EnsureGetMemberInt("YunoButtonIdx");
  SuzuButtonIdx = EnsureGetMemberInt("SuzuButtonIdx");
  GetMemberSpriteArray(CharacterPortraits, CharacterPortraitCount,
                       "CharacterPortraits");
  PortraitPosition = EnsureGetMemberVec2("PortraitPosition");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
