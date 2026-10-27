#pragma once

#include "bgfxhandle.h"
#include "texture.h"

#include <bgfx/bgfx.h>
#include <glm/ext/vector_uint2_sized.hpp>

#include <cassert>
#include <span>
#include <optional>

namespace Impacto::Bgfx {

class FrameBuffer {
 public:
  FrameBuffer() = default;
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer(FrameBuffer&&) = default;
  ~FrameBuffer() = default;

  FrameBuffer& operator=(const FrameBuffer&) = delete;
  FrameBuffer& operator=(FrameBuffer&&) = default;

  FrameBuffer(uint16_t width, uint16_t height);

  [[nodiscard]] operator bgfx::FrameBufferHandle() {
    return GetFrameBufferHandle();
  }
  [[nodiscard]] bgfx::FrameBufferHandle GetFrameBufferHandle() {
    assert(FrameBufferHandle.IsValid());
    return FrameBufferHandle;
  }

  [[nodiscard]] bgfx::TextureHandle GetColorAttachmentHandle() {
    assert(ColorAttachment.IsValid());
    return ColorAttachment;
  }

  [[nodiscard]] NonOwnedTexture& GetTexture() {
    assert(TextureObject.IsValid());
    return TextureObject;
  }

  [[nodiscard]] glm::u16vec2 GetSize() const { return Size; }

 private:
  BgfxHandle<bgfx::FrameBufferHandle> FrameBufferHandle;
  BgfxHandle<bgfx::TextureHandle> ColorAttachment;
  BgfxHandle<bgfx::TextureHandle> StencilAttachment;

  NonOwnedTexture TextureObject;

  glm::u16vec2 Size;
};

}  // namespace Impacto::Bgfx
