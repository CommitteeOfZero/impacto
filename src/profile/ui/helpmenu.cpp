#include "helpmenu.h"
#include "../profile_internal.h"
#include "../games/cclcc/helpmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {

template <>
struct TryGetImpl<HelpMenu::HelpAction> {
  static std::optional<HelpMenu::HelpAction> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;
    auto section = TryGetMember<std::string>("Section");
    auto action = TryGetMember<std::string>("Action");
    auto keyboardMouse = TryGetMember<std::string>("KeyboardMouse");
    auto controller = TryGetMember<std::string>("Controller");

    if (!section || !action || !keyboardMouse || !controller)
      return std::nullopt;

    return HelpMenu::HelpAction{
        .Section = std::move(*section),
        .Action = std::move(*action),
        .KeyboardMouse = std::move(*keyboardMouse),
        .Controller = std::move(*controller),
    };
  }
};

namespace HelpMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("HelpMenu")) {
    AssertIs(LUA_TTABLE);

    Type = EnsureGetMember<HelpMenuType>("Type");
    UseOverlayHelp = false;
    TryGetMember<bool>("UseOverlayHelp", UseOverlayHelp);
    OverlayHelpKbCustomId = -1;
    TryGetMember<int>("OverlayHelpKbCustomId", OverlayHelpKbCustomId);
    Actions.clear();
    TryGetMember<std::vector<HelpAction>>("Actions", Actions);

    if (Type == HelpMenuType::CCLCC) {
      CCLCC::HelpMenu::Configure();
    }

    Pop();
  }
}

}  // namespace HelpMenu
}  // namespace Profile
}  // namespace Impacto