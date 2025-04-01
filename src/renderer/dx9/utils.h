#pragma once

#include <d3d9.h>
#include <ankerl/unordered_dense.h>
#include <cstdint>

namespace Impacto {
namespace DirectX9 {

inline ankerl::unordered_dense::map<uint32_t, IDirect3DTexture9*> Textures;

}  // namespace DirectX9
}  // namespace Impacto