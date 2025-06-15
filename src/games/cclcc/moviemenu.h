#pragma once

#include "librarysubmenus.h"

namespace Impacto::UI::CCLCC {

class MovieMenu : public LibrarySubmenu {
 public:
  MovieMenu();
  void Show() override;
};

}