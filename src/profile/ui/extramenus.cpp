#include "optionsmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../games/mo6tw/clearlistmenu.h"
#include "../games/mo6tw/moviemenu.h"

namespace Impacto {
namespace Profile {
namespace ExtraMenus {

using namespace Impacto::UI;

ClearListMenuType ClearListType = ClearListMenuType::None;
AlbumMenuType AlbumType = AlbumMenuType::None;
MusicMenuType MusicType = MusicMenuType::None;
MovieMenuType MovieType = MovieMenuType::None;

void Configure() {
  if (TryPushMember("ExtraMenus")) {
    AssertIs(kObjectType);

    if (TryPushMember("ClearListMenu")) {
      ClearListType = ClearListMenuType::_from_integral_unchecked(
          EnsureGetMemberInt("Type"));

      if (ClearListType == +ClearListMenuType::MO6TW) {
        MO6TW::ClearListMenu::Configure();
      }

      Pop();
    }

    // if (TryPushMember("AlbumMenu")) {
    //  AlbumType =
    //      AlbumMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    //  if (AlbumType == +AlbumMenuType::MO6TW) {
    //    MO6TW::AlbumMenu::Configure();
    //  }

    //  Pop();
    //}

    // if (TryPushMember("MusicMenu")) {
    //  MusicType =
    //      MusicMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    //  if (MusicType == +MusicMenuType::MO6TW) {
    //    MO6TW::MusicMenu::Configure();
    //  }

    //  Pop();
    //}

    if (TryPushMember("MovieMenu")) {
      MovieType =
          MovieMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

      if (ClearListType == +MovieMenuType::MO6TW) {
        MO6TW::MovieMenu::Configure();
      }

      Pop();
    }

    Pop();
  }
}

}  // namespace ExtraMenus
}  // namespace Profile
}  // namespace Impacto