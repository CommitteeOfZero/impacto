#include "background2d.h"

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

  glm::vec4 col = glm::vec4(255.0f);
  col.a = ScrWork[SW_BG1ALPHA + ScrWorkBgStructSize * bgId] +
          ScrWork[SW_BG1ALPHA_OFS + 10 * bgId];
  if (GameInstructionSet == +Vm::InstructionSet::CC) {
    const uint32_t rgb = ScrWork[SW_BG1FILTER + ScrWorkBgStructSize * bgId];
    col = glm::vec4(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff, col.a);
  }
  col /= glm::vec4(255.0f, 255.0f, 255.0f, 256.0f);

  switch (GameInstructionSet) {
    case Vm::InstructionSet::Dash:
      col.a = ScrWork[SW_BG1ALPHA + ScrWorkBgStructSize * bgId] / 256.0f;
      break;

    case Vm::InstructionSet::CC:
      if (ScrWork[SW_BGEFF1_MODE + ScrWorkBgEffStructSize * bgId] == 1) {
        col.a =
            ScrWork[SW_BGEFF1_ALPHA + ScrWorkBgEffStructSize * bgId] / 256.0f;
      }
      break;

    default:
      break;
  }

  const int renderType = ScrWork[SW_BG1FADETYPE + ScrWorkBgStructSize * bgId];
  std::invoke(BackgroundRenderTable[renderType], this, col);
}

void Background2D::RenderCapture(int capId, int layer) {
  if (Status != LS_Loaded || !OnLayer(layer) || !Show) return;

  MaskNumber = ScrWork[SW_CAP1MASKNO + ScrWorkCaptureStructSize * capId];
  FadeCount = ScrWork[SW_CAP1FADECT + ScrWorkCaptureStructSize * capId];
  FadeRange = ScrWork[SW_CAP1MASKFADERANGE + ScrWorkCaptureStructSize * capId];

  glm::vec4 col = glm::vec4(255.0f);
  col.a = ScrWork[SW_CAP1ALPHA + ScrWorkCaptureStructSize * capId] +
          ScrWork[SW_CAP1ALPHA_OFS + 10 * capId];
  if (GameInstructionSet == +Vm::InstructionSet::CC) {
    const uint32_t rgb =
        ScrWork[SW_CAP1FILTER + ScrWorkCaptureStructSize * capId];
    col = glm::vec4(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff, col.a);
  }
  col /= glm::vec4(255.0f, 255.0f, 255.0f, 256.0f);

  const int renderType =
      ScrWork[SW_CAP1FADETYPE + ScrWorkCaptureStructSize * capId];
  std::invoke(BackgroundRenderTable[renderType], this, col);
}

