#include "font.h"

#include "io/physicalfilestream.h"
#include "log.h"
#include "renderer/renderer.h"
#include "texture/texture.h"

#include <hb-ft.h>
#include <hb.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>
#include <cstring>
#include <memory>

namespace Impacto {

namespace {

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

}  // namespace

struct ExternalFont::Impl {
  std::unique_ptr<std::remove_pointer_t<FT_Library>, FreeTypeLibraryDeleter>
      FreeTypeLibrary;
  std::unique_ptr<std::remove_pointer_t<FT_Face>, FreeTypeFaceDeleter> FontFace;
  std::unique_ptr<hb_font_t, HarfBuzzFontDeleter> HarfBuzzFace;
  std::vector<uint8_t> FontData;
};

ExternalFont::ExternalFont() : FontImpl(new Impl()) {}

ExternalFont::~ExternalFont() {
  Reset();
  delete FontImpl;
}

bool ExternalFont::Load(std::string const& path,
                        std::string const& logContext) {
  Reset();

  Io::Stream* stream = nullptr;
  IoError err = Io::PhysicalFileStream::Create(path, &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not open {:s} {:s}\n",
           logContext, path);
    return false;
  }

  FontImpl->FontData.resize(stream->Meta.Size);
  int64_t readSize =
      stream->Read(FontImpl->FontData.data(), FontImpl->FontData.size());
  delete stream;

  if (readSize != static_cast<int64_t>(FontImpl->FontData.size())) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not read {:s} {:s}\n",
           logContext, path);
    Reset();
    return false;
  }

  FT_Library ftLibrary = nullptr;
  if (FT_Init_FreeType(&ftLibrary) != 0) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not initialize FreeType for {:s}\n", logContext);
    Reset();
    return false;
  }
  FontImpl->FreeTypeLibrary.reset(ftLibrary);

  FT_Face face = nullptr;
  if (FT_New_Memory_Face(
          FontImpl->FreeTypeLibrary.get(), FontImpl->FontData.data(),
          static_cast<FT_Long>(FontImpl->FontData.size()), 0, &face) != 0) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not load {:s} {:s}\n",
           logContext, path);
    Reset();
    return false;
  }
  FontImpl->FontFace.reset(face);

  FontImpl->HarfBuzzFace.reset(
      hb_ft_font_create_referenced(FontImpl->FontFace.get()));
  if (!FontImpl->HarfBuzzFace) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not initialize HarfBuzz for {:s}\n", logContext);
    Reset();
    return false;
  }

  return true;
}

void ExternalFont::Reset() {
  FontImpl->HarfBuzzFace.reset();
  FontImpl->FontFace.reset();
  FontImpl->FreeTypeLibrary.reset();
  FontImpl->FontData.clear();
}

bool ExternalFont::IsLoaded() const {
  return FontImpl->HarfBuzzFace != nullptr && FontImpl->FontFace != nullptr;
}

std::vector<ExternalFontShapedGlyph> ExternalFont::ShapeLine(
    std::string_view text, float fontSize, float& width) {
  width = 0.0f;
  if (text.empty() || !IsLoaded()) return {};

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

void ExternalFont::RenderShapedLine(
    std::span<const ExternalFontShapedGlyph> glyphs, float fontSize,
    glm::vec2 origin, glm::vec4 tint,
    std::vector<ExternalFontGlyph>& outGlyphs) {
  if (!IsLoaded()) return;

  FT_Set_Pixel_Sizes(FontImpl->FontFace.get(), 0,
                     static_cast<FT_UInt>(std::round(fontSize)));

  glm::vec2 pen = origin;
  for (ExternalFontShapedGlyph const& glyph : glyphs) {
    if (FT_Load_Glyph(FontImpl->FontFace.get(), glyph.GlyphIndex,
                      FT_LOAD_DEFAULT) != 0) {
      pen += glyph.Advance;
      continue;
    }
    if (FT_Render_Glyph(FontImpl->FontFace.get()->glyph,
                        FT_RENDER_MODE_NORMAL) != 0) {
      pen += glyph.Advance;
      continue;
    }

    FT_GlyphSlot slot = FontImpl->FontFace.get()->glyph;
    FT_Bitmap const& bitmap = slot->bitmap;
    if (bitmap.width == 0 || bitmap.rows == 0) {
      pen += glyph.Advance;
      continue;
    }

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

    SpriteSheet sheet{static_cast<float>(bitmap.width),
                      static_cast<float>(bitmap.rows)};
    sheet.Texture = texture.Submit();

    glm::vec2 glyphPos =
        pen + glyph.Offset + glm::vec2(slot->bitmap_left, -slot->bitmap_top);
    outGlyphs.push_back({Sprite{sheet, 0, 0, static_cast<float>(bitmap.width),
                                static_cast<float>(bitmap.rows)},
                         glyphPos, tint});

    pen += glyph.Advance;
  }
}

void ExternalFont::FreeGlyphTextures(std::vector<ExternalFontGlyph>& glyphs) {
  for (ExternalFontGlyph const& glyph : glyphs) {
    if (glyph.GlyphSprite.Sheet.Texture != 0) {
      Renderer->FreeTexture(glyph.GlyphSprite.Sheet.Texture);
    }
  }
  glyphs.clear();
}

}  // namespace Impacto
