#pragma once

#include "librarysubmenus.h"

namespace Impacto::UI::CCLCC {

class MovieMenu : public LibrarySubmenu {
 public:
  MovieMenu();
  void Init() override;
};

}