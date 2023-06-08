#pragma once

#include <d3d9.h>
#include <d3dcommon.h>

namespace Impacto {
namespace DirectX9 {

class Shader {
 public:
  void Compile(char const* name, IDirect3DDevice9* device,
               IDirect3DVertexDeclaration9* vertexDeclaration,
               D3D_SHADER_MACRO* macros = nullptr);
  void UseShader(IDirect3DDevice9* device);

 private:
  IDirect3DVertexDeclaration9* VertexDeclaration;

  IDirect3DVertexShader9* VertexShader;
  IDirect3DPixelShader9* PixelShader;
};

}  // namespace DirectX9
}  // namespace Impacto