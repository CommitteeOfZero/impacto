#include "font.h"

#include "io/physicalfilestream.h"
#include "log.h"
#include "renderer/renderer.h"
#include "texture/texture.h"

#include <ankerl/unordered_dense.h>

#include <hb-ft.h>
#include <hb.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>

namespace Impacto {

struct ExternalFont::Impl {
  struct FreeTypeLibraryDeleter {
    void operator()(FT_Library library) const {
      if (library != nullptr) FT_Done_FreeType(library);
    }
  };

  struct FreeTypeFaceDeleter {
    void operator()(FT_Face face) const {
      if (face != nullptr) FT_Done_Face(face);
    }
  };

  struct HarfBuzzFontDeleter {
    void operator()(hb_font_t* font) const {
      if (font != nullptr) hb_font_destroy(font);
    }
  };

  struct GlyphCacheKey {
    uint32_t GlyphIndex;
    uint32_t PixelSize;

    friend bool operator==(GlyphCacheKey const&,
                           GlyphCacheKey const&) = default;

    struct hash {
      size_t operator()(GlyphCacheKey const& k) const {
        std::size_t seed = 0;
        HashCombine(seed, k.GlyphIndex, k.PixelSize);
        return seed;
      }
    };
  };

  std::unique_ptr<std::remove_pointer_t<FT_Library>, FreeTypeLibraryDeleter>
      FreeTypeLibrary;
  std::unique_ptr<std::remove_pointer_t<FT_Face>, FreeTypeFaceDeleter> FontFace;
  std::unique_ptr<hb_font_t, HarfBuzzFontDeleter> HarfBuzzFace;
  std::vector<uint8_t> FontData;

  ankerl::unordered_dense::map<GlyphCacheKey, CachedGlyph, GlyphCacheKey::hash>
      GlyphCache;
};

ExternalFont::ExternalFont(std::string const& path,
                           std::string const& logContext)
    : Font(FontType::External, 1.0f, 1.0f, OpacityCurve::Linear,
           OpacityCurve::Linear),
      FontImpl(new Impl()) {
  Io::Stream* stream = nullptr;
  IoError err = Io::PhysicalFileStream::Create(path, &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not open {:s} {:s}\n",
           logContext, path);
    return;
  }

  FontImpl->FontData.resize(stream->Meta.Size);
  int64_t readSize =
      stream->Read(FontImpl->FontData.data(), FontImpl->FontData.size());
  delete stream;

  if (readSize != static_cast<int64_t>(FontImpl->FontData.size())) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not read {:s} {:s}\n",
           logContext, path);
    Reset();
    return;
  }

  FT_Library ftLibrary = nullptr;
  if (FT_Init_FreeType(&ftLibrary) != 0) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not initialize FreeType for {:s}\n", logContext);
    Reset();
    return;
  }
  FontImpl->FreeTypeLibrary.reset(ftLibrary);

  FT_Face face = nullptr;
  if (FT_New_Memory_Face(
          FontImpl->FreeTypeLibrary.get(), FontImpl->FontData.data(),
          static_cast<FT_Long>(FontImpl->FontData.size()), 0, &face) != 0) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not load {:s} {:s}\n",
           logContext, path);
    Reset();
    return;
  }
  FontImpl->FontFace.reset(face);

  FontImpl->HarfBuzzFace.reset(
      hb_ft_font_create_referenced(FontImpl->FontFace.get()));
  if (!FontImpl->HarfBuzzFace) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not initialize HarfBuzz for {:s}\n", logContext);
    Reset();
  }
}

ExternalFont::~ExternalFont() {
  Reset();
  delete FontImpl;
}

void ExternalFont::Reset() {
  for (auto const& [key, glyph] : FontImpl->GlyphCache) {
    if (glyph.Sheet.Texture != 0) Renderer->FreeTexture(glyph.Sheet.Texture);
  }
  FontImpl->GlyphCache.clear();

  FontImpl->HarfBuzzFace.reset();
  FontImpl->FontFace.reset();
  FontImpl->FreeTypeLibrary.reset();
  FontImpl->FontData.clear();
}

bool ExternalFont::IsLoaded() const {
  return FontImpl->HarfBuzzFace != nullptr && FontImpl->FontFace != nullptr;
}

size_t ExternalFont::GetGlyphCount() const {
  assert(IsLoaded());
  return static_cast<size_t>(FontImpl->FontFace->num_glyphs);
}

std::vector<ExternalFontShapedGlyph> ExternalFont::ShapeLine(
    std::string_view text, float fontSize, float& width) {
  width = 0.0f;
  if (text.empty()) return {};
  assert(IsLoaded());

  FT_Set_Pixel_Sizes(FontImpl->FontFace.get(), 0,
                     static_cast<FT_UInt>(std::round(fontSize)));
  hb_ft_font_changed(FontImpl->HarfBuzzFace.get());

  hb_buffer_t* buffer = hb_buffer_create();
  hb_buffer_add_utf8(buffer, text.data(), static_cast<int>(text.size()), 0,
                     static_cast<int>(text.size()));
  hb_buffer_guess_segment_properties(buffer);
  hb_shape(FontImpl->HarfBuzzFace.get(), buffer, nullptr, 0);

  unsigned glyphCount = 0;
  hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
  hb_glyph_position_t* glyphPos =
      hb_buffer_get_glyph_positions(buffer, &glyphCount);

  std::vector<ExternalFontShapedGlyph> glyphs;
  glyphs.reserve(glyphCount);
  for (unsigned i = 0; i < glyphCount; i++) {
    ExternalFontShapedGlyph glyph{
        .GlyphIndex = glyphInfo[i].codepoint,
        .Offset = {static_cast<float>(glyphPos[i].x_offset) / 64.0f,
                   -static_cast<float>(glyphPos[i].y_offset) / 64.0f},
        .Advance = {static_cast<float>(glyphPos[i].x_advance) / 64.0f,
                    -static_cast<float>(glyphPos[i].y_advance) / 64.0f},
    };
    width += glyph.Advance.x;
    glyphs.push_back(glyph);
  }

  hb_buffer_destroy(buffer);
  return glyphs;
}

ExternalFont::CachedGlyph const& ExternalFont::GetOrRenderGlyph(
    uint32_t glyphIndex, uint32_t pixelSize) {
  Impl::GlyphCacheKey key{glyphIndex, pixelSize};
  if (auto it = FontImpl->GlyphCache.find(key);
      it != FontImpl->GlyphCache.end()) {
    return it->second;
  }

  CachedGlyph cached;

  FT_Set_Pixel_Sizes(FontImpl->FontFace.get(), 0,
                     static_cast<FT_UInt>(pixelSize));

  if (FT_Load_Glyph(FontImpl->FontFace.get(), glyphIndex, FT_LOAD_DEFAULT) !=
      0) {
    ImpLog(LogLevel::Warning, LogChannel::Profile,
           "Could not load glyph {:d}\n", glyphIndex);
  } else if (FT_Render_Glyph(FontImpl->FontFace.get()->glyph,
                             FT_RENDER_MODE_NORMAL) != 0) {
    ImpLog(LogLevel::Warning, LogChannel::Profile,
           "Could not render glyph {:d}\n", glyphIndex);
  } else {
    FT_GlyphSlot slot = FontImpl->FontFace.get()->glyph;
    FT_Bitmap const& bitmap = slot->bitmap;
    if (bitmap.width != 0 && bitmap.rows != 0) {
      Texture texture;
      texture.Init(TexFmt_U8, static_cast<int>(bitmap.width),
                   static_cast<int>(bitmap.rows));
      for (uint32_t y = 0; y < bitmap.rows; ++y) {
        const uint8_t* srcRow =
            bitmap.pitch >= 0
                ? bitmap.buffer + y * bitmap.pitch
                : bitmap.buffer + (bitmap.rows - 1 - y) * -bitmap.pitch;
        std::span<uint8_t> dstRow =
            std::span(texture.Buffer).subspan(y * bitmap.width, bitmap.width);
        std::memcpy(dstRow.data(), srcRow, bitmap.width);
      }

      cached.Sheet = SpriteSheet{static_cast<float>(bitmap.width),
                                 static_cast<float>(bitmap.rows)};
      cached.Sheet.Texture = texture.Submit();
      cached.Bearing = {static_cast<float>(slot->bitmap_left),
                        -static_cast<float>(slot->bitmap_top)};
      cached.Size = {static_cast<float>(bitmap.width),
                     static_cast<float>(bitmap.rows)};
    }
  }

  return FontImpl->GlyphCache.emplace(key, cached).first->second;
}

void ExternalFont::DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                                     float opacity, float outlineOpacity,
                                     RendererOutlineMode outlineMode,
                                     const SpriteSheet* maskedSheet,
                                     glm::mat4 transformation) {
  assert(IsLoaded());
  assert(maskedSheet == nullptr);

  const std::vector<size_t> visibleGlyphIds = GetVisibleGlyphIds(text);
  if (visibleGlyphIds.empty()) return;

  const auto drawPass = [&](uint32_t DialogueColorPair::* colorMember,
                            glm::vec2 offset) {
    const auto [passOpacity, opacityCurve] =
        colorMember == &DialogueColorPair::OutlineColor
            ? std::pair{outlineOpacity, OutlineOpacityCurve}
            : std::pair{opacity, ForegroundOpacityCurve};

    for (size_t idx : visibleGlyphIds) {
      ProcessedTextGlyph const& glyph = text[idx];

      CachedGlyph const& cached = GetOrRenderGlyph(
          glyph.CharId,
          static_cast<uint32_t>(std::round(glyph.DestRect.Height)));
      if (cached.Sheet.Texture == 0) continue;

      const glm::vec2 pos = glm::vec2(glyph.DestRect.X, glyph.DestRect.Y) +
                            cached.Bearing + offset;
      const Sprite sprite(cached.Sheet, 0, 0, cached.Size.x, cached.Size.y);
      const CornersQuad dest = sprite.ScaledBounds().Translate(pos);

      glm::vec4 color = RgbIntToFloat(glyph.Colors.*colorMember);
      color.a = ApplyOpacityCurve(glyph.Opacity * passOpacity, opacityCurve);

      Renderer->DrawSubtitleGlyph(sprite, dest, transformation, color);
    }
  };

  switch (outlineMode) {
    using enum RendererOutlineMode;
    case Full:
      drawPass(&DialogueColorPair::OutlineColor, {-1.0f, -1.0f});
      drawPass(&DialogueColorPair::OutlineColor, {1.0f, 1.0f});
      break;
    case BottomRight:
      drawPass(&DialogueColorPair::OutlineColor, {1.0f, 1.0f});
      break;
    case None:
      break;
  }

  drawPass(&DialogueColorPair::TextColor, {0.0f, 0.0f});
}

}  // namespace Impacto
