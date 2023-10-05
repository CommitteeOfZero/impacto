#include "renderer.h"

#include <SDL_syswm.h>

#include "../../profile/game.h"
#include "../../game.h"
#include "3d/scene.h"
#include "utils.h"

namespace Impacto {
namespace DirectX9 {

void Renderer::InitImpl() {
  if (IsInit) return;
  ImpLog(LL_Info, LC_Render, "Initializing Renderer2D DirectX 9 system\n");
  IsInit = true;
  NuklearSupported = false;

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
      {0, offsetof(VertexBufferSprites, Tint), D3DDECLTYPE_FLOAT4,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
      {0, offsetof(VertexBufferSprites, UV), D3DDECLTYPE_FLOAT2,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
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
  ShaderYUVFrame = new Shader();
  ShaderYUVFrame->Compile("YUVFrame", Device, vertexDeclaration);
  ShaderCCMessageBox = new Shader();
  ShaderCCMessageBox->Compile("CCMessageBoxSprite", Device, vertexDeclaration);

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
}

void Renderer::ShutdownImpl() {
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

void Renderer::NuklearInitImpl() {}

void Renderer::NuklearShutdownImpl() {}

int Renderer::NuklearHandleEventImpl(SDL_Event* ev) { return 0; }

void Renderer::BeginFrameImpl() {
  if (Drawing) {
    ImpLog(LL_Error, LC_Render,
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

void Renderer::BeginFrame2DImpl() {
  Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
  Device->SetRenderState(D3DRS_ZWRITEENABLE, 0);
  Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void Renderer::EndFrameImpl() {
  if (!Drawing) return;
  Flush();
  Drawing = false;
  Device->EndScene();
  Device->Present(NULL, NULL, NULL, NULL);
  CurrentShader = 0;
  CurrentTexture = -1;
}

uint32_t Renderer::SubmitTextureImpl(TexFmt format, uint8_t* buffer, int width,
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
    ImpLog(LL_Error, LC_Render, "Failed to load texture!\n");
    Window->Shutdown();
  }

  auto id = NextTextureId;
  NextTextureId += 1;
  Textures[id] = texture;

  return id;
}

void Renderer::FreeTextureImpl(uint32_t id) {}

YUVFrame* Renderer::CreateYUVFrameImpl(int width, int height) {
  VideoFrameInternal = new DX9YUVFrame(Device);
  VideoFrameInternal->Init(width, height);
  return (YUVFrame*)VideoFrameInternal;
}

void Renderer::DrawRectImpl(RectF const& dest, glm::vec4 color, float angle) {
  DrawSprite(RectSprite, dest, color, angle);
}

void Renderer::DrawSprite3DRotatedImpl(Sprite const& sprite, RectF const& dest,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, bool inverted) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawSprite3DRotated() called before BeginFrame()\n");
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

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition3DRotated(dest, depth, vanishingPoint, stayInScreen, rot,
                           (uintptr_t)&vertices[0].Position,
                           sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawRect3DRotatedImpl(RectF const& dest, float depth,
                                     glm::vec2 vanishingPoint,
                                     bool stayInScreen, glm::quat rot,
                                     glm::vec4 color) {
  DrawSprite3DRotated(RectSprite, dest, depth, vanishingPoint, stayInScreen,
                      rot, color);
}

void Renderer::DrawCharacterMvlImpl(Sprite const& sprite, glm::vec2 topLeft,
                                    int verticesCount, float* mvlVertices,
                                    int indicesCount, uint16_t* mvlIndices,
                                    bool inverted, glm::vec4 tint) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCharacterMvl() called before BeginFrame()\n");
    return;
  }

  // Draw just the character with this since we need to rebind the index buffer
  // anyway...
  Flush();

  // Are we in sprite mode?
  if (inverted)
    EnsureShader(ShaderSpriteInverted);
  else
    EnsureShader(ShaderSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += verticesCount * sizeof(VertexBufferSprites);

  IndexBufferFill += indicesCount;

  memcpy(IndexBuffer, mvlIndices, indicesCount * sizeof(mvlIndices[0]));

  for (int i = 0; i < verticesCount; i++) {
    vertices[i].Position = DesignToNDC(glm::vec2(
        mvlVertices[i * 5] + topLeft.x, mvlVertices[i * 5 + 1] + topLeft.y));
    vertices[i].UV = glm::vec2(mvlVertices[i * 5 + 3], mvlVertices[i * 5 + 4]);
    vertices[i].Tint = tint;
  }

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

void Renderer::DrawSpriteImpl(Sprite const& sprite, RectF const& dest,
                              glm::vec4 tint, float angle, bool inverted,
                              bool isScreencap) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
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

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, bool isScreencap,
                                    bool isInverted, bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawMaskedSprite() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  Flush();
  EnsureShader(ShaderMaskedSprite);

  Device->SetTexture(0, Textures[sprite.Sheet.Texture]);
  Device->SetTexture(1, Textures[mask.Sheet.Texture]);

  // This is cursed man, idk
  float alphaRes[] = {alphaRange, constAlpha};
  BOOL isInvertedB = (BOOL)isInverted;
  BOOL isSameTextureB = (BOOL)isSameTexture;
  Device->SetPixelShaderConstantF(0, alphaRes, 1);
  Device->SetPixelShaderConstantB(0, &isInvertedB, 1);
  Device->SetPixelShaderConstantB(1, &isSameTextureB, 1);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(sprite.Bounds, sprite.Bounds.Width, sprite.Bounds.Height,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    bool isScreencap) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
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

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(mask.Bounds, mask.Sheet.DesignWidth, mask.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

inline void Renderer::QuadSetUVFlipped(RectF const& spriteBounds,
                                       float designWidth, float designHeight,
                                       uintptr_t uvs, int stride) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  // top-left
  *(glm::vec2*)(uvs + 0 * stride) = glm::vec2(leftUV, topUV);
  // bottom-left
  *(glm::vec2*)(uvs + 1 * stride) = glm::vec2(leftUV, bottomUV);
  // bottom-right
  *(glm::vec2*)(uvs + 2 * stride) = glm::vec2(rightUV, bottomUV);
  // top-right
  *(glm::vec2*)(uvs + 3 * stride) = glm::vec2(rightUV, topUV);
}

inline void Renderer::QuadSetUV(RectF const& spriteBounds, float designWidth,
                                float designHeight, uintptr_t uvs, int stride) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  // bottom-left
  *(glm::vec2*)(uvs + 0 * stride) = glm::vec2(leftUV, bottomUV);
  // top-left
  *(glm::vec2*)(uvs + 1 * stride) = glm::vec2(leftUV, topUV);
  // top-right
  *(glm::vec2*)(uvs + 2 * stride) = glm::vec2(rightUV, topUV);
  // bottom-right
  *(glm::vec2*)(uvs + 3 * stride) = glm::vec2(rightUV, bottomUV);
}

inline void Renderer::QuadSetPosition(RectF const& transformedQuad, float angle,
                                      uintptr_t positions, int stride) {
  glm::vec2 bottomLeft =
      glm::vec2(transformedQuad.X, transformedQuad.Y + transformedQuad.Height);
  glm::vec2 topLeft = glm::vec2(transformedQuad.X, transformedQuad.Y);
  glm::vec2 topRight =
      glm::vec2(transformedQuad.X + transformedQuad.Width, transformedQuad.Y);
  glm::vec2 bottomRight = glm::vec2(transformedQuad.X + transformedQuad.Width,
                                    transformedQuad.Y + transformedQuad.Height);

  if (angle != 0.0f) {
    glm::vec2 center = transformedQuad.Center();
    glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * (bottomLeft - center) + center;
    topLeft = rot * (topLeft - center) + center;
    topRight = rot * (topRight - center) + center;
    bottomRight = rot * (bottomRight - center) + center;
  }

  // bottom-left
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDC(bottomLeft);
  // top-left
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDC(topLeft);
  // top-right
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDC(topRight);
  // bottom-right
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDC(bottomRight);
}

void Renderer::QuadSetPosition3DRotated(RectF const& transformedQuad,
                                        float depth, glm::vec2 vanishingPoint,
                                        bool stayInScreen, glm::quat rot,
                                        uintptr_t positions, int stride) {
  float widthNormalized = transformedQuad.Width / (Profile::DesignWidth * 0.5f);
  float heightNormalized =
      transformedQuad.Height / (Profile::DesignHeight * 0.5f);

  glm::vec4 corners[4]{
      // bottom-left
      {glm::vec2(-widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1},
      // top-left
      {glm::vec2(-widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // top-right
      {glm::vec2(widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // bottom-right
      {glm::vec2(widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1}};

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f),
                     glm::vec3(DesignToNDC(transformedQuad.Center()), 0)) *
      glm::mat4_cast(rot);

  glm::vec4 vanishingPointNDC(DesignToNDC(vanishingPoint), 0, 0);

  for (int i = 0; i < 4; i++) {
    corners[i] = transform * corners[i];
  }

  if (stayInScreen) {
    float maxZ = 0.0f;
    for (int i = 0; i < 4; i++) {
      if (corners[i].z > maxZ) maxZ = corners[i].z;
    }
    for (int i = 0; i < 4; i++) {
      corners[i].z -= maxZ;
    }
  }

  for (int i = 0; i < 4; i++) {
    // perspective
    corners[i] -= vanishingPointNDC;
    corners[i].x *= (depth / (depth - corners[i].z));
    corners[i].y *= (depth / (depth - corners[i].z));
    corners[i] += vanishingPointNDC;

    *(glm::vec2*)(positions + i * stride) = corners[i];
  }
}

void Renderer::EnsureSpaceAvailable(int vertices, int vertexSize, int indices) {
  if (VertexBufferFill + vertices * vertexSize > VertexBufferSize ||
      IndexBufferFill + indices > IndexBufferCount) {
    ImpLogSlow(
        LL_Trace, LC_Render,
        "Renderer->EnsureSpaceAvailable flushing because buffers full at "
        "VertexBufferFill=%08X,IndexBufferFill=%08X\n",
        VertexBufferFill, IndexBufferFill);
    Flush();
  }
}

void Renderer::EnsureTextureBound(uint32_t texture) {
  if (CurrentTexture != texture) {
    ImpLogSlow(LL_Trace, LC_Render,
               "Renderer->EnsureTextureBound flushing because texture %d is "
               "not %d\n",
               CurrentTexture, texture);
    Flush();
    auto res = Device->SetTexture(0, Textures[texture]);
    CurrentTexture = texture;
  }
}

void Renderer::EnsureShader(Shader* shader, bool flush) {
  if (CurrentShader != shader) {
    ImpLogSlow(LL_Debug, LC_Render, "Renderer2D changing mode\n");
    if (flush) Flush();
    shader->UseShader(Device);
    CurrentShader = shader;
  }
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
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
      ImpLog(LL_Debug, LC_Render, "Failed to set stream source!\n");
      Window->Shutdown();
    }

    result = Device->SetIndices(IndexBufferDevice);
    if (FAILED(result)) {
      ImpLog(LL_Debug, LC_Render, "Failed to set stream indices!\n");
      Window->Shutdown();
    }

    result = Device->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST, 0, 0, IndexBufferFill, 0, IndexBufferFill / 3);
    if (FAILED(result)) {
      ImpLog(LL_Debug, LC_Render, "Failed to draw!\n");
      Window->Shutdown();
    }
  }
  IndexBufferFill = 0;
  VertexBufferFill = 0;
}

void Renderer::DrawVideoTextureImpl(YUVFrame* tex, RectF const& dest,
                                    glm::vec4 tint, float angle,
                                    bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
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

  QuadSetUV(RectF(0.0f, 0.0f, tex->Width, tex->Height), tex->Width, tex->Height,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::CaptureScreencapImpl(Sprite const& sprite) { Flush(); }

void Renderer::EnableScissorImpl() {
  Flush();
  Device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
}

void Renderer::SetScissorRectImpl(RectF const& rect) {
  RECT rectW{};
  rectW.left = rect.X;
  rectW.top = rect.Y;
  rectW.right = rect.X + rect.Width;
  rectW.bottom = rect.Y + rect.Height;
  Device->SetScissorRect(&rectW);
}

void Renderer::DisableScissorImpl() {
  Flush();
  Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
}

void Renderer::DrawCHLCCMenuBackgroundImpl(const Sprite& sprite,
                                           const Sprite& mask,
                                           const RectF& dest, float alpha) {
  return;
}

}  // namespace DirectX9
}  // namespace Impacto
