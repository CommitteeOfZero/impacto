#include "fonts.h"
#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, Font*> Fonts;

void LoadFonts() {
  EnsurePushMemberOfType("Fonts", kObjectType);

  auto const& _fonts = TopVal();
  for (Value::ConstMemberIterator it = _fonts.MemberBegin();
       it != _fonts.MemberEnd(); it++) {
    std::string name(EnsureGetKeyString(it));

    EnsurePushMemberIteratorOfType(it, kObjectType);

    FontType type =
        FontType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    Font* baseFont;

    switch (type) {
      case FontType::Basic: {
        BasicFont* font = new BasicFont();
        Fonts[name] = font;
        baseFont = font;

        font->Sheet = EnsureGetMemberSpriteSheet("Sheet");

        break;
      }
      case FontType::LB: {
        LBFont* font = new LBFont();
        Fonts[name] = font;
        baseFont = font;

        font->ForegroundSheet = EnsureGetMemberSpriteSheet("ForegroundSheet");
        font->OutlineSheet = EnsureGetMemberSpriteSheet("OutlineSheet");

        font->OutlineOffset = EnsureGetMemberVec2("OutlineOffset");

        break;
      }
    }

    baseFont->Rows = EnsureGetMemberInt("Rows");
    baseFont->Columns = EnsureGetMemberInt("Columns");

    baseFont->CalculateDefaultSizes();

    baseFont->LineSpacing = EnsureGetMemberFloat("LineSpacing");

    float designColWidth = EnsureGetMemberFloat("DesignColWidth");
    float actualColWidth = baseFont->CellWidth;

    baseFont->Widths =
        (float*)malloc(baseFont->Columns * baseFont->Rows * sizeof(float));

    float extraLetterSpacing;
    if (!TryGetMemberFloat("ExtraLetterSpacing", extraLetterSpacing)) {
      extraLetterSpacing = 0;
    }

    {
      EnsurePushMember("Widths");

      Io::AssetPath widthTablePath;
      if (TryGetAssetPath(widthTablePath)) {
        uint8_t* widthBin;
        int64_t widthSz;
        if (widthTablePath.Slurp((void**)&widthBin, &widthSz) != IoError_OK) {
          ImpLog(LL_Fatal, LC_Profile,
                 "Failed to load width table file for font %s\n", name.c_str());
          Window::Shutdown();
        }
        assert(widthSz == baseFont->Columns * baseFont->Rows);
        for (uint16_t i = 0; i < widthSz; i++) {
          baseFont->Widths[i] = ((float)widthBin[i] + extraLetterSpacing) *
                                actualColWidth / designColWidth;
        }
      } else {
        AssertIs(kArrayType);

        auto const& _widths = TopVal();
        uint32_t widthCount = _widths.Size();
        for (uint32_t i = 0; i < widthCount; i++) {
          baseFont->Widths[i] =
              (EnsureGetArrayElementFloat(i) + extraLetterSpacing) *
              actualColWidth / designColWidth;
        }
      }

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto