#include "fonts.h"
#include "profile_internal.h"
#include "../log.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace Profile {

void LoadFonts() {
  EnsurePushMemberOfType("Fonts", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKeyString());

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
          Window->Shutdown();
        }
        assert(widthSz == baseFont->Columns * baseFont->Rows);
        for (uint16_t i = 0; i < widthSz; i++) {
          baseFont->Widths[i] = ((float)widthBin[i] + extraLetterSpacing) *
                                actualColWidth / designColWidth;
        }
      } else {
        AssertIs(LUA_TTABLE);

        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          int i = EnsureGetKeyInt();
          baseFont->Widths[i] =
              (EnsureGetArrayElementFloat() + extraLetterSpacing) *
              actualColWidth / designColWidth;
          Pop();
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