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
    const std::string name(EnsureGetKey<std::string>());

    const FontType fontType = EnsureGetMember<FontType>("Type");

    const std::optional<float> bitmapEmWidth =
        TryGetMember<float>("BitmapEmWidth");
    const std::optional<float> bitmapEmHeight =
        TryGetMember<float>("BitmapEmHeight");

    const OpacityCurve foregroundOpacityCurve =
        TryGetMember<OpacityCurve>("ForegroundOpacityCurve")
            .value_or(OpacityCurve::Linear);
    const OpacityCurve outlineOpacityCurve =
        TryGetMember<OpacityCurve>("OutlineOpacityCurve")
            .value_or(OpacityCurve::Linear);

    switch (fontType) {
      using enum FontType;
      case SingleSheet: {
        const SpriteSheet sheet = EnsureGetMember<SpriteSheet>("Sheet");
        const glm::ivec2 gridSize = EnsureGetMember<glm::ivec2>("GridSize");

        Fonts[name] = new SingleSheetFont(bitmapEmWidth, bitmapEmHeight,
                                          foregroundOpacityCurve,
                                          outlineOpacityCurve, sheet, gridSize);
      } break;

      case SeparateOutlineSheet:
      case LanguageBarrier: {
        const SpriteSheet foregroundSheet =
            EnsureGetMember<SpriteSheet>("ForegroundSheet");
        const SpriteSheet outlineSheet =
            EnsureGetMember<SpriteSheet>("OutlineSheet");

        const glm::ivec2 foregroundGridSize =
            EnsureGetMember<glm::ivec2>("ForegroundGridSize");
        const glm::ivec2 outlineGridSize =
            TryGetMember<glm::ivec2>("OutlineGridSize")
                .value_or(foregroundGridSize);

        if (fontType == SeparateOutlineSheet) {
          Fonts[name] = new SeparateOutlineSheetFont(
              bitmapEmWidth, bitmapEmHeight, foregroundOpacityCurve,
              outlineOpacityCurve, foregroundSheet, foregroundGridSize,
              outlineSheet, outlineGridSize);
        } else {
          assert(fontType == LanguageBarrier);

          const glm::vec2 foregroundOffset =
              TryGetMember<glm::vec2>("ForegroundOffset")
                  .value_or(glm::vec2(0.0f));
          const glm::vec2 outlineOffset =
              EnsureGetMember<glm::vec2>("OutlineOffset");

          Fonts[name] = new LanguageBarrierFont(
              bitmapEmWidth, bitmapEmHeight, foregroundOpacityCurve,
              outlineOpacityCurve, foregroundSheet, foregroundGridSize,
              outlineSheet, outlineGridSize, foregroundOffset, outlineOffset);
        }
      } break;
    }

    Font* const baseFont = Fonts[name];
    {
      EnsurePushMember("AdvanceWidths");

      const bool gotLuaArray =
          TryGet<std::vector<float>>(baseFont->AdvanceWidths);

      if (gotLuaArray) {
        Pop();
      } else {
        auto widthTablePath = EnsureGet<Io::AssetPath>();
        Pop();

        const float advanceWidthsEmWidth =
            EnsureGetMember<float>("AdvanceWidthsEmWidth");
        const float extraLetterSpacing =
            TryGetMember<float>("ExtraLetterSpacing").value_or(0.0f);

        uint8_t* widthBin;
        int64_t widthSz;
        if (widthTablePath.Slurp((void*&)widthBin, widthSz) != IoError_OK) {
          ImpLog(LogLevel::Fatal, LogChannel::Profile,
                 "Failed to load width table file for font {:s}\n", name);
          Window->Shutdown();
        }

        assert(static_cast<size_t>(widthSz) == baseFont->GetGlyphCount());
        baseFont->AdvanceWidths.reserve(widthSz);

        for (uint16_t i = 0; i < widthSz; i++) {
          baseFont->AdvanceWidths.emplace_back(
              static_cast<float>(widthBin[i] + extraLetterSpacing) *
              baseFont->BitmapEmWidth / advanceWidthsEmWidth);
        }
      }
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto