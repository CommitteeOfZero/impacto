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
    std::string name(EnsureGetKey<std::string>());

    FontType type =
        FontType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    Font* baseFont;

    switch (type) {
      case FontType::Basic: {
        BasicFont* font = new BasicFont();
        Fonts[name] = font;
        baseFont = font;

        font->Sheet = EnsureGetMember<SpriteSheet>("Sheet");

        break;
      }
      case FontType::LB: {
        LBFont* font = new LBFont();
        Fonts[name] = font;
        baseFont = font;

        font->ForegroundSheet = EnsureGetMember<SpriteSheet>("ForegroundSheet");
        font->OutlineSheet = EnsureGetMember<SpriteSheet>("OutlineSheet");

        if (!TryGetMember<glm::vec2>("ForegroundOffset",
                                     font->ForegroundOffset)) {
          font->ForegroundOffset = glm::vec2(0.0f);
        }
        font->OutlineOffset = EnsureGetMember<glm::vec2>("OutlineOffset");

        break;
      }
    }

    baseFont->Rows = EnsureGetMember<int>("Rows");
    baseFont->Columns = EnsureGetMember<int>("Columns");

    baseFont->CalculateDefaultSizes();

    if (!TryGetMember<float>("BitmapEmWidth", baseFont->BitmapEmWidth)) {
      baseFont->BitmapEmWidth = baseFont->CellWidth;
    }

    if (!TryGetMember<float>("BitmapEmHeight", baseFont->BitmapEmHeight)) {
      baseFont->BitmapEmHeight = baseFont->CellHeight;
    }

    baseFont->LineSpacing = EnsureGetMember<float>("LineSpacing");

    float advanceWidthsEmWidth = EnsureGetMember<float>("AdvanceWidthsEmWidth");
    float bitmapEmWidth = baseFont->BitmapEmWidth;

    baseFont->AdvanceWidths =
        (float*)malloc(baseFont->Columns * baseFont->Rows * sizeof(float));

    float extraLetterSpacing;
    if (!TryGetMember<float>("ExtraLetterSpacing", extraLetterSpacing)) {
      extraLetterSpacing = 0;
    }

    {
      EnsurePushMember("AdvanceWidths");

      auto widthTablePath = TryGet<Io::AssetPath>();
      if (widthTablePath) {
        uint8_t* widthBin;
        int64_t widthSz;
        if (widthTablePath->Slurp((void*&)widthBin, widthSz) != IoError_OK) {
          ImpLog(LogLevel::Fatal, LogChannel::Profile,
                 "Failed to load width table file for font {:s}\n", name);
          Window->Shutdown();
        }
        assert(widthSz == baseFont->Columns * baseFont->Rows);
        for (uint16_t i = 0; i < widthSz; i++) {
          baseFont->AdvanceWidths[i] =
              ((float)widthBin[i] + extraLetterSpacing) * bitmapEmWidth /
              advanceWidthsEmWidth;
        }
      } else {
        AssertIs(LUA_TTABLE);

        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          int i = EnsureGetKey<int32_t>();
          baseFont->AdvanceWidths[i] =
              (EnsureGetArrayElement<float>() + extraLetterSpacing) *
              bitmapEmWidth / advanceWidthsEmWidth;
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