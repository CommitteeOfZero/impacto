#include "renderer.h"

#include <SDL_syswm.h>
#include <array>

#include "../../profile/game.h"
#include "../../game.h"
#include "3d/scene.h"
#include "utils.h"

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui_impl_dx9.h>
#endif

namespace Impacto {
namespace DirectX9 {

void Renderer::Init() {
  if (IsInit) return;
  ImpLog(LogLevel::Info, LogChannel::Render,
         "Initializing Renderer2D DirectX 9 system\n");
  IsInit = true;

  DXWindow = new DirectX9Window();
  DXWindow->Init();
  Window = (BaseWindow*)DXWindow;

  Interface = Direct3DCreate9(D3D_SDK_VERSION);

  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(Window->SDLWindow, &wmInfo);
  HWND hWnd = wmInfo.info.win.window;

  D3DPRESENT_PARAMETERS d3dpp{};
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow = hWnd;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

  auto res = Interface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                     D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                     &d3dpp, &Device);
  Device->CreateVertexBuffer(VertexBufferSize, 0, 0, D3DPOOL_MANAGED,
                             &VertexBufferDevice, NULL);
  Device->CreateIndexBuffer(IndexBufferCount * sizeof(uint16_t), 0,
                            D3DFMT_INDEX16, D3DPOOL_MANAGED, &IndexBufferDevice,
                            NULL);

  Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
  Device->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
  Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  Device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
  Device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
  Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  IDirect3DVertexDeclaration9* vertexDeclaration;
  std::vector<D3DVERTEXELEMENT9> vertexDeclDesc{
      {0, offsetof(VertexBufferSprites, Position), D3DDECLTYPE_FLOAT2,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(VertexBufferSprites, UV), D3DDECLTYPE_FLOAT2,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      {0, offsetof(VertexBufferSprites, Tint), D3DDECLTYPE_FLOAT4,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
      {0, offsetof(VertexBufferSprites, MaskUV), D3DDECLTYPE_FLOAT2,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
      D3DDECL_END()};
  Device->CreateVertexDeclaration(vertexDeclDesc.data(), &vertexDeclaration);

  ShaderSprite = new Shader();
  ShaderSprite->Compile("Sprite", Device, vertexDeclaration);
  ShaderSpriteInverted = new Shader();
  ShaderSpriteInverted->Compile("Sprite_inverted", Device, vertexDeclaration);
  ShaderMaskedSprite = new Shader();
  ShaderMaskedSprite->Compile("MaskedSprite", Device, vertexDeclaration);
  ShaderMaskedSpriteNoAlpha = new Shader();
  ShaderMaskedSpriteNoAlpha->Compile("MaskedSpriteNoAlpha", Device,
                                     vertexDeclaration);
  ShaderYUVFrame = new Shader();
  ShaderYUVFrame->Compile("YUVFrame", Device, vertexDeclaration);
  ShaderCCMessageBox = new Shader();
  ShaderCCMessageBox->Compile("CCMessageBoxSprite", Device, vertexDeclaration);
  ShaderCHLCCMenuBackground = new Shader();
  ShaderCHLCCMenuBackground->Compile("CHLCCMenuBackground", Device,
                                     vertexDeclaration);

  CurrentShader = ShaderSprite;
  CurrentShader->UseShader(Device);

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene = new Scene3D(DXWindow, Device);
    Scene->Init();
  }

  // Fill index buffer with quads
  int index = 0;
  int vertex = 0;
  while (index + 6 <= IndexBufferCount) {
    // bottom-left -> top-left -> top-right
    IndexBuffer[index] = vertex + 0;
    IndexBuffer[index + 1] = vertex + 1;
    IndexBuffer[index + 2] = vertex + 2;
    // bottom-left -> top-right -> bottom-right
    IndexBuffer[index + 3] = vertex + 0;
    IndexBuffer[index + 4] = vertex + 2;
    IndexBuffer[index + 5] = vertex + 3;
    index += 6;
    vertex += 4;
  }

  // Make 1x1 white pixel for colored rectangles
  Texture rectTexture;
  rectTexture.Load1x1(0xFF, 0xFF, 0xFF, 0xFF);
  SpriteSheet rectSheet(1.0f, 1.0f);
  rectSheet.Texture = rectTexture.Submit();
  RectSprite = Sprite(rectSheet, 0.0f, 0.0f, 1.0f, 1.0f);

#ifndef IMPACTO_DISABLE_IMGUI
  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForD3D(DXWindow->SDLWindow);
  ImGui_ImplDX9_Init(Device);
#endif
}

void Renderer::Shutdown() {
  if (!IsInit) return;
  IsInit = false;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene->Shutdown();
  }

  VertexBufferDevice->Release();
  IndexBufferDevice->Release();
  Device->Release();
  Interface->Release();
}

#ifndef IMPACTO_DISABLE_IMGUI
void Renderer::ImGuiBeginFrame() {
  ImGui_ImplDX9_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}
#endif

void Renderer::BeginFrame() {
  if (Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->BeginFrame() called before EndFrame()\n");
    return;
  }

  Drawing = true;
  VertexBufferFill = 0;
  IndexBufferFill = 0;

  Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
  Device->BeginScene();
}

void Renderer::BeginFrame2D() {
  Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
  Device->SetRenderState(D3DRS_ZWRITEENABLE, 0);
  Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void Renderer::EndFrame() {
  if (!Drawing) return;
  Flush();
  Drawing = false;

#ifndef IMPACTO_DISABLE_IMGUI
  ImGui::Render();
  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
#endif

  Device->EndScene();
  Device->Present(NULL, NULL, NULL, NULL);
  CurrentShader = 0;
  CurrentTexture = -1;
}

uint32_t Renderer::SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                 int height) {
  int imageSize = 0;
  D3DFORMAT imageFormat = D3DFMT_A1;
  uint8_t* newBuffer;

  switch (format) {
    case TexFmt_RGBA: {
      imageSize = width * height * 4;
      imageFormat = D3DFMT_A8R8G8B8;
      newBuffer = (uint8_t*)malloc(imageSize);
      int num = width * height;
      for (int i = 0; i < num; i++) {
        newBuffer[4 * i] = buffer[4 * i + 2];
        newBuffer[4 * i + 1] = buffer[4 * i + 1];
        newBuffer[4 * i + 2] = buffer[4 * i];
        newBuffer[4 * i + 3] = buffer[4 * i + 3];
      }
    } break;
    case TexFmt_RGB: {
      imageSize = width * height * 4;
      imageFormat = D3DFMT_A8R8G8B8;
      newBuffer = (uint8_t*)malloc(imageSize);
      int num = width * height;
      for (int i = 0; i < num; i++) {
        newBuffer[4 * i] = buffer[3 * i + 2];
        newBuffer[4 * i + 1] = buffer[3 * i + 1];
        newBuffer[4 * i + 2] = buffer[3 * i];
        newBuffer[4 * i + 3] = 0xff;
      }
    } break;
    case TexFmt_U8:
      imageSize = width * height;
      imageFormat = D3DFMT_A8;
  }

  IDirect3DTexture9* texture;
  auto err = Device->CreateTexture(width, height, 1, 0, imageFormat,
                                   D3DPOOL_MANAGED, &texture, nullptr);
  D3DLOCKED_RECT lockRect;
  err = texture->LockRect(0, &lockRect, NULL, 0);

  if (format == TexFmt_RGBA || format == TexFmt_RGB) {
    memcpy(lockRect.pBits, newBuffer, imageSize);
    free(newBuffer);
  } else {
    memcpy(lockRect.pBits, buffer, imageSize);
  }

  if (FAILED(texture->UnlockRect(0))) {
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to load texture!\n");
    Window->Shutdown();
  }

  auto id = NextTextureId;
  NextTextureId += 1;
  Textures[id] = texture;

  return id;
}

void Renderer::FreeTexture(uint32_t id) {
  auto textureItr = Textures.find(id);
  if (textureItr != Textures.end()) {
    if (textureItr->second != nullptr) {
      textureItr->second->Release();
    }
    Textures.erase(textureItr);
  }
}

YUVFrame* Renderer::CreateYUVFrame(float width, float height) {
  VideoFrameInternal = new DX9YUVFrame(Device);
  VideoFrameInternal->Init(width, height);
  return (YUVFrame*)VideoFrameInternal;
}

void Renderer::DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                          const glm::mat4 transformation,
                          const std::span<const glm::vec4, 4> tints,
                          const glm::vec3 colorShift, const bool inverted,
                          const bool disableBlend,
                          const bool textureWrapRepeat) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  // Are we in sprite mode?
  if (inverted)
    EnsureShader(ShaderSpriteInverted);
  else
    EnsureShader(ShaderSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
            sizeof(VertexBufferSprites));
  QuadSetPosition(CornersQuad(dest).Transform(transformation),
                  &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawMaskedSprite(
    const Sprite& sprite, const Sprite& mask, const CornersQuad& spriteDest,
    const CornersQuad& maskDest, int alpha, const int fadeRange,
    const glm::mat4 spriteTransformation, const glm::mat4 maskTransformation,
    const std::span<const glm::vec4, 4> tints, const bool isInverted,
    const bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSpriteOverlay() called before BeginFrame()\n");
    return;
  }

  if (sprite.Sheet.IsScreenCap) Flush();

  alpha = std::clamp(alpha, 0, fadeRange + 256);
  const float alphaRange = 256.0f / fadeRange;
  const float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  std::array<float, 4> alphaRes = {alphaRange, constAlpha, 0.0f, 0.0f};
  const BOOL isInvertedB = (BOOL)isInverted;
  const BOOL isSameTextureB = (BOOL)isSameTexture;

  Flush();

  EnsureShader(ShaderMaskedSprite);
  Device->SetTexture(0, Textures[sprite.Sheet.Texture]);
  Device->SetTexture(1, Textures[mask.Sheet.Texture]);

  Device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  Device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  Device->SetPixelShaderConstantF(0, alphaRes.data(), 1);
  Device->SetPixelShaderConstantB(0, &isInvertedB, 1);
  Device->SetPixelShaderConstantB(1, &isSameTextureB, 1);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
            sizeof(VertexBufferSprites));
  QuadSetUV(CornersQuad(maskDest).Transform(maskTransformation),
            sprite.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(CornersQuad(spriteDest).Transform(spriteTransformation),
                  &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawMaskedSpriteOverlay(
    const Sprite& sprite, const Sprite& mask, const CornersQuad& spriteDest,
    const CornersQuad& maskDest, int alpha, const int fadeRange,
    const glm::mat4 spriteTransformation, const glm::mat4 maskTransformation,
    const std::span<const glm::vec4, 4> tints, const bool isInverted,
    const bool useMaskAlpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSpriteOverlay() called before BeginFrame()\n");
    return;
  }

  if (sprite.Sheet.IsScreenCap) Flush();

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  std::array<float, 4> alphaRes = {alphaRange, constAlpha, 0.0f, 0.0f};
  BOOL isInvertedB = (BOOL)isInverted;
  BOOL isSameTextureB = (BOOL) false;
  BOOL useMaskAlphaB = (BOOL)useMaskAlpha;

  if (useMaskAlpha) {
    EnsureShader(ShaderMaskedSprite);
    Device->SetPixelShaderConstantB(1, &isSameTextureB, 1);
  } else {
    EnsureShader(ShaderMaskedSpriteNoAlpha);
  }

  Device->SetTexture(0, Textures[sprite.Sheet.Texture]);
  Device->SetTexture(1, Textures[mask.Sheet.Texture]);

  Device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  Device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  Device->SetPixelShaderConstantF(0, alphaRes.data(), 1);
  Device->SetPixelShaderConstantB(0, &isInvertedB, 1);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
            sizeof(VertexBufferSprites));
  QuadSetUV(CornersQuad(maskDest).Transform(maskTransformation),
            mask.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(CornersQuad(spriteDest).Transform(spriteTransformation),
                  &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawVertices(const SpriteSheet& sheet,
                            const SpriteSheet* const mask,
                            const ShaderProgramType shaderType,
                            const std::span<const VertexBufferSprites> vertices,
                            const std::span<const uint16_t> indices,
                            const glm::mat4 transformation,
                            const bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called before BeginFrame()\n");
    return;
  }

  // The index buffer needs to be flushed
  Flush();

  if (mask != nullptr) {
    std::array<float, 4> alphaRes = {1.0f, 0.0f, 0.0f, 0.0f};
    BOOL isInvertedB = (BOOL)inverted;

    EnsureShader(ShaderMaskedSpriteNoAlpha);

    Device->SetTexture(0, Textures[sheet.Texture]);
    Device->SetTexture(1, Textures[mask->Texture]);

    Device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    Device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    Device->SetPixelShaderConstantF(0, alphaRes.data(), 1);
    Device->SetPixelShaderConstantB(0, &isInvertedB, 1);

  } else {
    if (inverted)
      EnsureShader(ShaderSpriteInverted);
    else
      EnsureShader(ShaderSprite);

    EnsureTextureBound(sheet.Texture);
  }

  // Push vertices
  VertexBufferSprites* vertexBuffer =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += vertices.size_bytes();

  const auto vertexInfoToNDC = [this,
                                transformation](VertexBufferSprites info) {
    info.Position = DesignToNDC(
        glm::vec2(transformation * glm::vec4(info.Position, 0.0f, 1.0f)));
    return info;
  };
  std::transform(vertices.begin(), vertices.end(), vertexBuffer,
                 vertexInfoToNDC);

  // Push indices
  IndexBufferFill += indices.size();
  std::copy(indices.begin(), indices.end(), IndexBuffer);

  // Flush again and bind back our buffer
  Flush();

  // ReFill index buffer with quads
  int index = 0;
  int vertex = 0;
  while (index + 6 <= IndexBufferCount) {
    // bottom-left -> top-left -> top-right
    IndexBuffer[index] = vertex + 0;
    IndexBuffer[index + 1] = vertex + 1;
    IndexBuffer[index + 2] = vertex + 2;
    // bottom-left -> top-right -> bottom-right
    IndexBuffer[index + 3] = vertex + 0;
    IndexBuffer[index + 4] = vertex + 2;
    IndexBuffer[index + 5] = vertex + 3;
    index += 6;
    vertex += 4;
  }
}

void Renderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, float effectCt) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCCMessageBox() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  EnsureShader(ShaderCCMessageBox);

  Device->SetTexture(0, Textures[sprite.Sheet.Texture]);
  CurrentTexture = sprite.Sheet.Texture;
  Device->SetTexture(1, Textures[mask.Sheet.Texture]);

  // This is cursed man, idk
  float alphaRes[] = {alphaRange, constAlpha, effectCt, 0.0f};
  Device->SetPixelShaderConstantF(0, alphaRes, 1);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
            sizeof(VertexBufferSprites));
  QuadSetUV(mask.Bounds, mask.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                                       const RectF& dest, float alpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCHLCCMenuBackground() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0.0f)
    alpha = 0;
  else if (alpha > 1.0f)
    alpha = 1.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  EnsureShader(ShaderCHLCCMenuBackground);

  Device->SetTexture(0, Textures[sprite.Sheet.Texture]);
  CurrentTexture = sprite.Sheet.Texture;
  Device->SetTexture(1, Textures[mask.Sheet.Texture]);

  // This is cursed man, idk
  float alphaRes[] = {alpha, 0.0f, 0.0f, 0.0f};
  Device->GetPixelShaderConstantF(0, alphaRes, 1);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
                   sizeof(VertexBufferSprites));

  QuadSetUV(mask.Bounds, mask.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++)
    vertices[i].Tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::EnsureSpaceAvailable(int vertices, int vertexSize, int indices) {
  if (VertexBufferFill + vertices * vertexSize > VertexBufferSize ||
      IndexBufferFill + indices > IndexBufferCount) {
    ImpLogSlow(
        LogLevel::Trace, LogChannel::Render,
        "Renderer->EnsureSpaceAvailable flushing because buffers full at "
        "VertexBufferFill=0x{:08x},IndexBufferFill=0x{:08x}\n",
        VertexBufferFill, IndexBufferFill);
    Flush();
  }
}

void Renderer::EnsureTextureBound(uint32_t texture) {
  if (CurrentTexture != texture) {
    ImpLogSlow(LogLevel::Trace, LogChannel::Render,
               "Renderer->EnsureTextureBound flushing because texture {:d} is "
               "not {:d}\n",
               CurrentTexture, texture);
    Flush();
    auto res = Device->SetTexture(0, Textures[texture]);
    CurrentTexture = texture;
  }
}

void Renderer::EnsureShader(Shader* shader, bool flush) {
  if (CurrentShader != shader) {
    ImpLogSlow(LogLevel::Debug, LogChannel::Render,
               "Renderer2D changing mode\n");
    if (flush) Flush();
    shader->UseShader(Device);
    CurrentShader = shader;
  }
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->Flush() called before BeginFrame()\n");
    return;
  }

  if (VertexBufferFill > 0 && IndexBufferFill > 0) {
    VOID* pVoid;
    auto res = VertexBufferDevice->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, VertexBuffer, VertexBufferFill);
    res = VertexBufferDevice->Unlock();

    res = IndexBufferDevice->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, IndexBuffer, IndexBufferFill * sizeof(uint16_t));
    res = IndexBufferDevice->Unlock();

    auto result = Device->SetStreamSource(0, VertexBufferDevice, 0,
                                          sizeof(VertexBufferSprites));
    if (FAILED(result)) {
      ImpLog(LogLevel::Debug, LogChannel::Render,
             "Failed to set stream source!\n");
      Window->Shutdown();
    }

    result = Device->SetIndices(IndexBufferDevice);
    if (FAILED(result)) {
      ImpLog(LogLevel::Debug, LogChannel::Render,
             "Failed to set stream indices!\n");
      Window->Shutdown();
    }

    result = Device->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST, 0, 0, IndexBufferFill, 0, IndexBufferFill / 3);
    if (FAILED(result)) {
      ImpLog(LogLevel::Debug, LogChannel::Render, "Failed to draw!\n");
      Window->Shutdown();
    }
  }
  IndexBufferFill = 0;
  VertexBufferFill = 0;
  CurrentTexture = -1;
}

void Renderer::DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                const glm::vec4 tint, const bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVideoTexture() called before BeginFrame()\n");
    return;
  }

  EnsureShader(ShaderYUVFrame);
  CurrentTexture = -1;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  auto err = Device->SetTexture(0, VideoFrameInternal->Luma);
  err = Device->SetTexture(1, VideoFrameInternal->Cb);
  err = Device->SetTexture(2, VideoFrameInternal->Cr);

  // This is cursed man, idk
  BOOL alphaVideoB = (BOOL)alphaVideo;
  Device->SetPixelShaderConstantB(0, &alphaVideoB, 1);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(RectF(0.0f, 0.0f, frame.Width, frame.Height),
            {frame.Width, frame.Height}, &vertices[0].UV,
            sizeof(VertexBufferSprites));
  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::CaptureScreencap(Sprite& sprite) {
  Flush();
  if (sprite.Sheet.Texture != 0) {
    FreeTexture(sprite.Sheet.Texture);
  }

  sprite.Sheet.IsScreenCap = true;
  sprite.Sheet.DesignWidth = static_cast<float>(Window->WindowWidth);
  sprite.Sheet.DesignHeight = static_cast<float>(Window->WindowHeight);
  sprite.Bounds.Width = sprite.Sheet.DesignWidth;
  sprite.Bounds.Height = sprite.Sheet.DesignHeight;

  IDirect3DSurface9* sourceSurface = nullptr;
  Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &sourceSurface);

  IDirect3DTexture9* texture;
  Device->CreateTexture(Window->WindowWidth, Window->WindowHeight, 1,
                        D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                        &texture, nullptr);
  auto id = NextTextureId;
  NextTextureId += 1;
  Textures[id] = texture;
  sprite.Sheet.Texture = id;

  IDirect3DSurface9* destSurface = nullptr;
  texture->GetSurfaceLevel(0, &destSurface);
  Device->StretchRect(sourceSurface, nullptr, destSurface, nullptr,
                      D3DTEXF_NONE);

  sourceSurface->Release();
  destSurface->Release();
}

void Renderer::EnableScissor() {
  Flush();
  Device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
}

void Renderer::SetScissorRect(RectF const& rect) {
  Rect viewport = Window->GetViewport();
  float scale = fmin((float)Window->WindowWidth / Profile::DesignWidth,
                     (float)Window->WindowHeight / Profile::DesignHeight);
  float rectX = rect.X * scale;
  float rectY = rect.Y * scale;
  float rectWidth = rect.Width * scale;
  float rectHeight = rect.Height * scale;

  RECT rectW{};
  rectW.left = (LONG)rectX;
  rectW.top = (LONG)rectY;
  rectW.right = (LONG)(rect.X + rectWidth);
  rectW.bottom = (LONG)(rect.Y + rectHeight);
  Device->SetScissorRect(&rectW);
}

void Renderer::DisableScissor() {
  Flush();
  Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
}

glm::vec2 Renderer::DesignToNDC(glm::vec2 designCoord) const {
  glm::vec2 result;
  result.x = (designCoord.x / (Profile::DesignWidth * 0.5f)) - 1.0f;
  result.y = 1.0f - (designCoord.y / (Profile::DesignHeight * 0.5f));
  return result;
}

void Renderer::SetBlendMode(RendererBlendMode blendMode) {
  Flush();

  switch (blendMode) {
    case RendererBlendMode::Normal:
      Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      break;
    case RendererBlendMode::Additive:
      Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      break;
  }
}

void Renderer::Clear(glm::vec4 color) {
  Flush();

  color *= 255;
  D3DCOLOR clearColor =
      D3DCOLOR_ARGB((BYTE)color.a, (BYTE)color.r, (BYTE)color.g, (BYTE)color.b);
  Device->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
}

}  // namespace DirectX9
}  // namespace Impacto
