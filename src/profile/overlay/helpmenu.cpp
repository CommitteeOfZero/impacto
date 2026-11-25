#include "helpmenu.h"

#include <cassert>
#include <unordered_set>

#include "../profile_internal.h"
#include "../../log.h"

namespace Impacto::Profile {

template <>
struct TryGetImpl<Overlay::HelpMenu::HelpAction> {
  static std::optional<Overlay::HelpMenu::HelpAction> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;
    auto action = TryGetMember<std::string>("Action");
    auto keyboardMouse = TryGetMember<std::string>("KeyboardMouse");
    auto controller = TryGetMember<std::string>("Controller");
    auto touch = TryGetMember<std::string>("Touch");

    if (!action || !keyboardMouse || !controller || !touch) return std::nullopt;

    return Overlay::HelpMenu::HelpAction{
        .Action = std::move(*action),
        .KeyboardMouse = std::move(*keyboardMouse),
        .Controller = std::move(*controller),
        .Touch = std::move(*touch),
    };
  }
};

template <>
struct TryGetImpl<Overlay::HelpMenu::HelpSection> {
  static std::optional<Overlay::HelpMenu::HelpSection> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    std::optional<Overlay::HelpMenu::HelpSection> result;
    int keyCount = 0;
    ForEachProfileTable<std::string>([&](std::string const& name) {
      ++keyCount;
      if (result) return;
      auto actions = TryGet<std::vector<Overlay::HelpMenu::HelpAction>>();
      if (!actions) return;
      result = Overlay::HelpMenu::HelpSection{.Name = name,
                                              .Actions = std::move(*actions)};
    });
    assert(keyCount == 1 &&
           "Each HelpMenu section table must have exactly one key");
    return result;
  }
};

namespace Overlay::HelpMenu {

void Configure() {
  if (TryPushMember("HelpMenu")) {
    AssertIs(LUA_TTABLE);

    TryGetMember<bool>("UseOverlayHelp", UseOverlayHelp);
    OverlayHelpKbCustomId = TryGetMember<uint8_t>("OverlayHelpKbCustomId");
    TryGetMember<std::vector<HelpSection>>("Sections", Sections);

    Pop();
  }
}

}  // namespace Overlay::HelpMenu

}  // namespace Impacto::Profile
