#include "gamedefinitions.h"

#include "profile_internal.h"
namespace Impacto::Profile {
std::string const* PatchDefinitions::Find(std::string_view name) const {
  auto itr = std::find_if(
      Entries.begin(), Entries.end(),
      [name](PatchEntry const& entry) { return entry.Name == name; });

  if (itr != Entries.end()) return &(itr->PatchPath);

  return nullptr;
}

template <>
struct TryGetImpl<PatchDefinitions> {
  static std::optional<PatchDefinitions> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;
    PatchDefinitions patches;

    ForEachProfileTable<std::string>([&](std::string const& name) {
      auto patchLuaPath = TryGet<std::string>().value_or("");
      if (patchLuaPath.empty()) {
        ImpLog(LogLevel::Warning, LogChannel::Profile,
               "Patch {} is missing patch path.", name);
        return;
      }
      if (!std::filesystem::exists(patchLuaPath)) {
        ImpLog(LogLevel::Info, LogChannel::Profile,
               "Patch {} is not present at \"{}\"", name, patchLuaPath);
        return;
      }
      patches.Entries.push_back(PatchDefinitions::PatchEntry{
          .Name = std::move(name),
          .PatchPath = std::move(patchLuaPath),
      });
    });

    return patches;
  }
};

template <>
struct TryGetImpl<GameDefinition> {
  static std::optional<GameDefinition> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    auto gameProfileOpt = TryGetMember<std::string>("GameProfile");
    auto name = TryGetMember<std::string>("Name").value_or("");
    auto patch =
        TryGetMember<decltype(GameDefinition::Patch)>("Patch").value_or(
            decltype(GameDefinition::Patch){});
    auto hidden = TryGetMember<bool>("Hidden").value_or(false);
    auto launcherOrderId = TryGetMember<int>("LauncherOrderId").value_or(99);
    auto launcherTheme =
        TryGetMember<uint32_t>("LauncherTheme").value_or(0xFFFFFF);

    if (!gameProfileOpt || gameProfileOpt->empty()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Missing GameProfile path\n");
      return std::nullopt;
    }
    return GameDefinition{
        .GameProfile = std::move(*gameProfileOpt),
        .Name = std::move(name),
        .Patch = std::move(patch),
        .Hidden = hidden,
        .LauncherOrderId = launcherOrderId,
        .LauncherTheme = launcherTheme,
    };
  }
};

void GameDefinition::Configure() {
  GameDefinitions =
      EnsureGetMember<decltype(GameDefinitions)>("GameDefinitions");
}

}  // namespace Impacto::Profile