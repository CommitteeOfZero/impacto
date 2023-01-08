#include "shader.h"
#include "renderer.h"

#include <d3dcompiler.h>

namespace Impacto {
namespace DirectX9 {

static char const ShaderPath[] = "./shaders/dx9";
static char const FragShaderExtension[] = "_frag.hlsl";
static char const VertShaderExtension[] = "_vert.hlsl";

void Shader::Compile(char const* name, IDirect3DDevice9* device,
                     IDirect3DVertexDeclaration9* vertexDeclaration,
                     D3D_SHADER_MACRO* macros) {
  ImpLog(LL_Debug, LC_Render, "Compiling shader \"%s\"\n", name);

  VertexDeclaration = vertexDeclaration;

  size_t pathSz =
      std::max(
          snprintf(NULL, 0, "%s/%s%s", ShaderPath, name, FragShaderExtension),
          snprintf(NULL, 0, "%s/%s%s", ShaderPath, name, VertShaderExtension)) +
      1;

  char* fullPath = (char*)ImpStackAlloc(pathSz);

  ID3DBlob* vertexShaderBuffer{};
  ID3DBlob* errorBlob{};
  ID3DBlob* pixelShaderBuffer{};

  // Vertex shader
  sprintf(fullPath, "%s/%s%s", ShaderPath, name, VertShaderExtension);
  size_t sourceRawSz;
  char* source = (char*)SDL_LoadFile(fullPath, &sourceRawSz);
  if (!source) {
    ImpLog(LL_Debug, LC_Render, "Failed to read shader source file\n");
    return;
  }
  auto result =
      D3DCompile(source, sourceRawSz, nullptr, macros, nullptr, "main",
                 "vs_3_0", 0, 0, &vertexShaderBuffer, &errorBlob);
  if (FAILED(result)) {
    ImpLog(LL_Debug, LC_Render, "Failed to compile shader source file %s\n",
           errorBlob->GetBufferPointer());
    return;
  }

  result = device->CreateVertexShader(
      (DWORD*)vertexShaderBuffer->GetBufferPointer(), &VertexShader);
  if (FAILED(result)) return;

  // Pixel shader
  sprintf(fullPath, "%s/%s%s", ShaderPath, name, FragShaderExtension);
  source = (char*)SDL_LoadFile(fullPath, &sourceRawSz);
  if (!source) {
    ImpLog(LL_Debug, LC_Render, "Failed to read shader source file\n");
    return;
  }
  result = D3DCompile(source, sourceRawSz, nullptr, macros, nullptr, "main",
                      "ps_3_0", 0, 0, &pixelShaderBuffer, &errorBlob);
  if (FAILED(result)) {
    ImpLog(LL_Debug, LC_Render, "Failed to compile shader source file %s\n",
           errorBlob->GetBufferPointer());
    return;
  }

  result = device->CreatePixelShader(
      (DWORD*)pixelShaderBuffer->GetBufferPointer(), &PixelShader);
  if (FAILED(result)) return;

  vertexShaderBuffer->Release();
  pixelShaderBuffer->Release();
  if (errorBlob) errorBlob->Release();
}

void Shader::UseShader(IDirect3DDevice9* device) {
  device->SetVertexDeclaration(VertexDeclaration);
  device->SetVertexShader(VertexShader);
  device->SetPixelShader(PixelShader);
}

}  // namespace DirectX9
}  // namespace Impacto