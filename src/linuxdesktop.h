#pragma once

#ifdef __linux__
#include <string>

namespace Impacto {
namespace LinuxDesktop {

void SetWMClassHints(const std::string& profileName);

}  // namespace LinuxDesktop
}  // namespace Impacto
#endif
