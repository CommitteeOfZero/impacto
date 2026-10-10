#include "textureref.h"

#include "renderer.h"

namespace Impacto {

TextureRef::TextureRef(TextureInterface* const ptr) : Ptr(ptr) {
  if (Ptr != nullptr) {
    Renderer->AlterRefCount(Ptr, +1);
  }
}

TextureRef& TextureRef::operator=(const TextureRef& other) {
  if (this == &other || Ptr == other.Ptr) return *this;

  if (Ptr != nullptr && Renderer != nullptr) {
    Renderer->AlterRefCount(Ptr, -1);
  }

  Ptr = other.Ptr;

  if (Ptr != nullptr) {
    Renderer->AlterRefCount(Ptr, +1);
  }

  return *this;
}

TextureRef& TextureRef::operator=(TextureRef&& other) {
  if (this == &other || Ptr == other.Ptr) return *this;

  if (Ptr != nullptr && Renderer != nullptr) {
    Renderer->AlterRefCount(Ptr, -1);
  }

  Ptr = std::exchange(other.Ptr, nullptr);

  return *this;
}

TextureRef::~TextureRef() {
  if (Ptr == nullptr || Renderer == nullptr) return;
  Renderer->AlterRefCount(Ptr, -1);
}

}  // namespace Impacto
