#pragma once

#include <glm/glm.hpp>

#include <span>

#include "../util.h"

namespace Impacto {

class TextureRefInterface {
 public:
  virtual ~TextureRefInterface() = default;

  [[nodiscard]] virtual bool IsValid() const = 0;

  [[nodiscard]] virtual glm::vec<2, size_t> GetDimensions() const = 0;

  [[nodiscard]] virtual uint64_t GetTextureId() const = 0;
};

class PlainTextureRefInterface : public TextureRefInterface {};

class MutableTextureRefInterface : public TextureRefInterface {
 public:
  virtual void Update(std::span<const uint8_t> data, size_t rowStride) = 0;
};

template <typename T>
concept TextureRefInterfaceType =
    is_any_of_v<T, PlainTextureRefInterface, MutableTextureRefInterface>;

template <typename Interface>
  requires TextureRefInterfaceType<Interface>
class TextureRef {
 public:
  TextureRef() = default;
  TextureRef(const TextureRef<Interface>& other) { *this = other; }
  TextureRef(TextureRef<Interface>&& other) { *this = std::move(other); }
  ~TextureRef();

  TextureRef(Interface* ptr);

  TextureRef<Interface>& operator=(const TextureRef<Interface>&);
  TextureRef<Interface>& operator=(TextureRef<Interface>&&);

  Interface& operator*() const { return *Ptr; }
  Interface* operator->() const { return Ptr; }
  Interface* Get() const { return Ptr; }

  bool operator==(const TextureRef<Interface>&) const = default;

  bool IsValid() const { return Ptr != nullptr && Ptr->IsValid(); }

 private:
  Interface* Ptr = nullptr;
};

using PlainTextureRef = TextureRef<PlainTextureRefInterface>;
using MutableTextureRef = TextureRef<MutableTextureRefInterface>;

}  // namespace Impacto
