#pragma once

#include "../yuvframe.h"

#include <d3d9.h>

namespace Impacto {
namespace DirectX9 {

class DX9YUVFrame : public YUVFrame {
  friend class Renderer;

 public:
  DX9YUVFrame(IDirect3DDevice9* device);

  void Init(float width, float height) override;

  void Submit(void* luma, void* cb, void* cr) override;
  void Release() override;

 protected:
  IDirect3DDevice9* Device;

  IDirect3DTexture9* Luma;
  IDirect3DTexture9* Cb;
  IDirect3DTexture9* Cr;
};

}  // namespace DirectX9
}  // namespace Impacto