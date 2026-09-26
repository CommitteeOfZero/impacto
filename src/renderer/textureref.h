#pragma once

#include <glm/glm.hpp>

#include <span>

namespace Impacto {

enum class TextureRefType { Plain, Mutable };

class TextureRefInterface {
 public:
  TextureRefInterface() = default;
  TextureRefInterface(const TextureRefInterface&) = default;
  TextureRefInterface(TextureRefInterface&&) = default;
  virtual ~TextureRefInterface() = default;

  TextureRefInterface& operator=(const TextureRefInterface&) = default;
  TextureRefInterface& operator=(TextureRefInterface&&) = default;

  [[nodiscard]] virtual bool IsValid() const = 0;

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const {
    assert(IsValid());
    return Dimensions;
  }

  [[nodiscard]] virtual uint64_t GetTextureId() const = 0;

  [[nodiscard]] constexpr virtual TextureRefType GetType() const {
    return TextureRefType::Plain;
  }

 protected:
  glm::vec<2, size_t> Dimensions = {0, 0};
};

class MutableTextureRefInterface : public TextureRefInterface {
 public:
  virtual void Update(std::span<const uint8_t> data, size_t rowStride) = 0;

  [[nodiscard]] constexpr TextureRefType GetType() const override {
    return TextureRefType::Mutable;
  }
};

struct TextureRef {
 public:
  TextureRef() = default;
  TextureRef(const TextureRef& other) { *this = other; }
  TextureRef(TextureRef&& other) { *this = std::move(other); }
  ~TextureRef();

  TextureRef(TextureRefInterface* ptr) : Ptr(ptr) {}

  TextureRef& operator=(const TextureRef&);
  TextureRef& operator=(TextureRef&&);

  bool operator==(const TextureRef& other) const { return Ptr == other.Ptr; }

  [[nodiscard]] virtual bool IsValid() const {
    return Ptr != nullptr && Ptr->IsValid();
  }

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const {
    assert(IsValid());
    return Ptr->GetDimensions();
  }

  [[nodiscard]] uint64_t GetTextureId() const {
    assert(IsValid());
    return Ptr->GetTextureId();
  }

  [[nodiscard]] TextureRefInterface* Get() const { return Ptr; }

 private:
  TextureRefInterface* Ptr = nullptr;
};

struct MutableTextureRef {
 public:
  MutableTextureRef() = default;
  MutableTextureRef(const MutableTextureRef& other) { *this = other; }
  MutableTextureRef(MutableTextureRef&& other) { *this = std::move(other); }
  ~MutableTextureRef();

  MutableTextureRef(MutableTextureRefInterface* ptr) : Ptr(ptr) {}

  MutableTextureRef& operator=(const MutableTextureRef&);
  MutableTextureRef& operator=(MutableTextureRef&&);

  bool operator==(const MutableTextureRef& other) const {
    return Ptr == other.Ptr;
  }

  [[nodiscard]] virtual bool IsValid() const {
    return Ptr != nullptr && Ptr->IsValid();
  }

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const {
    assert(IsValid());
    return Ptr->GetDimensions();
  }

  [[nodiscard]] uint64_t GetTextureId() const {
    assert(IsValid());
    return Ptr->GetTextureId();
  }

  void Update(std::span<const uint8_t> data, size_t rowStride) {
    assert(IsValid());
    Ptr->Update(data, rowStride);
  }

  [[nodiscard]] MutableTextureRefInterface* Get() const { return Ptr; }
  operator MutableTextureRefInterface*() const { return Get(); }

 private:
  MutableTextureRefInterface* Ptr = nullptr;
};

}  // namespace Impacto
