#pragma once

#include <d3d9.h>
#include <map>

namespace Impacto {
namespace DirectX9 {

extern std::map<uint32_t, IDirect3DTexture9*> Textures;

}  // namespace DirectX9
}  // namespace Impacto