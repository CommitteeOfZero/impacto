#pragma once

#include <glm/glm.hpp>

#include <span>

#include "../util.h"

namespace Impacto {

class TextureInterface {
 public:
  virtual ~TextureInterface() = default;

  [[nodiscard]] virtual bool IsValid() const = 0;

  [[nodiscard]] virtual glm::vec<2, size_t> GetDimensions() const = 0;

  [[nodiscard]] virtual uint64_t GetTextureId() const = 0;
};

class MutableTextureInterface : public virtual TextureInterface {
 public:
  virtual void Update(std::span<const uint8_t> data, size_t rowStride) = 0;
};

class TextureRef {
 public:
  TextureRef() = default;
  TextureRef(const TextureRef& other) { *this = other; }
  TextureRef(TextureRef&& other) { *this = std::move(other); }
  virtual ~TextureRef();

  TextureRef(TextureInterface* ptr);

  TextureRef& operator=(const TextureRef&);
  TextureRef& operator=(TextureRef&&);

  TextureInterface& operator*() const { return *Ptr; }
  TextureInterface* operator->() const { return Ptr; }
  TextureInterface* Get() const { return Ptr; }

  bool operator==(const TextureRef&) const = default;

  bool IsValid() const { return Ptr != nullptr && Ptr->IsValid(); }

 protected:
  TextureInterface* Ptr = nullptr;
};

class MutableTextureRef final : public TextureRef {
 public:
  MutableTextureRef() = default;
  MutableTextureRef(const MutableTextureRef&) = default;
  MutableTextureRef(MutableTextureRef&&) = default;

  MutableTextureRef(MutableTextureInterface* ptr)
      : TextureRef(static_cast<TextureInterface*>(ptr)) {}

  MutableTextureRef& operator=(const MutableTextureRef&) = default;
  MutableTextureRef& operator=(MutableTextureRef&&) = default;

  MutableTextureInterface& operator*() const {
    return dynamic_cast<MutableTextureInterface&>(*Ptr);
  }
  MutableTextureInterface* operator->() const {
    return dynamic_cast<MutableTextureInterface*>(Ptr);
  }
  MutableTextureInterface* Get() const {
    return dynamic_cast<MutableTextureInterface*>(Ptr);
  }
};

}  // namespace Impacto
