#include "framebuffer.h"

#include <array>

namespace Impacto::Bgfx {

FrameBuffer::FrameBuffer(FrameBuffer&& other) { *this = std::move(other); }

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) {
  if (&other == this) return *this;
  Reset(true);

  FrameBufferHandle = other.FrameBufferHandle;
  ColorAttachment = other.ColorAttachment;

  other.Reset(false);

  return *this;
}

FrameBuffer::~FrameBuffer() { Reset(true); }

FrameBuffer::FrameBuffer(const uint16_t width, const uint16_t height)
    : ColorAttachment(bgfx::createTexture2D(width, height, false, 1,
                                            bgfx::TextureFormat::RGBA8,
                                            BGFX_TEXTURE_RT)) {
  assert(bgfx::isValid(ColorAttachment));

  FrameBufferHandle = bgfx::createFrameBuffer(1, &ColorAttachment, true);
  assert(bgfx::isValid(FrameBufferHandle));
}

FrameBuffer FrameBuffer::CreateBackBufferFrameBuffer() {
  FrameBuffer frameBuffer;

  frameBuffer.ColorAttachment =
      bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1,
                            bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
  assert(bgfx::isValid(frameBuffer.ColorAttachment));

  frameBuffer.FrameBufferHandle =
      bgfx::createFrameBuffer(1, &frameBuffer.ColorAttachment, true);
  assert(bgfx::isValid(frameBuffer.FrameBufferHandle));

  return frameBuffer;
}

void FrameBuffer::Reset(bool cleanUpResources) {
  if (cleanUpResources) {
    if (bgfx::isValid(FrameBufferHandle)) bgfx::destroy(FrameBufferHandle);
  }

  FrameBufferHandle.idx = bgfx::kInvalidHandle;
  ColorAttachment.idx = bgfx::kInvalidHandle;
}

}  // namespace Impacto::Bgfx
