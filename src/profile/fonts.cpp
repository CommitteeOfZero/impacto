#include "fonts.h"
#include "profile_internal.h"
#include "../log.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace Profile {

static std::vector<float> EnsureGetAdvanceWidths(const std::string_view name,
                                                 float bitmapEmWidth) {
  std::vector<float> advanceWidths;

  if (!TryGetMember("AdvanceWidthsTable", advanceWidths)) {
    EnsurePushMember("AdvanceWidthsBinary");

    auto widthTablePath = EnsureGetMember<Io::AssetPath>("Path");
    const size_t bytesPerGlyph = EnsureGetMember<size_t>("BytesPerGlyph");
    assert(bytesPerGlyph <= 4);

    const float emWidth = EnsureGetMember<float>("EmWidth");
    const float extraLetterSpacing =
        TryGetMember<float>("ExtraLetterSpacing").value_or(0.0f);

    uint8_t* widthBin;
    int64_t widthSz;
    if (widthTablePath.Slurp((void*&)widthBin, widthSz) != IoError_OK) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Failed to load width table file for font {:s}\n", name);
      Window->Shutdown();
    }
    assert(widthSz % bytesPerGlyph == 0);

    const size_t widthCount = widthSz / bytesPerGlyph;
    advanceWidths.reserve(widthCount);
    const auto readEntry = [&](size_t idx) -> uint64_t {
      uint64_t value = 0;
      for (size_t byte = 0; byte < bytesPerGlyph; byte++) {
        value = (value << 8) | widthBin[idx * bytesPerGlyph + byte];
      }
      return value;
    };

    for (size_t i = 0; i < widthCount; i++) {
      advanceWidths.emplace_back(
          static_cast<float>(readEntry(i) + extraLetterSpacing) *
          bitmapEmWidth / emWidth);
    }

    Pop();
  }

  return advanceWidths;
}

void LoadFonts() {
  EnsurePushMemberOfType("Fonts", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    const std::string name(EnsureGetKey<std::string>());

    const FontType fontType = EnsureGetMember<FontType>("Type");

    std::optional<float> bitmapEmWidth = TryGetMember<float>("BitmapEmWidth");
    std::optional<float> bitmapEmHeight = TryGetMember<float>("BitmapEmHeight");

    const OpacityCurve foregroundOpacityCurve =
        TryGetMember<OpacityCurve>("ForegroundOpacityCurve")
            .value_or(OpacityCurve::Linear);
    const OpacityCurve outlineOpacityCurve =
        TryGetMember<OpacityCurve>("OutlineOpacityCurve")
            .value_or(OpacityCurve::Linear);

    switch (fontType) {
      using enum FontType;
      case SingleSheet:
      case EdgeDetectedSingleSheet: {
        const SpriteSheet sheet = EnsureGetMember<SpriteSheet>("Sheet");
        const glm::ivec2 gridSize = EnsureGetMember<glm::ivec2>("GridSize");

        if (!bitmapEmWidth.has_value()) {
          bitmapEmWidth.emplace(sheet.DesignWidth / gridSize.x);
        }
        if (!bitmapEmHeight.has_value()) {
          bitmapEmHeight.emplace(sheet.DesignHeight / gridSize.y);
        }

        if (fontType == SingleSheet) {
          Fonts[name] = new SingleSheetFont(
              *bitmapEmWidth, *bitmapEmHeight, foregroundOpacityCurve,
              outlineOpacityCurve, sheet, gridSize,
              EnsureGetAdvanceWidths(name, *bitmapEmWidth));
        } else {
          assert(fontType == EdgeDetectedSingleSheet);

          const float differenceFactor =
              EnsureGetMember<float>("DifferenceFactor");
          const float intensityShift = EnsureGetMember<float>("IntensityShift");
          const float alphaShift = EnsureGetMember<float>("AlphaShift");

          Fonts[name] = new EdgeDetectedSingleSheetFont(
              *bitmapEmWidth, *bitmapEmHeight, foregroundOpacityCurve, sheet,
              gridSize, EnsureGetAdvanceWidths(name, *bitmapEmWidth),
              differenceFactor, intensityShift, alphaShift);
        }
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

        if (!bitmapEmWidth.has_value()) {
          bitmapEmWidth.emplace(foregroundSheet.DesignWidth /
                                foregroundGridSize.x);
        }
        if (!bitmapEmHeight.has_value()) {
          bitmapEmHeight.emplace(foregroundSheet.DesignHeight /
                                 foregroundGridSize.y);
        }

        if (fontType == SeparateOutlineSheet) {
          Fonts[name] = new SeparateOutlineSheetFont(
              *bitmapEmWidth, *bitmapEmHeight, foregroundOpacityCurve,
              outlineOpacityCurve, foregroundSheet, foregroundGridSize,
              outlineSheet, outlineGridSize,
              EnsureGetAdvanceWidths(name, *bitmapEmWidth));
        } else {
          assert(fontType == LanguageBarrier);

          const glm::vec2 foregroundOffset =
              TryGetMember<glm::vec2>("ForegroundOffset")
                  .value_or(glm::vec2(0.0f));
          const glm::vec2 outlineOffset =
              EnsureGetMember<glm::vec2>("OutlineOffset");

          Fonts[name] = new LanguageBarrierFont(
              *bitmapEmWidth, *bitmapEmHeight, foregroundOpacityCurve,
              outlineOpacityCurve, foregroundSheet, foregroundGridSize,
              outlineSheet, outlineGridSize,
              EnsureGetAdvanceWidths(name, *bitmapEmWidth), foregroundOffset,
              outlineOffset);
        }
      } break;

      case EdgeDetectedSingleVariableWidthSheet: {
        if (!bitmapEmWidth.has_value()) {
          throw std::runtime_error(
              fmt::format("BitmapEmWidth field is required for "
                          "EdgeDetectedSingleVariableWidthSheet font \"{:s}\"",
                          name));
        }
        if (!bitmapEmHeight.has_value()) {
          throw std::runtime_error(
              fmt::format("BitmapEmHeight field is required for "
                          "EdgeDetectedSingleVariableWidthSheet font \"{:s}\"",
                          name));
        }

        const float differenceFactor =
            EnsureGetMember<float>("DifferenceFactor");
        const float intensityShift = EnsureGetMember<float>("IntensityShift");
        const float alphaShift = EnsureGetMember<float>("AlphaShift");

        const SpriteSheet sheet = EnsureGetMember<SpriteSheet>("Sheet");
        Io::AssetPath binaryPath = EnsureGetMember<Io::AssetPath>("BinaryPath");

        Fonts[name] = new EdgeDetectedSingleVariableWidthSheetFont(
            *bitmapEmWidth, *bitmapEmHeight, foregroundOpacityCurve, sheet,
            binaryPath, differenceFactor, intensityShift, alphaShift);
      } break;

      case External:
        ImpLog(LogLevel::Fatal, LogChannel::Profile,
               "External fonts cannot be loaded from the profile for {:s}\n",
               name);
        Window->Shutdown();
        break;
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto