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
#include "vm/interface/scene2d.h"

namespace Impacto {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

void Background2D::InitFrameBuffers() {
  RectF viewPort = Window->GetViewport();
  for (size_t i = 0; i < Framebuffers.max_size(); i++) {
    Framebuffers[i].BgSprite =
        Sprite(SpriteSheet(Profile::DesignWidth, Profile::DesignHeight), 0.0f,
               0.0f, Profile::DesignWidth, Profile::DesignHeight);
    Framebuffers[i].BgSprite.Sheet.Texture =
        Renderer->GetFramebufferTexture(i + 1);

    Framebuffers[i].Status = LoadStatus::Loaded;
    Framebuffers[i].BgSprite.Sheet.IsScreenCap = true;
  }
}

void Background2D::Init() {
  for (int i = 0; i < Backgrounds.size(); i++) {
    Backgrounds2D[i] = &Backgrounds[i];
  }

  for (Capture2D& capture : Screencaptures) {
    capture.BgSprite.Sheet.IsScreenCap = true;
    capture.LoadSolidColor(0xFF000000, Window->WindowWidth,
                           Window->WindowHeight);
    capture.Status = LoadStatus::Loaded;
  }

  ShaderScreencapture.BgSprite.Sheet.IsScreenCap = true;
  InitFrameBuffers();

  for (size_t i = 0; i < Framebuffers.max_size(); i++) {
    Framebuffers[i].BgSprite =
        Sprite(SpriteSheet(Profile::DesignWidth, Profile::DesignHeight), 0.0f,
               0.0f, Profile::DesignWidth, Profile::DesignHeight);
    Framebuffers[i].BgSprite.Sheet.Texture =
        Renderer->GetFramebufferTexture(i + 1);

    Framebuffers[i].Status = LoadStatus::Loaded;
    Framebuffers[i].BgSprite.Sheet.IsScreenCap = true;
  }

  ShaderScreencapture.LoadSolidColor(0xFF000000, Window->WindowWidth,
                                     Window->WindowHeight);
  ShaderScreencapture.Status = LoadStatus::Loaded;
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

    if (Profile::UseBgEffects) {
      Io::FileMeta meta;
      err = Io::VfsGetMeta("bg", bgId, &meta);
      if (err != IoError_OK) return false;

      const std::string baseName =
          meta.FileName.substr(0, meta.FileName.find('.'));

      LoadedBgEffCount = 0;
      for (size_t i = 0; i < BgEffTextures.size(); i++) {
        const std::string filename = fmt::format("FT_{}{}.png", baseName, i);

        Io::Stream* bgEffStream;
        int64_t err = Io::VfsOpen("bgeffect", filename, &bgEffStream);

        if (err == IoError_NotFound) {
          // No more background effects
          break;
        } else if (err != IoError::IoError_OK) {
          return false;
        }

        BgEffTextures[i].Load(bgEffStream);
        LoadedBgEffCount++;

        delete bgEffStream;
      }

      BgEffChaLoaded = false;
      const std::string filename = fmt::format("FT_{}CH.png", baseName);

      Io::Stream* bgEffStream;
      int64_t err = Io::VfsOpen("bgeffect", filename, &bgEffStream);

      if (err != IoError_NotFound) {
        if (err != IoError_OK) return false;

        BgEffChaTexture.Load(bgEffStream);
        BgEffChaLoaded = true;
        delete bgEffStream;
      }
    }
  }

  return true;
}

void Background2D::LoadSolidColor(uint32_t color, int width, int height) {
  BgTexture.LoadSolidColor(width, height, color);
  MainThreadOnLoad(true);
}

void Background2D::UnloadSync() {
  Renderer->FreeTexture(BgSprite.Sheet.Texture);
  BgSprite.Sheet.DesignHeight = 0.0f;
  BgSprite.Sheet.DesignWidth = 0.0f;
  BgSprite.Sheet.Texture = 0;
  BgSprite.Sheet.IsScreenCap = false;

  LoadedBgEffCount = 0;
  for (Sprite& bgEff : BgEffSprites) {
    Renderer->FreeTexture(bgEff.Sheet.Texture);
    bgEff.Sheet.DesignHeight = 0.0f;
    bgEff.Sheet.DesignWidth = 0.0f;
    bgEff.Sheet.Texture = 0;
  }

  BgEffChaLoaded = false;
  Renderer->FreeTexture(BgEffChaSprite.Sheet.Texture);
  BgEffChaSprite.Sheet.DesignHeight = 0.0f;
  BgEffChaSprite.Sheet.DesignWidth = 0.0f;
  BgEffChaSprite.Sheet.Texture = 0;

  Show = false;

  std::fill(Layers.begin(), Layers.end(), -1);
}

void Background2D::MainThreadOnLoad(bool result) {
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

  if (Profile::UseBgEffects) {
    for (size_t i = 0; i < LoadedBgEffCount; i++) {
      BgEffSprites[i].Sheet.Texture = BgEffTextures[i].Submit();

      BgEffSprites[i].Sheet.DesignWidth = (float)BgEffTextures[i].Width;
      BgEffSprites[i].Sheet.DesignHeight = (float)BgEffTextures[i].Height;

      BgEffSprites[i].Bounds =
          RectF(0.0f, 0.0f, BgEffSprites[i].Sheet.DesignWidth,
                BgEffSprites[i].Sheet.DesignHeight);
    }

    if (BgEffChaLoaded) {
      BgEffChaSprite.Sheet.Texture = BgEffChaTexture.Submit();

      BgEffChaSprite.Sheet.DesignWidth = (float)BgEffChaTexture.Width;
      BgEffChaSprite.Sheet.DesignHeight = (float)BgEffChaTexture.Height;

      BgEffChaSprite.Bounds =
          RectF(0.0f, 0.0f, BgEffChaSprite.Sheet.DesignWidth,
                BgEffChaSprite.Sheet.DesignHeight);
    }
  }

  Show = false;

  std::fill(Layers.begin(), Layers.end(), -1);
}

void Background2D::LinkBuffers(const int linkCode, const int currentBufferId) {
  const int srcBufId = (linkCode >> 8) & 0xFF;
  if (srcBufId == Vm::Interface::GetScriptBufferId(currentBufferId)) {
    const LinkDirection dir = (LinkDirection)((linkCode >> 16) & 0xFF);

    for (size_t i = 0; i < MaxLinkedBgBuffers; i++) {
      int childBufId = (linkCode >> (i * 24)) & 0xFF;
      if (childBufId != 0) {
        childBufId = Vm::Interface::GetBufferId(childBufId);
        Background2D* const childBuf =
            Backgrounds2D[ScrWork[SW_BG1SURF + childBufId]];

        childBuf->BgSprite.BaseScale = BgSprite.BaseScale;

        Links[i].LinkedBuffer = childBuf;
        Links[i].Direction = dir;

        switch (dir) {
          case LinkDirection::Up3: {
            float offset =
                i == 0 ? 0.0f
                       : Links[i - 1].LinkedBuffer->BgSprite.ScaledHeight();
            Links[i].DisplayCoords = {
                Position.x, Position.y - BgSprite.ScaledHeight() - offset};
          } break;

          case LinkDirection::Up: {
            Links[i].DisplayCoords = {Position.x,
                                      Position.y - BgSprite.ScaledHeight()};
          } break;

          case LinkDirection::Down3: {
            float offset =
                i == 0 ? 0.0f
                       : Links[i - 1].LinkedBuffer->BgSprite.ScaledHeight();
            Links[i].DisplayCoords = {
                Position.x, Position.y + BgSprite.ScaledHeight() + offset};
          } break;

          case LinkDirection::Down: {
            Links[i].DisplayCoords = {Position.x,
                                      Position.y + BgSprite.ScaledHeight()};
          } break;

          case LinkDirection::Left: {
            Links[i].DisplayCoords = {Position.x - BgSprite.ScaledWidth(),
                                      Position.y};
          } break;

          case LinkDirection::Right: {
            Links[i].DisplayCoords = {Position.x + BgSprite.ScaledWidth(),
                                      Position.y};
          } break;
        }
      }
    }
  }
}

