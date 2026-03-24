#include "subtitle.h"
#include "../io/filemeta.h"

#include "profile_internal.h"

namespace Impacto::Profile {
template <>
struct TryGetImpl<Subtitle::SubtitleTrackFile> {
  static std::optional<Subtitle::SubtitleTrackFile> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;
    auto typeOpt = TryGetMember<Subtitle::SubtitleType>("Type");
    auto idOpt = TryGetMember<int>("Id");
    auto pathOpt = TryGetMember<std::string>("Path");
    auto configOpt = TryGetMember<SubtitleConfigType>("Config");

    if (!typeOpt || !configOpt || !(pathOpt.has_value() ^ idOpt.has_value()))
      return std::nullopt;
    return Subtitle::SubtitleTrackFile{
        .Type = *typeOpt,
        .Config = *configOpt,
        .Id = std::move(idOpt),
        .Path = std::move(pathOpt),
    };
  }
};

}  // namespace Impacto::Profile

namespace Impacto::Profile::Subtitle {
void Configure() {
  if (TryPushMember("Subtitle")) {
    AssertIs(LUA_TTABLE);
    SubtitleMappings =
        TryGetMember<decltype(SubtitleMappings)>("SubtitleMappings")
            .value_or(SubtitleMappings);

    SubtitleFontsDir =
        TryGetMember<decltype(SubtitleFontsDir)>("SubtitleFontsDir")
            .value_or(SubtitleFontsDir);
    for (auto& dir : SubtitleFontsDir) {
      dir = GetSystemDependentPath(dir);
    }

    Pop();
  }
}
}  // namespace Impacto::Profile::Subtitle