#pragma once

#include "librarysubmenus.h"

namespace Impacto::UI::CCLCC {

class MovieMenu : public LibrarySubmenu {
 public:
  MovieMenu();
  void Init() override;
  void UpdateInput(float dt) override;

 private:
  void UpdateFirstMovieDiskVisuals() const;
  bool IsExtraMovieModeOn = false;
};

}  // namespace Impacto::UI::CCLCC
