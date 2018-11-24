#include "fonts.h"
#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, Font> Fonts;

void LoadFonts() {
  auto const& _fonts = EnsureGetMemberOfType(Json, "/", "Fonts", kObjectType);

  for (Value::ConstMemberIterator it = _fonts.MemberBegin();
       it != _fonts.MemberEnd(); ++it) {
    std::string name(EnsureGetString(it->name, "/Fonts/x (name)"));
    AssertIs(it->value, "/Fonts/x", kObjectType);

    ImpLog(LL_Debug, LC_Profile, "Loading font %s\n", name.c_str());

    Font& font = Fonts[name];

    char const* sheetName =
        EnsureGetMemberString(it->value, "/Fonts/x", "Sheet");

    auto const sheetRef = SpriteSheets.find(sheetName);
    if (sheetRef == SpriteSheets.end()) {
      ImpLog(LL_Fatal, LC_Profile, "No spritesheet %s (used by font %s)\n",
             sheetName, name.c_str());
      Window::Shutdown();
    }

    font.Sheet = sheetRef->second;
    font.Rows = EnsureGetMemberInt(it->value, "/Fonts/x", "Rows");
    font.Columns = EnsureGetMemberInt(it->value, "/Fonts/x", "Columns");

    float designColWidth =
        EnsureGetMemberFloat(it->value, "/Fonts/x", "DesignColWidth");
    float actualColWidth = font.ColWidth();

    auto const& _widths =
        EnsureGetMemberOfType(it->value, "/Fonts/x", "Widths", kArrayType);

    font.Widths = (float*)malloc(_widths.Size() * sizeof(float));

    if (designColWidth != actualColWidth) {
      int i = 0;
      for (auto it = _widths.Begin(); it != _widths.End(); it++) {
        font.Widths[i] = EnsureGetFloat(*it, "/Fonts/x/Widths/y") *
                         actualColWidth / designColWidth;
        i++;
      }
    } else {
      int i = 0;
      for (auto it = _widths.Begin(); it != _widths.End(); it++) {
        font.Widths[i] = EnsureGetFloat(*it, "/Fonts/x/Widths/y");
        i++;
      }
    }
  }
}

}  // namespace Profile
}  // namespace Impacto