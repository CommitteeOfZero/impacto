#include "moviemenu.h"

#include <ranges>
#include "../../ui/ui.h"
#include "../../audio/audiosystem.h"
#include "../../profile/games/cclcc/librarymenu.h"

namespace Impacto::UI::CCLCC {
using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

MovieMenu::MovieMenu() : LibrarySubmenu() {
  for (size_t i = 0; i < MovieDiskSprites.size(); ++i) {
    const auto& diskSprite = MovieDiskSprites[i];
    const auto& diskHighlightSprite = MovieDiskHighlightSprites[i];
    auto movieOnclick = [](Widgets::Button* target) {
      ScrWork[SW_MOVIEMODE_CUR] = MovieDiskPlayIds[target->Id];
      LibraryMenuPtr->AllowsScriptInput = true;
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
      Audio::Channels[Audio::AC_BGM0]->Stop(0.0f);
    };
    auto disk = new Widgets::Button(static_cast<int>(i), diskSprite,
                                    diskHighlightSprite, Sprite(),
                                    MovieDiskDisplayPositions[i]);
    disk->OnClickHandler = movieOnclick;
    MainItems.Add(disk, FDIR_RIGHT);
  }

  auto setFocus = [](Widget* btn, Widget* btn2, FocusDirection dir) {
    FocusDirection oppositeDir = [dir] {
      switch (dir) {
        case FDIR_LEFT:
          return FDIR_RIGHT;
        case FDIR_RIGHT:
          return FDIR_LEFT;
        case FDIR_UP:
          return FDIR_DOWN;
        case FDIR_DOWN:
          return FDIR_UP;
      }
      return FDIR_LEFT;  // unreachable
    }();
    btn->SetFocus(btn2, dir);
    btn2->SetFocus(btn, oppositeDir);
  };

  setFocus(MainItems.Children[0], MainItems.Children[2], FDIR_RIGHT);
  setFocus(MainItems.Children[2], MainItems.Children[1], FDIR_RIGHT);
  setFocus(MainItems.Children[1], MainItems.Children[3], FDIR_RIGHT);
  setFocus(MainItems.Children[3], MainItems.Children[0], FDIR_RIGHT);
  setFocus(MainItems.Children[2], MainItems.Children[1], FDIR_UP);
  setFocus(MainItems.Children[2], MainItems.Children[1], FDIR_DOWN);
}

void MovieMenu::Init() {
  ScrWork[SW_MOVIEMODE_CUR] = 0xff;
  MainItems.Children[0]->Enabled = true;
  MainItems.Children[1]->Enabled = GetFlag(SF_CLR_TRUE_CCLCC);
  auto const endingFlags =
      std::views::iota(1, 10) |
      std::views::transform([](int i) { return GetFlag(SF_CLR_END1 + i); });
  MainItems.Children[2]->Enabled =
      std::ranges::any_of(endingFlags, [](bool flag) { return flag; });
  MainItems.Children[3]->Enabled = GetFlag(SF_CLR_END1);
}

}  // namespace Impacto::UI::CCLCC