#include "textureref.h"

#include "renderer.h"

namespace Impacto {

template <typename T>
  requires TextureRefInterfaceType<T>
TextureRef<T>::TextureRef(T* ptr) : Ptr(ptr) {
  if (Ptr != nullptr) {
    Renderer->AlterRefCount(Ptr, +1);
  }
}

template <typename T>
  requires TextureRefInterfaceType<T>
TextureRef<T>& TextureRef<T>::operator=(const TextureRef<T>& other) {
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

template <typename T>
  requires TextureRefInterfaceType<T>
TextureRef<T>& TextureRef<T>::operator=(TextureRef<T>&& other) {
  if (this == &other || Ptr == other.Ptr) return *this;

  if (Ptr != nullptr && Renderer != nullptr) {
    Renderer->AlterRefCount(Ptr, -1);
  }

  Ptr = other.Ptr;
  other.Ptr = nullptr;

  return *this;
}

template <typename T>
  requires TextureRefInterfaceType<T>
TextureRef<T>::~TextureRef() {
  if (Ptr == nullptr || Renderer == nullptr) return;
  Renderer->AlterRefCount(Ptr, -1);
}

template class TextureRef<PlainTextureRefInterface>;
template class TextureRef<MutableTextureRefInterface>;

}  // namespace Impacto
