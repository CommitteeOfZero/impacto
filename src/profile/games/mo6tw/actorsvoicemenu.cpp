#include "actorsvoicemenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/actorsvoicemenu.h"
#include "../../../text/text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ActorsVoiceMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  GetMemberArray<Sprite>(std::span(UnlockedSprites, ActorsVoiceCount),
                         "UnlockedSprites");
  GetMemberArray<Sprite>(std::span(LockedSprites, ActorsVoiceCount),
                         "LockedSprites");
  GetMemberArray<Sprite>(
      std::span(UnlockedHighlightedSprites, ActorsVoiceCount),
      "UnlockedHighlightedSprites");
  GetMemberArray<Sprite>(std::span(LockedHighlightedSprites, ActorsVoiceCount),
                         "LockedHighlightedSprites");

  InitialItemPosition = EnsureGetMember<glm::vec2>("InitialItemPosition");
  ItemOffset = EnsureGetMember<glm::vec2>("ItemOffset");

  CharacterBackgroundBufferId =
      EnsureGetMember<int>("CharacterBackgroundBufferId");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::Menus[drawType].push_back(new UI::MO6TW::ActorsVoiceMenu());
}

}  // namespace ActorsVoiceMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
