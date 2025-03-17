#include "librarymenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/librarymenu.h"
#include "../../../text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace LibraryMenu {

void Configure() {
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  LibraryBackgroundSprite = EnsureGetMember<Sprite>("LibraryBackgroundSprite");
  LibraryBackgroundPosition =
      EnsureGetMember<glm::vec2>("LibraryBackgroundPosition");
  LibraryIndexSprite = EnsureGetMember<Sprite>("LibraryIndexSprite");
  LibraryIndexPosition = EnsureGetMember<glm::vec2>("LibraryIndexPosition");
  LibraryButtonGuideSprite =
      EnsureGetMember<Sprite>("LibraryButtonGuideSprite");
  LibraryButtonGuidePosition =
      EnsureGetMember<glm::vec2>("LibraryButtonGuidePosition");
  LibraryMaskSprite = EnsureGetMember<Sprite>("LibraryMaskSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  auto library = new UI::CCLCC::LibraryMenu();
  UI::Menus[drawType].push_back(library);
}

}  // namespace LibraryMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto