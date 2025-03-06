#pragma once

#include <d3d9.h>
#include <flat_hash_map.hpp>
#include <cstdint>

namespace Impacto {
namespace DirectX9 {

inline ska::flat_hash_map<uint32_t, IDirect3DTexture9*> Textures;

}  // namespace DirectX9
}  // namespace Impacto