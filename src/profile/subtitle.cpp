#include "subtitle.h"
#include "../io/filemeta.h"

#include "profile_internal.h"

namespace Impacto::Profile::Subtitle {
void Configure() {
  EnsurePushMemberOfType("Subtitle", LUA_TTABLE);

  if (TryPushMember("SubtitleMappings")) {
    AssertIs(LUA_TTABLE);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      std::string primaryMediaFile(EnsureGetKey<std::string>());
      SubtitleTrackFiles subtitleTrackFiles;

      ForEachProfileArray([&](uint32_t index) {
        SubtitleType type =
            SubtitleType::_from_integral(EnsureGetMember<uint8_t>("Type"));
        std::string path = EnsureGetMember<std::string>("Path");
        subtitleTrackFiles.emplace_back(std::move(type), std::move(path));
      });

      SubtitleMappings.try_emplace(std::move(primaryMediaFile),
                                   std::move(subtitleTrackFiles));
      Pop();
    }

    Pop();
  }

  SubtitleFontsDir = TryGetMember<std::string>("SubtitleFontsDir");
  if (SubtitleFontsDir) {
    SubtitleFontsDir = GetSystemDependentPath((*SubtitleFontsDir));
  }

  Pop();
}
}  // namespace Impacto::Profile::Subtitle