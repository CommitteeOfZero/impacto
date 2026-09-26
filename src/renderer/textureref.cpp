#include "textureref.h"

#include "renderer.h"

namespace Impacto {

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

  Ptr = other.Ptr;
  other.Ptr = nullptr;

  return *this;
}

TextureRef::~TextureRef() {
  if (Ptr == nullptr || Renderer == nullptr) return;
  Renderer->AlterRefCount(Ptr, -1);
}

MutableTextureRef& MutableTextureRef::operator=(
    const MutableTextureRef& other) {
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

MutableTextureRef& MutableTextureRef::operator=(MutableTextureRef&& other) {
  if (this == &other || Ptr == other.Ptr) return *this;

  if (Ptr != nullptr && Renderer != nullptr) {
    Renderer->AlterRefCount(Ptr, -1);
  }

  Ptr = other.Ptr;
  other.Ptr = nullptr;

  return *this;
}

MutableTextureRef::~MutableTextureRef() {
  if (Ptr == nullptr || Renderer == nullptr) return;
  Renderer->AlterRefCount(Ptr, -1);
}

}  // namespace Impacto
