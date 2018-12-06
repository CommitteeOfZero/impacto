#include "fonts.h"
#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, Font> Fonts;

void LoadFonts() {
  EnsurePushMemberOfType("Fonts", kObjectType);

  auto const& _fonts = TopVal();
  for (Value::ConstMemberIterator it = _fonts.MemberBegin();
       it != _fonts.MemberEnd(); it++) {
    std::string name(EnsureGetKeyString(it));

    EnsurePushMemberIteratorOfType(it, kObjectType);

    Font& font = Fonts[name];
    font.Sheet = EnsureGetMemberSpriteSheet("Sheet");
    font.Rows = EnsureGetMemberInt("Rows");
    font.Columns = EnsureGetMemberInt("Columns");

    float designColWidth = EnsureGetMemberFloat("DesignColWidth");
    float actualColWidth = font.ColWidth();

    {
      EnsurePushMemberOfType("Widths", kArrayType);

      auto const& _widths = TopVal();
      uint32_t widthCount = _widths.Size();
      font.Widths = (float*)malloc(widthCount * sizeof(float));
      for (uint32_t i = 0; i < widthCount; i++) {
        font.Widths[i] =
            EnsureGetArrayElementFloat(i) * actualColWidth / designColWidth;
      }

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto