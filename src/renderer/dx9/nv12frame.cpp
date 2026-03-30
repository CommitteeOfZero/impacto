#include "nv12frame.h"

#include "../../log.h"

namespace Impacto {
namespace DirectX9 {

DX9NV12Frame::DX9NV12Frame(IDirect3DDevice9* device) { Device = device; }

void DX9NV12Frame::Init(float width, float height) {
  Width = width;
  Height = height;
  auto err = Device->CreateTexture((UINT)Width, (UINT)Height, 1, 0, D3DFMT_A8,
                                   D3DPOOL_MANAGED, &Luma, nullptr);
  err = Device->CreateTexture((UINT)(Width / 2), (UINT)(Height / 2), 1, 0,
                              D3DFMT_A8L8, D3DPOOL_MANAGED, &CbCr, nullptr);
}

void DX9NV12Frame::Submit(const void* luma, const void* cbcr) {
  D3DLOCKED_RECT lockRect;
  auto err = Luma->LockRect(0, &lockRect, NULL, 0);
  memcpy(lockRect.pBits, luma, (size_t)(Width * Height));
  err = Luma->UnlockRect(0);

  err = CbCr->LockRect(0, &lockRect, NULL, 0);
  memcpy(lockRect.pBits, cbcr, (size_t)((Width) * (Height / 2)));
  err = CbCr->UnlockRect(0);
}

void DX9NV12Frame::Release() {
  Luma->Release();
  CbCr->Release();
}

}  // namespace DirectX9
}  // namespace Impacto