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
  [[maybe_unused]] RectF viewPort = Window->GetViewport();
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
  for (size_t i = 0; i < Backgrounds.size(); i++) {
    Backgrounds2D[static_cast<int>(i)] = &Backgrounds[i];
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
  bool error = false;

  if (bgId & 0xFF000000) {
    BgTexture.Load1x1((bgId >> 16) & 0xFF, (bgId >> 8) & 0xFF, bgId & 0xFF,
                      0xFF);
  } else {
    Io::Stream* stream;
    int64_t err = Io::VfsOpen("bg", bgId, &stream);

    if (err != IoError_OK) {
      error = true;
    } else {
      BgTexture.Load(stream);
      delete stream;
    }

    if (Profile::UseBgFrameEffects && BgEffTextureIdMap.contains(bgId)) {
      std::for_each(FrameBgEffs.begin(), FrameBgEffs.end(),
                    [](auto& bgEff) { bgEff.Loaded = false; });

      const std::array<int, 4>& textureIds = BgEffTextureIdMap[bgId];

      for (size_t i = 0; i < FrameBgEffs.size(); i++) {
        if (textureIds[i] == 0) continue;

        Io::Stream* bgEffStream;
        err = Io::VfsOpen("bgeffect", textureIds[i], &bgEffStream);

        if (err != IoError::IoError_OK) {
          error = true;
        } else {
          FrameBgEffs[i].BgEffTexture.Load(bgEffStream);
          FrameBgEffs[i].Loaded = true;

          delete bgEffStream;
        }
      }
    }

    if (Profile::UseBgChaEffects && BgEffTextureIdMap.contains(bgId)) {
      ChaBgEff.Loaded = false;

      const int textureId = BgEffTextureIdMap[bgId][3];
      if (textureId != 0) {
        Io::Stream* bgEffStream;
        err = Io::VfsOpen("bgeffect", textureId, &bgEffStream);

        if (err != IoError::IoError_OK) {
          error = true;
        } else {
          ChaBgEff.BgEffTexture.Load(bgEffStream);
          ChaBgEff.Loaded = true;

          delete bgEffStream;
        }
      }
    }
  }

  return !error;
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

  if (Profile::UseBgFrameEffects) {
    for (BgEff& bgEff : FrameBgEffs) {
      bgEff.Loaded = false;

      Renderer->FreeTexture(bgEff.BgEffSprite.Sheet.Texture);
      bgEff.BgEffSprite.Sheet.DesignHeight = 0.0f;
      bgEff.BgEffSprite.Sheet.DesignWidth = 0.0f;
      bgEff.BgEffSprite.Sheet.Texture = 0;
    }
  }

  if (Profile::UseBgChaEffects) {
    ChaBgEff.Loaded = false;

    Renderer->FreeTexture(ChaBgEff.BgEffSprite.Sheet.Texture);
    ChaBgEff.BgEffSprite.Sheet.DesignHeight = 0.0f;
    ChaBgEff.BgEffSprite.Sheet.DesignWidth = 0.0f;
    ChaBgEff.BgEffSprite.Sheet.Texture = 0;
  }

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

  if (Profile::UseBgFrameEffects) {
    for (BgEff& bgEff : FrameBgEffs) {
      if (!bgEff.Loaded) continue;

      bgEff.BgEffSprite.Sheet.Texture = bgEff.BgEffTexture.Submit();

      bgEff.BgEffSprite.Sheet.DesignWidth = (float)bgEff.BgEffTexture.Width;
      bgEff.BgEffSprite.Sheet.DesignHeight = (float)bgEff.BgEffTexture.Height;

      bgEff.BgEffSprite.Bounds =
          RectF(0.0f, 0.0f, bgEff.BgEffSprite.Sheet.DesignWidth,
                bgEff.BgEffSprite.Sheet.DesignHeight);
    }
  }

  if (Profile::UseBgChaEffects && ChaBgEff.Loaded) {
    ChaBgEff.BgEffSprite.Sheet.Texture = ChaBgEff.BgEffTexture.Submit();

    ChaBgEff.BgEffSprite.Sheet.DesignWidth = (float)ChaBgEff.BgEffTexture.Width;
    ChaBgEff.BgEffSprite.Sheet.DesignHeight =
        (float)ChaBgEff.BgEffTexture.Height;

    ChaBgEff.BgEffSprite.Bounds =
        RectF(0.0f, 0.0f, ChaBgEff.BgEffSprite.Sheet.DesignWidth,
              ChaBgEff.BgEffSprite.Sheet.DesignHeight);
  }

  Show = false;

  std::fill(Layers.begin(), Layers.end(), -1);
}

