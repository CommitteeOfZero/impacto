#ifdef __linux__
#include "linuxdesktop.h"
#include "log.h"
#include <SDL.h>
#include <cstdlib>

namespace Impacto {
namespace LinuxDesktop {

void SetWMClassHints(const std::string& profileName) {
  const std::string& wmClass = profileName;

  setenv("SDL_VIDEO_X11_WMCLASS", wmClass.c_str(), 1);
  setenv("SDL_VIDEO_WAYLAND_WMCLASS", wmClass.c_str(), 1);
  setenv("SDL_APP_ID", wmClass.c_str(), 1);

  SDL_SetHintWithPriority("SDL_APP_ID", wmClass.c_str(), SDL_HINT_OVERRIDE);
  SDL_SetHintWithPriority("SDL_VIDEO_X11_WMCLASS", wmClass.c_str(),
                          SDL_HINT_OVERRIDE);
  SDL_SetHintWithPriority("SDL_VIDEO_WAYLAND_WMCLASS", wmClass.c_str(),
                          SDL_HINT_OVERRIDE);

  ImpLog(LogLevel::Info, LogChannel::General, "Set WM_CLASS: {:s}\n", wmClass);
}

}  // namespace LinuxDesktop
}  // namespace Impacto
#endif
