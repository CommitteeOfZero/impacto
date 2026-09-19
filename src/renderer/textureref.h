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

class MutableTextureRefInterface : public virtual TextureRefInterface {
 public:
  virtual void Update(std::span<const uint8_t> data, size_t rowStride) = 0;
};

class TextureRef {
 public:
  TextureRef() = default;
  TextureRef(const TextureRef& other) { *this = other; }
  TextureRef(TextureRef&& other) { *this = std::move(other); }
  virtual ~TextureRef();

  TextureRef(TextureRefInterface* ptr);

  TextureRef& operator=(const TextureRef&);
  TextureRef& operator=(TextureRef&&);

  TextureRefInterface& operator*() const { return *Ptr; }
  TextureRefInterface* operator->() const { return Ptr; }
  TextureRefInterface* Get() const { return Ptr; }

  bool operator==(const TextureRef&) const = default;

  bool IsValid() const { return Ptr != nullptr && Ptr->IsValid(); }

 protected:
  TextureRefInterface* Ptr = nullptr;
};

class MutableTextureRef final : public TextureRef {
 public:
  MutableTextureRef() = default;
  MutableTextureRef(const MutableTextureRef&) = default;
  MutableTextureRef(MutableTextureRef&&) = default;

  MutableTextureRef(MutableTextureRefInterface* ptr)
      : TextureRef(static_cast<TextureRefInterface*>(ptr)) {}

  MutableTextureRef& operator=(const MutableTextureRef&) = default;
  MutableTextureRef& operator=(MutableTextureRef&&) = default;

  MutableTextureRefInterface& operator*() const {
    return dynamic_cast<MutableTextureRefInterface&>(*Ptr);
  }
  MutableTextureRefInterface* operator->() const {
    return dynamic_cast<MutableTextureRefInterface*>(Ptr);
  }
  MutableTextureRefInterface* Get() const {
    return dynamic_cast<MutableTextureRefInterface*>(Ptr);
  }
};

}  // namespace Impacto
