#include "framebuffer.h"

#include <array>

namespace Impacto::Bgfx {

FrameBuffer::FrameBuffer(FrameBuffer&& other) { *this = std::move(other); }

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) {
  if (&other == this) return *this;
  Reset(true);
  if (!other.IsInit) return *this;

  IsInit = true;
  FrameBufferHandle = other.FrameBufferHandle;
  ColorAttachment = other.ColorAttachment;
  DepthStencilAttachment = other.DepthStencilAttachment;

  other.Reset(false);

  return *this;
}

FrameBuffer::~FrameBuffer() { Reset(true); }

FrameBuffer::FrameBuffer(const uint16_t width, const uint16_t height)
    : ColorAttachment(bgfx::createTexture2D(width, height, false, 1,
                                            bgfx::TextureFormat::RGBA8,
                                            BGFX_TEXTURE_RT)),
      DepthStencilAttachment(bgfx::createTexture2D(width, height, false, 1,
                                                   bgfx::TextureFormat::D24S8,
                                                   BGFX_TEXTURE_RT)) {
  assert(bgfx::isValid(ColorAttachment));
  assert(bgfx::isValid(*DepthStencilAttachment));

  std::array<bgfx::Attachment, 2> attachments;
  attachments[0].init(ColorAttachment);
  attachments[1].init(*DepthStencilAttachment);

  FrameBufferHandle = bgfx::createFrameBuffer(
      static_cast<uint8_t>(attachments.size()), attachments.data(), true);
  assert(bgfx::isValid(FrameBufferHandle));

  IsInit = true;
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

  frameBuffer.IsInit = true;

  return frameBuffer;
}

void FrameBuffer::Reset(bool cleanUpResources) {
  if (!IsInit) return;
  IsInit = false;

  DepthStencilAttachment.reset();

  if (cleanUpResources) {
    bgfx::destroy(FrameBufferHandle);
  }
}

}  // namespace Impacto::Bgfx
