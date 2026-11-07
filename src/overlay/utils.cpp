#include "utils.h"

#include "../profile/gamedefinitions.h"

namespace Impacto::Overlay {

std::string GetGameDisplayName(std::string const& gameKey) {
  auto const& name = Profile::GameDefinitions.at(gameKey).Name;
  return name.empty() ? gameKey : name;
}

}  // namespace Impacto::Overlay
