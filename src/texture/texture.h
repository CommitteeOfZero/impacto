#pragma once

#include "../impacto.h"
#include <vector>
#include "../io/inputstream.h"

namespace Impacto {

enum TexFmt { TexFmt_RGB, TexFmt_RGBA, TexFmt_U8, TexFmt_DC_A, TexFmt_DC };

#ifdef PLATFORM_DREAMCAST
int const TextureRegistrySize = 10;
#endif

struct Texture {
  int Width;
  int Height;
  TexFmt Format;
  uint8_t* Buffer;
  int BufferSize;

  void Init(TexFmt fmt, int width, int height);

  bool Load(Io::InputStream* stream);
  void Load1x1(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0,
               uint8_t alpha = 0);
  void LoadSolidColor(int width, int height, uint32_t color = 0xFFFFFFFF);
  void LoadCheckerboard();
  void LoadPoliticalCompass();
  uint32_t Submit();

  typedef bool (*TextureLoader)(Io::InputStream* stream, Texture* texture);
  static bool AddTextureLoader(TextureLoader c);

 private:
#ifdef PLATFORM_DREAMCAST
  static TextureLoader Registry[TextureRegistrySize];
#else
  static std::vector<TextureLoader> Registry;
#endif
};

}  // namespace Impacto