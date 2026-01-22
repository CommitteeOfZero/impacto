#include "texture.h"

#include <string.h>

#include "../log.h"

#include "../renderer/renderer.h"

#include "plainloader.h"

namespace Impacto {

std::vector<Texture::TextureLoader>& Texture::GetRegistry() {
  static std::vector<TextureLoader> registry;
  return registry;
}

bool Texture::Load(Io::Stream* stream) {
  using namespace TexLoad;

  for (auto f : GetRegistry()) {
    if (f(stream, this)) return true;
  }

  // no registry for this one, since it has no real magic - we must try it last
  if (TextureIsPlain(stream)) return TextureLoadPlain(stream, this);

  uint32_t magic = Io::ReadBE<uint32_t>(stream);
  ImpLog(LogLevel::Error, LogChannel::TextureLoad,
         "No loader for texture, possible magic 0x{:08x}\n", magic);
  return false;
}

void Texture::Init(TexFmt format, int width, int height) {
  Width = width;
  Height = height;
  Format = format;

  const size_t bufferSize = [&]() {
    switch (format) {
      case TexFmt_RGBA:
        return width * height * 4;
      case TexFmt_RGB:
        return width * height * 3;
      case TexFmt_U8:
        return width * height;
      default:
        ImpLog(LogLevel::Warning, LogChannel::TextureLoad,
               "Unknown texture buffer format {:d}", static_cast<int>(format));
        return 0;
    }
  }();
  Buffer.resize(bufferSize);
}

void Texture::Load1x1(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
  Init(TexFmt_RGBA, 1, 1);
  Buffer[0] = red;
  Buffer[1] = green;
  Buffer[2] = blue;
  Buffer[3] = alpha;
}

void Texture::LoadSolidColor(int width, int height, uint32_t color) {
  Init(TexFmt_RGBA, width, height);

  const auto colorArray =
      std::bit_cast<std::array<const uint8_t, sizeof(uint32_t)>>(color);
  for (size_t i = 0; i + colorArray.size() <= Buffer.size();
       i += colorArray.size()) {
    std::ranges::copy(colorArray, Buffer.begin() + i);
  }
}

void Texture::LoadCheckerboard() {
  Init(TexFmt_U8, 128, 128);
  uint8_t color = 0xFF;
  for (uint8_t& byte : Buffer) {
    byte = color;
    color = ~color;
  }
}

void Texture::LoadPoliticalCompass() {
  Init(TexFmt_RGBA, 512, 512);

  for (int x = 0; x < Width / 2; x++) {
    for (int y = 0; y < Height / 2; y++) {
      Buffer[4 * x + 4 * y * Height + 0] = 0xE0;
      Buffer[4 * x + 4 * y * Height + 1] = 0x77;
      Buffer[4 * x + 4 * y * Height + 2] = 0x77;
      Buffer[4 * x + 4 * y * Height + 3] = 0xFF;

      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 0] = 0x38;
      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 1] = 0xBE;
      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 2] = 0xE0;
      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 3] = 0xFF;

      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 0] = 0x89;
      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 1] = 0xC7;
      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 2] = 0x72;
      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 3] = 0xFF;

      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 0] = 0xC6;
      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 1] = 0x8D;
      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 2] = 0xC3;
      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 3] = 0xFF;
    }
  }
}

uint32_t Texture::Submit() {
  ImpLog(LogLevel::Debug, LogChannel::Render, "Submitting texture\n");

  if (Buffer.empty()) return std::numeric_limits<uint32_t>::max();

  uint32_t result =
      Renderer->SubmitTexture(Format, Buffer.data(), Width, Height);

  // TODO I meant to do this elsewhere but we gotta do it somewhere
  Buffer.clear();

  return result;
}

bool Texture::AddTextureLoader(Texture::TextureLoader c) {
  GetRegistry().push_back(c);
  return true;
}

}  // namespace Impacto