#pragma once

#include "../../util.h"

#include <bgfx/bgfx.h>

#include <utility>

namespace Impacto::Bgfx {

template <typename T>
concept BgfxHandleType =
    is_any_of_v<T, bgfx::DynamicIndexBufferHandle,
                bgfx::DynamicVertexBufferHandle, bgfx::FrameBufferHandle,
                bgfx::IndexBufferHandle, bgfx::IndirectBufferHandle,
                bgfx::OcclusionQueryHandle, bgfx::ProgramHandle,
                bgfx::ShaderHandle, bgfx::TextureHandle, bgfx::UniformHandle,
                bgfx::VertexBufferHandle, bgfx::VertexLayoutHandle>;

template <typename T>
  requires BgfxHandleType<T>
struct BgfxHandle {
  BgfxHandle() = default;
  BgfxHandle(T handle) : Handle(handle) {}
  BgfxHandle(const BgfxHandle<T>&) = delete;
  BgfxHandle(BgfxHandle<T>&& other) { *this = std::move(other); }

  ~BgfxHandle() {
    if (Handle.idx == bgfx::kInvalidHandle) return;
    bgfx::destroy(Handle);
    Handle = {bgfx::kInvalidHandle};
  }

  BgfxHandle<T>& operator=(const BgfxHandle<T>&) = delete;
  BgfxHandle<T>& operator=(BgfxHandle<T>&& other) {
    if (&other == this) return *this;
    if (*this != other && Handle.idx != bgfx::kInvalidHandle) {
      bgfx::destroy(Handle);
    }

    Handle = std::exchange(other.Handle, {bgfx::kInvalidHandle});
    return *this;
  }

  BgfxHandle<T>& operator=(const T&& handle) {
    if (Handle.idx == handle.idx) return *this;
    if (Handle.idx != bgfx::kInvalidHandle) bgfx::destroy(Handle);
    Handle = handle;
    return *this;
  }

  [[nodiscard]] bool operator==(const T& other) const {
    return Handle.idx == other.idx;
  }
  [[nodiscard]] bool operator!=(const T& other) const {
    return !(*this == other);
  }

  [[nodiscard]] bool operator==(const BgfxHandle<T>& other) const {
    return *this == other.Handle;
  }
  [[nodiscard]] bool operator!=(const BgfxHandle<T>& other) const {
    return !(*this == other);
  }

  [[nodiscard]] T Get() const { return Handle; }
  [[nodiscard]] T& Get() { return Handle; }
  [[nodiscard]] operator T() const { return Get(); }

  [[nodiscard]] bool IsValid() const { return bgfx::isValid(Handle); }

  T Handle = {bgfx::kInvalidHandle};
};

}  // namespace Impacto::Bgfx