void Background2D::UpdateState(const int bgId) {
  const size_t structOffset = ScrWorkBgStructSize * bgId;
  const size_t structOfsOffset = ScrWorkBgOffsetStructSize * bgId;

  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  Layers = {ScrWork[SW_BG1PRI + structOffset],
            ScrWork[SW_BG1PRI2 + structOffset]};
  Show = GetFlag(SF_BG1DISP + bgId);
  RenderType = ScrWork[SW_BG1FADETYPE + structOffset];

  MaskNumber = ScrWork[SW_BG1MASKNO + structOffset];
  FadeRange = ScrWork[SW_BG1MASKFADERANGE + structOffset];
  FadeCount = ScrWork[SW_BG1FADECT + structOffset];

  const int dispMode = ScrWork[SW_BG1DISPMODE + structOffset];
  switch (dispMode) {
    case 0: {
      Position = -glm::vec2(ScrWork[SW_BG1POSX + structOffset] +
                                ScrWork[SW_BG1POSX_OFS + structOfsOffset],
                            ScrWork[SW_BG1POSY + structOffset] +
                                ScrWork[SW_BG1POSY_OFS + structOfsOffset]) *
                 resolutionScale;

      if (GameInstructionSet == +Vm::InstructionSet::MO8) {
        Position -= (BgSprite.Bounds.GetSize() -
                     glm::vec2(Profile::DesignWidth, Profile::DesignHeight)) *
                    0.5f;
      }

      Scale = {1.0f, 1.0f};
    } break;

    case 1: {
      Position = -glm::vec2(ScrWork[SW_BG1SX + structOffset] +
                                ScrWork[SW_BG1SX_OFS + structOfsOffset],
                            ScrWork[SW_BG1SY + structOffset] +
                                ScrWork[SW_BG1SY_OFS + structOfsOffset]) *
                 resolutionScale;

      Scale = glm::vec2(1280.0f, 720.0f) /
              glm::vec2(ScrWork[SW_BG1LX + structOffset] +
                            ScrWork[SW_BG1LX_OFS + structOfsOffset],
                        ScrWork[SW_BG1LY + structOffset] +
                            ScrWork[SW_BG1LY_OFS + structOfsOffset]);
    } break;

    case 2: {
      Position = glm::vec2(ScrWork[SW_BG1POSX + structOffset] +
                               ScrWork[SW_BG1POSX_OFS + structOfsOffset],
                           ScrWork[SW_BG1POSY + structOffset] +
                               ScrWork[SW_BG1POSY_OFS + structOfsOffset]) *
                 resolutionScale;
      Position -= BgSprite.ScaledBounds().GetSize() / 2.0f;

      Scale = glm::vec2(ScrWork[SW_BG1SIZE + structOffset] +
                        ScrWork[SW_BG1SIZE_OFS + structOfsOffset]) /
              1000.0f;
    } break;

    case 4: {
      Position = -glm::vec2(ScrWork[SW_BG1POSX + structOffset] +
                                ScrWork[SW_BG1POSX_OFS + structOfsOffset],
                            ScrWork[SW_BG1POSY + structOffset] +
                                ScrWork[SW_BG1POSY_OFS + structOfsOffset]) *
                 resolutionScale / 1000.0f;

      Scale = {1.0f, 1.0f};
    } break;
  }

  Origin = BgSprite.ScaledBounds().Center();

  switch (GameInstructionSet) {
    case Vm::InstructionSet::CHLCC:
      Rotation = ScrWorkAngleZToQuaternion(ScrWork[SW_BG1ROTZ + structOffset]);
      break;

    case Vm::InstructionSet::CC:
    default:
      Rotation =
          ScrWorkAngleZToQuaternion(ScrWork[SW_BG1ROTZ + structOffset] +
                                    ScrWork[SW_BG1ROTZ_OFS + structOfsOffset]);
      break;
  }

  switch (GameInstructionSet) {
    case Vm::InstructionSet::CC:
      Tint = ScrWorkGetColor(SW_BG1FILTER + structOffset);
      break;

    default:
      Tint = glm::vec4(1.0f);
      break;
  }
  Tint.a = (ScrWork[SW_BG1ALPHA + structOffset] +
            ScrWork[SW_BG1ALPHA_OFS + structOfsOffset]) /
           256.0f;

  if (ScrWork[SW_BGLINK]) {
    LinkBuffers(ScrWork[SW_BGLINK], bgId);
  } else if (ScrWork[SW_BGLINK2]) {
    LinkBuffers(ScrWork[SW_BGLINK2], bgId);
  } else {
    for (size_t i = 0; i < MaxLinkedBgBuffers; i++) {
      Links[i].Direction = LinkDirection::Off;
      Links[i].LinkedBuffer = nullptr;
    }
  }

  if (BgSprite.Sheet.IsScreenCap) {
    BgSprite.BaseScale *=
        glm::vec2(Profile::DesignWidth / Window->WindowWidth,
                  Profile::DesignHeight / Window->WindowHeight);
  }
}

void Background2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) return;

  std::invoke(BackgroundRenderTable[RenderType], this);
}

void Capture2D::UpdateState(const int capId) {
  const size_t structOffset = ScrWorkCaptureStructSize * capId;
  const size_t structOfsOffset = ScrWorkCaptureOffsetStructSize * capId;

  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  Layers = {ScrWork[SW_CAP1PRI + structOffset],
            ScrWork[SW_CAP1PRI2 + structOffset]};
  Show = GetFlag(SF_CAP1DISP + capId);
  RenderType = ScrWork[SW_CAP1FADETYPE + structOffset];

  MaskNumber = ScrWork[SW_CAP1MASKNO + structOffset];
  FadeRange = ScrWork[SW_CAP1MASKFADERANGE + structOffset];
  FadeCount = ScrWork[SW_CAP1FADECT + structOffset];

  const int dispMode = ScrWork[SW_CAP1DISPMODE + structOffset];
  switch (dispMode) {
    case 0: {
      Position = -glm::vec2(ScrWork[SW_CAP1POSX + structOffset] +
                                ScrWork[SW_CAP1POSX_OFS + structOfsOffset],
                            ScrWork[SW_CAP1POSY + structOffset] +
                                ScrWork[SW_CAP1POSY_OFS + structOfsOffset]) *
                 resolutionScale;

      if (GameInstructionSet == +Vm::InstructionSet::MO8) {
        Position -= (BgSprite.Bounds.GetSize() -
                     glm::vec2(Profile::DesignWidth, Profile::DesignHeight)) *
                    0.5f;
      }

      Scale = {1.0f, 1.0f};
    } break;

    case 1: {
      Position = -glm::vec2(ScrWork[SW_CAP1SX + structOffset] +
                                ScrWork[SW_CAP1SX_OFS + structOfsOffset],
                            ScrWork[SW_CAP1SY + structOffset] +
                                ScrWork[SW_CAP1SY_OFS + structOfsOffset]) *
                 resolutionScale;

      Scale = glm::vec2(1280.0f, 720.0f) /
              glm::vec2(ScrWork[SW_CAP1LX + structOffset] +
                            ScrWork[SW_CAP1LX_OFS + structOfsOffset],
                        ScrWork[SW_CAP1LY + structOffset] +
                            ScrWork[SW_CAP1LY_OFS + structOfsOffset]);
    } break;

    case 2: {
      Position = glm::vec2(ScrWork[SW_CAP1POSX + structOffset] +
                               ScrWork[SW_CAP1POSX_OFS + structOfsOffset],
                           ScrWork[SW_CAP1POSY + structOffset] +
                               ScrWork[SW_CAP1POSY_OFS + structOfsOffset]) *
                 resolutionScale;
      Position -= BgSprite.ScaledBounds().GetSize() / 2.0f;

      Scale = glm::vec2(ScrWork[SW_CAP1SIZE + structOffset] +
                        ScrWork[SW_CAP1SIZE_OFS + structOfsOffset]) /
              1000.0f;
    } break;

    case 4: {
      Position = -glm::vec2(ScrWork[SW_CAP1POSX + structOffset] +
                                ScrWork[SW_CAP1POSX_OFS + structOfsOffset],
                            ScrWork[SW_CAP1POSY + structOffset] +
                                ScrWork[SW_CAP1POSY_OFS + structOfsOffset]) *
                 resolutionScale / 1000.0f;

      Scale = {1.0f, 1.0f};
    } break;
  }

  Origin = BgSprite.ScaledBounds().Center();

  switch (GameInstructionSet) {
    case Vm::InstructionSet::CHLCC:
      Rotation = ScrWorkAngleZToQuaternion(ScrWork[SW_CAP1ROTZ + structOffset]);
      break;

    case Vm::InstructionSet::CC:
    default:
      Rotation =
          ScrWorkAngleZToQuaternion(ScrWork[SW_CAP1ROTZ + structOffset] +
                                    ScrWork[SW_CAP1ROTZ_OFS + structOfsOffset]);
      break;
  }

  switch (GameInstructionSet) {
    case Vm::InstructionSet::CC:
      Tint = ScrWorkGetColor(SW_CAP1FILTER + structOffset);
      break;

    default:
      Tint = glm::vec4(1.0f);
      break;
  }
  Tint.a = (ScrWork[SW_CAP1ALPHA + structOffset] +
            ScrWork[SW_CAP1ALPHA_OFS + structOfsOffset]) /
           256.0f;

  if (BgSprite.Sheet.IsScreenCap) {
    BgSprite.BaseScale *=
        glm::vec2(Profile::DesignWidth / Window->WindowWidth,
                  Profile::DesignHeight / Window->WindowHeight);
  }
}

void Capture2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) return;

  std::invoke(BackgroundRenderTable[RenderType], this);
}

