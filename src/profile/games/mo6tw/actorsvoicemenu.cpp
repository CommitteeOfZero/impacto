#include "actorsvoicemenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/actorsvoicemenu.h"
#include "../../../text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ActorsVoiceMenu {

Sprite BackgroundSprite;

Sprite UnlockedSprites[ActorsVoiceCount];
Sprite LockedSprites[ActorsVoiceCount];
Sprite UnlockedHighlightedSprites[ActorsVoiceCount];
Sprite LockedHighlightedSprites[ActorsVoiceCount];
glm::vec2 InitialItemPosition;
glm::vec2 ItemOffset;
int CharacterBackgroundBufferId;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  GetMemberSpriteArray(UnlockedSprites, ActorsVoiceCount, "UnlockedSprites");
  GetMemberSpriteArray(LockedSprites, ActorsVoiceCount, "LockedSprites");
  GetMemberSpriteArray(UnlockedHighlightedSprites, ActorsVoiceCount,
                       "UnlockedHighlightedSprites");
  GetMemberSpriteArray(LockedHighlightedSprites, ActorsVoiceCount,
                       "LockedHighlightedSprites");

  InitialItemPosition = EnsureGetMemberVec2("InitialItemPosition");
  ItemOffset = EnsureGetMemberVec2("ItemOffset");

  CharacterBackgroundBufferId =
      EnsureGetMemberInt("CharacterBackgroundBufferId");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::ActorsVoiceMenu());
}

}  // namespace ActorsVoiceMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
