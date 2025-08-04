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

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  GetMemberArray<Sprite>(UnlockedSprites, ActorsVoiceCount, "UnlockedSprites");
  GetMemberArray<Sprite>(LockedSprites, ActorsVoiceCount, "LockedSprites");
  GetMemberArray<Sprite>(UnlockedHighlightedSprites, ActorsVoiceCount,
                         "UnlockedHighlightedSprites");
  GetMemberArray<Sprite>(LockedHighlightedSprites, ActorsVoiceCount,
                         "LockedHighlightedSprites");

  InitialItemPosition = EnsureGetMember<glm::vec2>("InitialItemPosition");
  ItemOffset = EnsureGetMember<glm::vec2>("ItemOffset");

  CharacterBackgroundBufferId =
      EnsureGetMember<int>("CharacterBackgroundBufferId");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::ActorsVoiceMenu());
}

}  // namespace ActorsVoiceMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
