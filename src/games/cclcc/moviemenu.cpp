#include "moviemenu.h"

#include <ranges>
#include "../../ui/ui.h"
#include "../../profile/games/cclcc/librarymenu.h"

namespace Impacto::UI::CCLCC {
using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

MovieMenu::MovieMenu() : LibrarySubmenu() {
  for (int i = 0; i < MovieDiskSprites.size(); ++i) {
    const auto& diskSprite = MovieDiskSprites[i];
    const auto& diskHighlightSprite = MovieDiskHighlightSprites[i];
    auto movieOnclick = [](Widgets::Button* target) {
      ScrWork[SW_MOVIEMODE_CUR] = MovieDiskPlayIds[target->Id];
      LibraryMenuPtr->AllowsScriptInput = true;
    };
    auto disk =
        new Widgets::Button(i, diskSprite, Sprite(), diskHighlightSprite,
                            MovieDiskDisplayPositions[i]);
    disk->OnClickHandler = movieOnclick;
    MainItems.Add(disk, FDIR_RIGHT);
  }
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