void BackgroundEffect2D::UpdateState(const int bgId) {
  const int structOffset = ScrWorkBgEffStructSize * bgId;
  const int structOfsOffset = ScrWorkBgEffOffsetStructSize * bgId;

  Show = GetFlag(SF_BGEFF1DISP + bgId);
  Layers = {ScrWork[SW_BGEFF1_PRI + structOffset],
            ScrWork[SW_BGEFF1_PRI2 + structOffset]};
  RenderType = ScrWork[SW_BGEFF1_MODE + structOffset];

  MaskNumber = ScrWork[SW_BGEFF1_MASKNO + structOffset];
  FadeRange = ScrWork[SW_BGEFF1_MASKFADERANGE + structOffset];
  FadeCount = ScrWork[SW_BGEFF1_FADECT + structOffset];

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

  VertexCount = maskType == 0 ? 4 : 3;
  for (size_t i = 0; i < VertexCount; i++) {
    const int x =
        ScrWork[SW_BGEFF1_MASK_VERTEX1_X + structOffset + i * 2] +
        ScrWork[SW_BGEFF1_MASK_VERTEX1_OFSX + structOfsOffset + i * 2];
    const int y =
        ScrWork[SW_BGEFF1_MASK_VERTEX1_Y + structOffset + i * 2] +
        ScrWork[SW_BGEFF1_MASK_VERTEX1_OFSY + structOfsOffset + i * 2];
    Vertices[i] = glm::vec2((float)x, (float)y) * resolutionScale;
  }
  if (VertexCount == 4) std::swap(Vertices[1], Vertices[2]);

  const glm::vec2 backgroundOffset =
      glm::vec2(ScrWork[SW_BGEFF1_SX + structOffset],
                ScrWork[SW_BGEFF1_SY + structOffset]) *
      resolutionScale;

  const glm::vec2 pos =
      glm::vec2(ScrWork[SW_BGEFF1_POSX + structOffset] +
                    ScrWork[SW_BGEFF1_OFSX + structOfsOffset],
                ScrWork[SW_BGEFF1_POSY + structOffset] +
                    ScrWork[SW_BGEFF1_OFSY + structOfsOffset]) *
      resolutionScale;

  Position = pos - backgroundOffset;
  StencilOffset = pos - Vertices[0];

  // Origin is the center of mass
  Origin = std::reduce(Vertices.begin(), Vertices.begin() + VertexCount) /
           (float)VertexCount;
}

void BackgroundEffect2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) return;

  // Transform vertices
  const glm::mat4 stencilTransformation = TransformationMatrix(
      Origin, Scale, {Origin, 0.0f}, Rotation, StencilOffset);

  // Draw
  Renderer->SetStencilMode(StencilBufferMode::Write);
  Renderer->ClearStencilBuffer();

  Renderer->DrawConvexShape(
      std::span(Vertices.begin(), Vertices.begin() + VertexCount),
      stencilTransformation, glm::vec4(1.0f));

  Renderer->SetStencilMode(StencilBufferMode::Test);

  std::invoke(BackgroundRenderTable[RenderType], this);

  Renderer->SetStencilMode(StencilBufferMode::Off);
}

void Background2D::RenderRegular() {
  const glm::mat4 transformation =
      TransformationMatrix(Origin, Scale, {Origin, 0.0f}, Rotation, Position);
  Renderer->DrawSprite(BgSprite, transformation, Tint, false);

  for (int i = 0; i < MaxLinkedBgBuffers; i++) {
    if (Links[i].Direction != LinkDirection::Off &&
        Links[i].LinkedBuffer != NULL) {
      const glm::mat4 linkTransformation =
          transformation *
          glm::translate(glm::mat4(1.0f), {Links[i].DisplayCoords, 0.0f});

      Renderer->DrawSprite(Links[i].LinkedBuffer->BgSprite, transformation,
                           Tint, false);
    }
  }
}

void Background2D::RenderMasked() {
  const glm::mat4 transformation =
      TransformationMatrix(Origin, Scale, {Origin, 0.0f}, Rotation, Position);

  Renderer->DrawMaskedSprite(BgSprite, Masks2D[MaskNumber].MaskSprite,
                             FadeCount, FadeRange, transformation, Tint, false,
                             false);
}

void Background2D::RenderMaskedInverted() {
  const glm::mat4 transformation =
      TransformationMatrix(Origin, Scale, {Origin, 0.0f}, Rotation, Position);

  Renderer->DrawMaskedSprite(BgSprite, Masks2D[MaskNumber].MaskSprite,
                             FadeCount, FadeRange, transformation, Tint, true,
                             false);
}

void Background2D::RenderFade() {
  Tint.a *= FadeCount / 256.0f;

  RenderRegular();
}

}  // namespace Impacto