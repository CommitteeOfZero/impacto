#pragma once

#include "../impacto.h"
#include <vector>
#include "../io/stream.h"

namespace Impacto {

enum TexFmt { TexFmt_RGB, TexFmt_RGBA, TexFmt_U8 };

struct Texture {
  int Width = 0;
  int Height = 0;
  TexFmt Format = TexFmt_U8;
  std::vector<uint8_t> Buffer;

  void Init(TexFmt format, int width, int height);

  bool Load(Io::Stream* stream);
  void Load1x1(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0,
               uint8_t alpha = 0);
  void LoadSolidColor(int width, int height, uint32_t color = 0xFFFFFFFF);
  void LoadCheckerboard();
  void LoadPoliticalCompass();
  uint32_t Submit();

  using TextureLoader = auto (*)(Io::Stream* stream, Texture* texture) -> bool;
  static bool AddTextureLoader(TextureLoader c);

 private:
  static std::vector<TextureLoader>& GetRegistry();
};

}  // namespace Impacto