#include "textureref.h"

#include "renderer.h"

namespace Impacto::Bgfx {

PlainTextureRef& PlainTextureRef::operator=(PlainTextureRef&& other) {
  if (&other == this) return *this;

  TextureMapId = other.TextureMapId;
  TextureObject = other.TextureObject;
  Dimensions = other.Dimensions;

  other.TextureMapId = 0;
  other.TextureObject = nullptr;

  return *this;
}

MutableTextureRef& MutableTextureRef::operator=(MutableTextureRef&& other) {
  if (&other == this) return *this;

  TextureMapId = other.TextureMapId;
  TextureObject = other.TextureObject;
  Dimensions = other.Dimensions;

  other.TextureMapId = 0;
  other.TextureObject = nullptr;

  return *this;
}

const Bgfx::TextureRefInterface& TextureRefInterface::ToBgfxTextureRefInterface(
    const Impacto::TextureRefInterface& texture) {
  switch (texture.GetType()) {
    using enum Impacto::TextureRefType;
    case Plain:
      return static_cast<const Bgfx::PlainTextureRef&>(texture);
    case Mutable:
      return static_cast<const Bgfx::MutableTextureRef&>(texture);
  }

  Panic(LogChannel::General, "Unexpected texture ref type \"{:s}\"",
        magic_enum::enum_name(texture.GetType()));
}

}  // namespace Impacto::Bgfx
