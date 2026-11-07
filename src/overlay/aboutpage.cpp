#include "aboutpage.h"

#include <imgui.h>

#include <version.h>
#include "utils.h"
#include "../mem.h"
#include "../profile/game.h"
#include "../profile/scriptvars.h"

namespace Impacto::Overlay {

using namespace Impacto::Profile::ScriptVars;

static void ShowPlayTime(char const* label, int totalSeconds) {
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;
  ImGui::Text("%s: %02d:%02d:%02d", label, hours, minutes, seconds);
}

void ShowAboutPage(std::string const& selectedGame) {
  ImGui::Text("Committee of Zero");
  ImGui::SameLine();
  ImGui::TextLinkOpenURL("Technical Support", "https://discord.gg/hRtvaYawg6");
  ImGui::Text("Impacto Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
              VERSION_PATCH);
  ImGui::Text("OS: %s", SDL_GetPlatform());
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);
  if (Profile::Game::HasInit) {
    ImGui::Separator();
    ImGui::Text("%s", GetGameDisplayName(selectedGame).c_str());
    ShowPlayTime("Current Session Play Time", ScrWork[SW_PLAYTIME]);
    ShowPlayTime("Total Play Time", ScrWork[SW_TOTALPLAYTIME]);
  }
}

}  // namespace Impacto::Overlay