void Background2D::LinkBuffers(const int linkCode, const int currentBufferId) {
  const int srcBufId = (linkCode >> 8) & 0xFF;
  if (srcBufId == Vm::Interface::GetScriptBufferId(currentBufferId)) {
    const LinkDirection dir = (LinkDirection)((linkCode >> 16) & 0xFF);

    for (int i = 0; i < MaxLinkedBgBuffers; i++) {
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

          case LinkDirection::Off:
            break;
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

      Scale = glm::vec2((float)ScrWork[SW_BG1SIZE + structOffset] +
                        (float)ScrWork[SW_BG1SIZE_OFS + structOfsOffset]) /
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
    for (int i = 0; i < MaxLinkedBgBuffers; i++) {
      Links[i].Direction = LinkDirection::Off;
      Links[i].LinkedBuffer = nullptr;
    }
  }

  if (Profile::UseBgChaEffects || Profile::UseBgFrameEffects) {
    BgEffsLayers = {ScrWork[SW_BG1EFFPRI + structOffset],
                    ScrWork[SW_BG1EFFPRI2 + structOffset]};
  }

  if (Profile::UseBgFrameEffects) {
    const int bgNo = ScrWork[SW_BG1NO + structOffset];
    for (size_t bgEffId = 0; bgEffId < FrameBgEffs.size(); bgEffId++) {
      FrameBgEffs[bgEffId].Shader = GetBgEffShader(bgNo, bgEffId);
    }
  }

  if (Profile::UseBgChaEffects) {
    ChaBgEff.Shader = GetBgEffShader(ScrWork[SW_BG1NO + structOffset], 3);
  }

  if (BgSprite.Sheet.IsScreenCap) {
    BgSprite.BaseScale *=
        glm::vec2(Profile::DesignWidth / Window->WindowWidth,
                  Profile::DesignHeight / Window->WindowHeight);
  }
}

void Background2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) return;

  if (Profile::UseBgChaEffects || Profile::UseBgFrameEffects) {
    bool renderBgEffs = false;

    for (size_t i = 0; i < BgEffsLayers.size(); i++) {
      renderBgEffs |= BgEffsLayers[i] != 0 && BgEffsLayers[i] > Layers[i];
    }

    if (renderBgEffs) {
      bool nonSpriteShader = false;

      if (Profile::UseBgFrameEffects) {
        nonSpriteShader |= std::any_of(
            FrameBgEffs.begin(), FrameBgEffs.end() - 1, [](const auto bgEff) {
              return bgEff.Shader != +ShaderProgramType::Sprite;
            });
      }

      if (Profile::UseBgChaEffects) {
        nonSpriteShader |= ChaBgEff.Shader != +ShaderProgramType::Sprite;
      }

      if (!nonSpriteShader) renderBgEffs = false;
    }

    LastRenderedBackground = renderBgEffs ? this : nullptr;
  }

  std::invoke(BackgroundRenderTable[RenderType], this);
}

void Background2D::RenderBgEff(const int layer) {
  if (!Profile::UseBgFrameEffects || layer == 0 ||
      std::find(BgEffsLayers.begin(), BgEffsLayers.end(), layer) ==
          BgEffsLayers.end()) {
    return;
  }

  static Sprite frameSprite{};
  if (frameSprite.Sheet.Texture == 0) {
    Texture frameTexture{};
    frameTexture.LoadSolidColor(Window->WindowWidth, Window->WindowHeight,
                                0x00000000);
    frameSprite.Sheet.Texture = frameTexture.Submit();
    // I am aware this leaks a texture at shutdown
  }

  const std::array<VertexBufferSprites, 4> vertices{
      VertexBufferSprites{
          .Position = {0.0f, Profile::DesignHeight},
          .UV = {0.0f, 1.0f},
          .MaskUV = {0.0f, 0.0f},
      },
      VertexBufferSprites{
          .Position = {0.0f, 0.0f},
          .UV = {0.0f, 0.0f},
          .MaskUV = {0.0f, 1.0f},
      },
      VertexBufferSprites{
          .Position = {Profile::DesignWidth, 0.0f},
          .UV = {1.0f, 0.0f},
          .MaskUV = {1.0f, 1.0f},
      },
      VertexBufferSprites{
          .Position = {Profile::DesignWidth, Profile::DesignHeight},
          .UV = {1.0f, 1.0f},
          .MaskUV = {1.0f, 0.0f},
      },
  };
  constexpr std::array<uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};

  // Render in reverse order
  for (auto bgEff = FrameBgEffs.crbegin(); bgEff != FrameBgEffs.crend();
       bgEff++) {
    if (!bgEff->Loaded) continue;

    Renderer->CaptureScreencap(frameSprite);
    Renderer->DrawVertices(frameSprite.Sheet, &bgEff->BgEffSprite.Sheet,
                           bgEff->Shader, vertices, indices);
  }

  LastRenderedBackground = nullptr;
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

      Scale = glm::vec2((float)ScrWork[SW_CAP1SIZE + structOffset] +
                        (float)ScrWork[SW_CAP1SIZE_OFS + structOfsOffset]) /
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

      Renderer->DrawSprite(Links[i].LinkedBuffer->BgSprite, linkTransformation,
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