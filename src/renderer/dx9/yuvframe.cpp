#include "yuvframe.h"

#include "../../log.h"

namespace Impacto {
namespace DirectX9 {

DX9YUVFrame::DX9YUVFrame(IDirect3DDevice9* device) { Device = device; }

void DX9YUVFrame::Init(float width, float height) {
  Width = width;
  Height = height;
  D3DFORMAT imageFormat = D3DFMT_A8;
  auto err = Device->CreateTexture((UINT)Width, (UINT)Height, 1, 0, imageFormat,
                                   D3DPOOL_MANAGED, &Luma, nullptr);
  err = Device->CreateTexture((UINT)(Width / 2), (UINT)(Height / 2), 1, 0,
                              imageFormat, D3DPOOL_MANAGED, &Cb, nullptr);
  err = Device->CreateTexture((UINT)(Width / 2), (UINT)(Height / 2), 1, 0,
                              imageFormat, D3DPOOL_MANAGED, &Cr, nullptr);
}

void DX9YUVFrame::Submit(void* luma, void* cb, void* cr) {
  int imageSize = 0;

  D3DLOCKED_RECT lockRect;
  auto err = Luma->LockRect(0, &lockRect, NULL, 0);
  memcpy(lockRect.pBits, luma, (size_t)(Width * Height));
  err = Luma->UnlockRect(0);

  err = Cb->LockRect(0, &lockRect, NULL, 0);
  memcpy(lockRect.pBits, cb, (size_t)((Width / 2) * (Height / 2)));
  err = Cb->UnlockRect(0);

  err = Cr->LockRect(0, &lockRect, NULL, 0);
  memcpy(lockRect.pBits, cr, (size_t)((Width / 2) * (Height / 2)));
  err = Cr->UnlockRect(0);
}

void DX9YUVFrame::Release() {
  Luma->Release();
  Cb->Release();
  Cr->Release();
}

}  // namespace DirectX9
}  // namespace Impacto