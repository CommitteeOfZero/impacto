#pragma once

#include "../nv12frame.h"

#include <d3d9.h>

namespace Impacto {
namespace DirectX9 {

class DX9NV12Frame : public NV12Frame {
  friend class Renderer;

 public:
  DX9NV12Frame(IDirect3DDevice9* device);

  void Init(float width, float height) override;

  void Submit(const void* luma, const void* cbcr) override;
  void Release() override;

 protected:
  IDirect3DDevice9* Device;

  IDirect3DTexture9* Luma;
  IDirect3DTexture9* CbCr;
};

}  // namespace DirectX9
}  // namespace Impacto