void Background2D::RenderBgEff(int bgId, int layer) {
  if (Status != LS_Loaded) return;

  const int structOffset = ScrWorkBgEffStructSize * bgId;
  const int structOfsOffset = 20 * bgId;

  MaskNumber = ScrWork[SW_BGEFF1_MASKNO + structOffset];
  FadeCount = ScrWork[SW_BGEFF1_FADECT + structOffset];
  FadeRange = ScrWork[SW_BGEFF1_MASKFADERANGE + structOffset];

  Angle = (ScrWork[SW_BGEFF1_ROTZ + structOffset] +
           ScrWork[SW_BGEFF1_ROTZ_OFS + structOfsOffset]) *
          (float)(2.0f * M_PI / (float)(1 << 16));

  Scale = glm::vec2((ScrWork[SW_BGEFF1_SIZE + structOffset] +
                     ScrWork[SW_BGEFF1_SIZE_OFS + structOfsOffset]) /
                    1000.0f);

  // Set tint
  glm::vec4 col = glm::vec4(255.0f);
  col.a = ScrWork[SW_BGEFF1_ALPHA + structOffset] +
          ScrWork[SW_BGEFF1_ALPHA_OFS + structOfsOffset];
  if (GameInstructionSet == +Vm::InstructionSet::CC) {
    const uint32_t rgb = ScrWork[SW_BGEFF1_FILTER + structOffset];
    col = glm::vec4(rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff, col.a);
  }
  col /= glm::vec4(255.0f, 255.0f, 255.0f, 256.0f);

  // Get position coordinates
  const int maskType = ScrWork[SW_BGEFF1_MASK_TYPE + structOffset];
  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  const size_t vertexCount = maskType == 0 ? 4 : 3;
  std::vector<glm::vec2> vertices;
  vertices.reserve(vertexCount);
  for (size_t i = 0; i < vertexCount; i++) {
    const int x =
        ScrWork[SW_BGEFF1_MASK_VERTEX1_X + structOffset + i * 2] +
        ScrWork[SW_BGEFF1_MASK_VERTEX1_OFSX + structOfsOffset + i * 2];
    const int y =
        ScrWork[SW_BGEFF1_MASK_VERTEX1_Y + structOffset + i * 2] +
        ScrWork[SW_BGEFF1_MASK_VERTEX1_OFSY + structOfsOffset + i * 2];
    vertices.emplace_back(glm::vec2(x, y) * resolutionScale);
  }

  const glm::vec2 pos =
      glm::vec2(ScrWork[SW_BGEFF1_POSX + structOffset] +
                    ScrWork[SW_BGEFF1_OFSX + structOfsOffset],
                ScrWork[SW_BGEFF1_POSY + structOffset] +
                    ScrWork[SW_BGEFF1_OFSY + structOfsOffset]) *
      resolutionScale;

  DisplayCoords = pos - vertices[0];

  // Origin is the center of mass
  Origin = {0.0f, 0.0f};
  for (const glm::vec2 vertex : vertices) Origin += vertex / (float)vertexCount;

  // Draw
  if (maskType == 0) {  // Rectangle
    const glm::vec2 maskDimensions = vertices[3] - vertices[0];
    const RectF mask = RectF(pos.x, pos.y, maskDimensions.x, maskDimensions.y)
                           .Scale(Scale, vertices[0]);

    Renderer->EnableScissor();
    Renderer->SetScissorRect(mask);
  }

  const int renderType = ScrWork[SW_BGEFF1_MODE + structOffset];
  std::invoke(BackgroundRenderTable[renderType], this, col);

  if (maskType == 0) {  // Rectangle
    Renderer->DisableScissor();
  }
}

void Background2D::RenderRegular(glm::vec4 col) {
  Renderer->DrawSpriteOffset(BgSprite, DisplayCoords, Origin, col, Scale, Angle,
                             false);

  for (int i = 0; i < MaxLinkedBgBuffers; i++) {
    if (Links[i].Direction != LD_Off && Links[i].LinkedBuffer != NULL) {
      Renderer->DrawSprite(
          Links[i].LinkedBuffer->BgSprite,
          RectF(Links[i].DisplayCoords.x, Links[i].DisplayCoords.y,
                Links[i].LinkedBuffer->BgSprite.ScaledWidth(),
                Links[i].LinkedBuffer->BgSprite.ScaledHeight()),
          col, 0.0f, false);
    }
  }
}

void Background2D::RenderMasked(glm::vec4 col) {
  Renderer->DrawMaskedSprite(
      BgSprite, Masks2D[MaskNumber].MaskSprite,
      RectF(DisplayCoords.x, DisplayCoords.y, BgSprite.ScaledWidth(),
            BgSprite.ScaledHeight()),
      col, FadeCount, FadeRange);
}

void Background2D::RenderMaskedInverted(glm::vec4 col) {
  Renderer->DrawMaskedSprite(
      BgSprite, Masks2D[MaskNumber].MaskSprite,
      RectF(DisplayCoords.x, DisplayCoords.y, BgSprite.ScaledWidth(),
            BgSprite.ScaledHeight()),
      col, FadeCount, FadeRange, true);
}

void Background2D::RenderFade(glm::vec4 col) {
  col.a *= FadeCount / 256.0f;

  Renderer->DrawSprite(BgSprite,
                       RectF(DisplayCoords.x, DisplayCoords.y,
                             BgSprite.ScaledWidth(), BgSprite.ScaledHeight()),
                       col, 0.0f, false);

  for (int i = 0; i < MaxLinkedBgBuffers; i++) {
    if (Links[i].Direction != LD_Off && Links[i].LinkedBuffer != NULL) {
      Renderer->DrawSprite(
          Links[i].LinkedBuffer->BgSprite,
          RectF(Links[i].DisplayCoords.x, Links[i].DisplayCoords.y,
                Links[i].LinkedBuffer->BgSprite.ScaledWidth(),
                Links[i].LinkedBuffer->BgSprite.ScaledHeight()),
          col, Angle, false);
    }
  }
}

}  // namespace Impacto