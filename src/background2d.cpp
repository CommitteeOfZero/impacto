#include "background2d.h"

#include <numeric>

#include "mask2d.h"
#include "mem.h"
#include "io/memorystream.h"
#include "io/vfs.h"
#include "util.h"
#include "profile/game.h"
#include "profile/scriptvars.h"
#include "profile/vm.h"
// #include "window.h"
#include "renderer/renderer.h"

namespace Impacto {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

void Background2D::Init() {
  for (int i = 0; i < MaxBackgrounds2D; i++) {
    Backgrounds2D[i] = &Backgrounds[i];
  }

  for (int i = 0; i < MaxScreencaptures; i++) {
    Screencaptures[i].BgSprite.Sheet.IsScreenCap = true;
    Screencaptures[i].LoadSolidColor(0xFF000000, Window->WindowWidth,
                                     Window->WindowHeight);
    Screencaptures[i].Status = LS_Loaded;
  }

  ShaderScreencapture.BgSprite.Sheet.IsScreenCap = true;

  for (size_t i = 0; i < Framebuffers.max_size(); i++) {
    Framebuffers[i].BgSprite =
        Sprite(SpriteSheet(Profile::DesignWidth, Profile::DesignHeight), 0.0f,
               0.0f, Profile::DesignWidth, Profile::DesignHeight);
    Framebuffers[i].BgSprite.Sheet.Texture =
        Renderer->GetFramebufferTexture(i + 1);

    Framebuffers[i].Status = LS_Loaded;
    Framebuffers[i].BgSprite.Sheet.IsScreenCap = true;
  }

  ShaderScreencapture.LoadSolidColor(0xFF000000, Window->WindowWidth,
                                     Window->WindowHeight);
  ShaderScreencapture.Status = LS_Loaded;
}

bool Background2D::LoadSync(uint32_t bgId) {
  if (bgId & 0xFF000000) {
    BgTexture.Load1x1((bgId >> 16) & 0xFF, (bgId >> 8) & 0xFF, bgId & 0xFF,
                      0xFF);
  } else {
    Io::Stream* stream;
    int64_t err = Io::VfsOpen("bg", bgId, &stream);
    if (err != IoError_OK) return false;
    BgTexture.Load(stream);
    delete stream;
  }
  return true;
}

void Background2D::LoadSolidColor(uint32_t color, int width, int height) {
  BgTexture.LoadSolidColor(width, height, color);
  MainThreadOnLoad();
}

void Background2D::UnloadSync() {
  Renderer->FreeTexture(BgSprite.Sheet.Texture);
  BgSprite.Sheet.DesignHeight = 0.0f;
  BgSprite.Sheet.DesignWidth = 0.0f;
  BgSprite.Sheet.Texture = 0;
  BgSprite.Sheet.IsScreenCap = false;
  Show = false;
  std::fill(Layers.begin(), Layers.end(), -1);
}

void Background2D::MainThreadOnLoad() {
  BgSprite.Sheet.Texture = BgTexture.Submit();
  if ((BgTexture.Width == 1) && (BgTexture.Height == 1)) {
    BgSprite.Sheet.DesignWidth = Profile::DesignWidth;
    BgSprite.Sheet.DesignHeight = Profile::DesignHeight;
  } else {
    BgSprite.Sheet.DesignWidth = (float)BgTexture.Width;
    BgSprite.Sheet.DesignHeight = (float)BgTexture.Height;
  }
  BgSprite.BaseScale = glm::vec2(1.0f);
  BgSprite.Bounds = RectF(0.0f, 0.0f, BgSprite.Sheet.DesignWidth,
                          BgSprite.Sheet.DesignHeight);
  Show = false;
  std::fill(Layers.begin(), Layers.end(), -1);
}

void Background2D::Render(int bgId, int layer) {
  if (Status != LS_Loaded || !OnLayer(layer) || !Show) return;

  MaskNumber = ScrWork[SW_BG1MASKNO + ScrWorkBgStructSize * bgId];
  FadeCount = ScrWork[SW_BG1FADECT + ScrWorkBgStructSize * bgId];
  FadeRange = ScrWork[SW_BG1MASKFADERANGE + ScrWorkBgStructSize * bgId];

  // Set tint
  switch (GameInstructionSet) {
    case Vm::InstructionSet::CC:
      Tint = ScrWorkGetColor(SW_BG1FILTER + ScrWorkBgStructSize * bgId);
      break;

    default:
      Tint = glm::vec4(1.0f);
      break;
  }
  Tint.a = (ScrWork[SW_BG1ALPHA + ScrWorkBgStructSize * bgId] +
            ScrWork[SW_BG1ALPHA_OFS + ScrWorkBgOffsetStructSize * bgId]) /
           256.0f;

  switch (GameInstructionSet) {
    case Vm::InstructionSet::Dash:
      Tint.a = ScrWork[SW_BG1ALPHA + ScrWorkBgStructSize * bgId] / 256.0f;
      break;

    case Vm::InstructionSet::CC:
      if (ScrWork[SW_BGEFF1_MODE + ScrWorkBgEffStructSize * bgId] == 1) {
        Tint.a =
            ScrWork[SW_BGEFF1_ALPHA + ScrWorkBgEffStructSize * bgId] / 256.0f;
      }
      break;

    default:
      break;
  }

  const int renderType = ScrWork[SW_BG1FADETYPE + ScrWorkBgStructSize * bgId];
  std::invoke(BackgroundRenderTable[renderType], this);
}

void Background2D::RenderCapture(int capId, int layer) {
  if (Status != LS_Loaded || !OnLayer(layer) || !Show) return;

  MaskNumber = ScrWork[SW_CAP1MASKNO + ScrWorkCaptureStructSize * capId];
  FadeCount = ScrWork[SW_CAP1FADECT + ScrWorkCaptureStructSize * capId];
  FadeRange = ScrWork[SW_CAP1MASKFADERANGE + ScrWorkCaptureStructSize * capId];

  // Set tint
  switch (GameInstructionSet) {
    case Vm::InstructionSet::CC:
      Tint = ScrWorkGetColor(SW_CAP1FILTER + ScrWorkCaptureStructSize * capId);
      break;

    default:
      Tint = glm::vec4(1.0f);
      break;
  }
  Tint.a =
      (ScrWork[SW_CAP1ALPHA + ScrWorkCaptureStructSize * capId] +
       ScrWork[SW_CAP1ALPHA_OFS + ScrWorkCaptureOffsetStructSize * capId]) /
      256.0f;

  const int renderType =
      ScrWork[SW_CAP1FADETYPE + ScrWorkCaptureStructSize * capId];
  std::invoke(BackgroundRenderTable[renderType], this);
}

void Background2D::RenderBgEff(int bgId, int layer) {
  if (Status != LS_Loaded) return;

  const int structOffset = ScrWorkBgEffStructSize * bgId;
  const int structOfsOffset = ScrWorkBgEffOffsetStructSize * bgId;

  MaskNumber = ScrWork[SW_BGEFF1_MASKNO + structOffset];
  FadeCount = ScrWork[SW_BGEFF1_FADECT + structOffset];
  FadeRange = ScrWork[SW_BGEFF1_MASKFADERANGE + structOffset];

  Rotation = ScrWorkAnglesToQuaternion(
      ScrWork[SW_BGEFF1_ROTX + structOffset] +
          ScrWork[SW_BGEFF1_ROTX_OFS + structOfsOffset],
      ScrWork[SW_BGEFF1_ROTY + structOffset] +
          ScrWork[SW_BGEFF1_ROTY_OFS + structOfsOffset],
      ScrWork[SW_BGEFF1_ROTZ + structOffset] +
          ScrWork[SW_BGEFF1_ROTZ_OFS + structOfsOffset]);

  Scale = glm::vec2((ScrWork[SW_BGEFF1_SIZE + structOffset] +
                     ScrWork[SW_BGEFF1_SIZE_OFS + structOfsOffset]) /
                    1000.0f);

  // Set tint
  switch (GameInstructionSet) {
    case Vm::InstructionSet::CC:
      Tint = ScrWorkGetColor(SW_BGEFF1_FILTER + structOffset);
      break;

    default:
      Tint = glm::vec4(1.0f);
      break;
  }
  Tint.a = (ScrWork[SW_BGEFF1_ALPHA + structOffset] +
            ScrWork[SW_BGEFF1_ALPHA_OFS + structOfsOffset]) /
           256.0f;

  // Get position coordinates
  const int maskType = ScrWork[SW_BGEFF1_MASK_TYPE + structOffset];
  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  const size_t vertexCount = maskType == 0 ? 4 : 3;
  std::array<glm::vec2, 4> vertices;
  for (size_t i = 0; i < vertexCount; i++) {
    const int x =
        ScrWork[SW_BGEFF1_MASK_VERTEX1_X + structOffset + i * 2] +
        ScrWork[SW_BGEFF1_MASK_VERTEX1_OFSX + structOfsOffset + i * 2];
    const int y =
        ScrWork[SW_BGEFF1_MASK_VERTEX1_Y + structOffset + i * 2] +
        ScrWork[SW_BGEFF1_MASK_VERTEX1_OFSY + structOfsOffset + i * 2];
    vertices[i] = glm::vec2((float)x, (float)y) * resolutionScale;
  }
  if (vertexCount == 4) std::swap(vertices[1], vertices[2]);

  const glm::vec2 pos =
      glm::vec2(ScrWork[SW_BGEFF1_POSX + structOffset] +
                    ScrWork[SW_BGEFF1_OFSX + structOfsOffset],
                ScrWork[SW_BGEFF1_POSY + structOffset] +
                    ScrWork[SW_BGEFF1_OFSY + structOfsOffset]) *
      resolutionScale;

  DisplayCoords = pos - vertices[0];

  // Origin is the center of mass
  Origin = std::reduce(vertices.begin(), vertices.begin() + vertexCount) /
           (float)vertexCount;

  // Transform vertices
  const glm::mat4 stencilTransformation = TransformationMatrix(
      Origin, Scale, {Origin, 0.0f}, Rotation, DisplayCoords);

  // Draw
  Renderer->SetStencilMode(StencilBufferMode::Write);
  Renderer->ClearStencilBuffer();

  Renderer->DrawConvexShape(
      std::span(vertices.begin(), vertices.begin() + vertexCount),
      stencilTransformation, glm::vec4(1.0f));

  Renderer->SetStencilMode(StencilBufferMode::Test);

  const int renderType = ScrWork[SW_BGEFF1_MODE + structOffset];
  std::invoke(BackgroundRenderTable[renderType], this);

  Renderer->SetStencilMode(StencilBufferMode::Off);
}

void Background2D::RenderRegular() {
  const glm::mat4 transformation = TransformationMatrix(
      Origin, Scale, {Origin, 0.0f}, Rotation, DisplayCoords);
  Renderer->DrawSprite(BgSprite, transformation, Tint, false);

  for (int i = 0; i < MaxLinkedBgBuffers; i++) {
    if (Links[i].Direction != LD_Off && Links[i].LinkedBuffer != NULL) {
      const glm::mat4 linkTransformation =
          transformation *
          glm::translate(glm::mat4(1.0f), {Links[i].DisplayCoords, 0.0f});

      Renderer->DrawSprite(Links[i].LinkedBuffer->BgSprite, transformation,
                           Tint, false);
    }
  }
}

void Background2D::RenderMasked() {
  const glm::mat4 transformation = TransformationMatrix(
      Origin, Scale, {Origin, 0.0f}, Rotation, DisplayCoords);

  Renderer->DrawMaskedSprite(BgSprite, Masks2D[MaskNumber].MaskSprite,
                             FadeCount, FadeRange, transformation, Tint, false,
                             false);
}

void Background2D::RenderMaskedInverted() {
  const glm::mat4 transformation = TransformationMatrix(
      Origin, Scale, {Origin, 0.0f}, Rotation, DisplayCoords);
  Renderer->DrawMaskedSprite(BgSprite, Masks2D[MaskNumber].MaskSprite,
                             FadeCount, FadeRange, transformation, Tint, true,
                             false);
}

void Background2D::RenderFade() {
  Tint.a *= FadeCount / 256.0f;

  RenderRegular();
}

}  // namespace Impacto