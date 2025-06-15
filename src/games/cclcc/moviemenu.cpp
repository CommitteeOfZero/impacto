#include "moviemenu.h"

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

void MovieMenu::Show() {
  if (State != Shown) {
    MainItems.Children[0]->Enabled = true;
    MainItems.Children[1]->Enabled = GetFlag(SF_CLR_TRUE_CCLCC);
    MainItems.Children[2]->Enabled = [&]() {
      for (int i = 1; i < 10; ++i) {
        if (GetFlag(SF_CLR_END1 + i)) return true;
      }
      return false;
    }();
    MainItems.Children[3]->Enabled = GetFlag(SF_CLR_END1);
    MainItems.Show();
  }
  LibrarySubmenu::Show();
}
}  // namespace Impacto::UI::CCLCC