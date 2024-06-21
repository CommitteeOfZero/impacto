#pragma once

#include <d3d9.h>
#include <map>
#include <cstdint>

namespace Impacto {
namespace DirectX9 {

inline std::map<uint32_t, IDirect3DTexture9*> Textures;

}  // namespace DirectX9
}  // namespace Impacto