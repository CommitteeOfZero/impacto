#include "textureref.h"

#include "renderer.h"

namespace Impacto::Bgfx {

PlainTextureRef& PlainTextureRef::operator=(PlainTextureRef&& other) {
  if (&other == this) return *this;

  TextureObject = other.TextureObject;
  other.TextureObject = nullptr;

  return *this;
}

PlainTextureRef::~PlainTextureRef() {
  if (TextureObject == nullptr) return;
  Bgfx::Renderer* const renderer =
      static_cast<Bgfx::Renderer*>(Impacto::Renderer.get());

  const auto textureIt = std::ranges::find(renderer->Textures, TextureObject,
                                           &std::unique_ptr<Texture>::get);
  assert(textureIt != renderer->Textures.end() && "Double free");
  renderer->Textures.erase(textureIt);  // Also cleans up the texture

  TextureObject = nullptr;
}

MutableTextureRef& MutableTextureRef::operator=(MutableTextureRef&& other) {
  if (&other == this) return *this;

  TextureObject = other.TextureObject;
  other.TextureObject = nullptr;

  return *this;
}

MutableTextureRef::~MutableTextureRef() {
  if (TextureObject == nullptr) return;
  Bgfx::Renderer* const renderer =
      static_cast<Bgfx::Renderer*>(Impacto::Renderer.get());

  const auto textureIt =
      std::ranges::find(renderer->MutableTextures, TextureObject,
                        &std::unique_ptr<Bgfx::MutableTexture>::get);
  assert(textureIt != renderer->MutableTextures.end() && "Double free");
  renderer->MutableTextures.erase(textureIt);  // Also cleans up the texture

  TextureObject = nullptr;
}

}  // namespace Impacto::Bgfx
