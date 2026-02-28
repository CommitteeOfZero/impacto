#include "background2d.h"

#include <ranges>
#include <numeric>
#include <glm/ext/quaternion_exponential.hpp>

#include "mask2d.h"
#include "mem.h"
#include "io/memorystream.h"
#include "io/vfs.h"
#include "util.h"
#include "profile/scriptvars.h"
#include "profile/vm.h"
// #include "window.h"
#include "renderer/renderer.h"
#include "effects/wave.h"

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
  for (int i = 0; i < std::ssize(Backgrounds); i++) {
    Backgrounds2D[i] = &Backgrounds[i];
  }

  const auto initCapture = [](Capture2D& capture) {
    capture.BgSprite.Sheet.IsScreenCap = true;
    capture.LoadSolidColor(0xFF000000, Window->WindowWidth,
                           Window->WindowHeight);
    capture.Status = LoadStatus::Loaded;
  };
  for (Capture2D& capture : Screencaptures) initCapture(capture);
  initCapture(MaskCapture);

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

void Background2D::LinkBuffers(const int linkCode, const int currentBgId) {
  const int srcBufId = (linkCode >> 8) & 0xFF;
  if (GetBufferId(srcBufId) != currentBgId) return;

  const LinkDirection dir = static_cast<LinkDirection>((linkCode >> 16) & 0xFF);

  for (int i = 0; i < MaxLinkedBgBuffers; i++) {
    int childBufId = (linkCode >> (i * 24)) & 0xFF;
    if (childBufId == 0) continue;
    childBufId = GetBufferId(childBufId);

    Background2D* const childBuf =
        Backgrounds2D[ScrWork[SW_BG1SURF + childBufId]];
    if (childBuf == nullptr) continue;

    childBuf->RenderSprite.BaseScale = RenderSprite.BaseScale;

    Links[i].LinkedBuffer = childBuf;
    Links[i].Direction = dir;

    constexpr size_t overlapPixelCt = 2;
    switch (dir) {
      case LinkDirection::Up3: {
        float offset =
            i == 0 ? 0.0f
                   : Links[i - 1].LinkedBuffer->RenderSprite.ScaledHeight();
        Links[i].DisplayCoords =
            TransformState.Position -
            glm::vec2(0.0f, RenderSprite.ScaledHeight() + offset);
      } break;

      case LinkDirection::Up: {
        Links[i].DisplayCoords =
            TransformState.Position -
            glm::vec2(0.0f, RenderSprite.ScaledHeight() - overlapPixelCt);
      } break;

      case LinkDirection::Down3: {
        float offset =
            i == 0 ? 0.0f
                   : Links[i - 1].LinkedBuffer->RenderSprite.ScaledHeight();
        Links[i].DisplayCoords =
            TransformState.Position +
            glm::vec2(0.0f, RenderSprite.ScaledHeight() + offset);
      } break;

      case LinkDirection::Down: {
        Links[i].DisplayCoords =
            TransformState.Position +
            glm::vec2(0.0f, RenderSprite.ScaledHeight() - overlapPixelCt);
      } break;

      case LinkDirection::Left: {
        Links[i].DisplayCoords =
            TransformState.Position -
            glm::vec2(RenderSprite.ScaledWidth() - overlapPixelCt, 0.0f);
      } break;

      case LinkDirection::Right: {
        Links[i].DisplayCoords =
            TransformState.Position +
            glm::vec2(RenderSprite.ScaledWidth() - overlapPixelCt, 0.0f);
      } break;

      case LinkDirection::Off:
        break;
    }
  }
}

Background2D::BgTransformState
Background2D::BgTransformState::GetBgTransformState(int bgId) {
  assert(ScrWorkBgStructSize >= 20);
  assert(ScrWorkBgOffsetStructSize >= 10);

  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  const std::span<const int> bgStruct =
      std::span(ScrWork.begin() + ScrWorkBgStructSize * bgId, ScrWork.end());
  const std::span<const int> bgOfsStruct = std::span(
      ScrWork.begin() + ScrWorkBgOffsetStructSize * bgId, ScrWork.end());

  BgTransformState state;

  state.Position =
      glm::vec2(bgStruct[SW_BG1POSX] + bgOfsStruct[SW_BG1POSX_OFS],
                bgStruct[SW_BG1POSY] + bgOfsStruct[SW_BG1POSY_OFS]) *
      resolutionScale;

  state.Scale = glm::vec2(static_cast<float>(bgStruct[SW_BG1SIZE] +
                                             bgOfsStruct[SW_BG1SIZE_OFS])) /
                1000.0f;

  state.Rotation = ScrWorkAngleZToQuaternion(
      Profile::Vm::ScrWorkBgStructSize >= 40
          ? bgStruct[SW_BG1ROTZ] + bgOfsStruct[SW_BG1ROTZ_OFS]
          : bgStruct[SW_BG1ROTZ]);

  state.Subsection =
      RectF(static_cast<float>(bgStruct[SW_BG1SX] + bgOfsStruct[SW_BG1SX_OFS]),
            static_cast<float>(bgStruct[SW_BG1SY] + bgOfsStruct[SW_BG1SY_OFS]),
            static_cast<float>(bgStruct[SW_BG1LX] + bgOfsStruct[SW_BG1LX_OFS]),
            static_cast<float>(bgStruct[SW_BG1LY] + bgOfsStruct[SW_BG1LY_OFS]))
          .Scale(resolutionScale, glm::vec2(0.0f));

  return state;
}

Background2D::BgTransformState
Background2D::BgTransformState::GetCapTransformState(int capId) {
  assert(ScrWorkCaptureStructSize >= 20);
  assert(ScrWorkCaptureOffsetStructSize >= 10);

  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  const std::span<const int> capStruct = std::span(
      ScrWork.begin() + ScrWorkCaptureStructSize * capId, ScrWork.end());
  const std::span<const int> capOfsStruct = std::span(
      ScrWork.begin() + ScrWorkCaptureOffsetStructSize * capId, ScrWork.end());

  BgTransformState state;

  state.Position =
      glm::vec2(capStruct[SW_CAP1POSX] + capOfsStruct[SW_CAP1POSX_OFS],
                capStruct[SW_CAP1POSY] + capOfsStruct[SW_CAP1POSY_OFS]) *
      resolutionScale;

  state.Scale = glm::vec2(static_cast<float>(capStruct[SW_CAP1SIZE] +
                                             capOfsStruct[SW_CAP1SIZE_OFS])) /
                1000.0f;

  state.Rotation = ScrWorkAngleZToQuaternion(
      Profile::Vm::ScrWorkBgEffOffsetStructSize >= 20
          ? capStruct[SW_CAP1ROTZ] + capOfsStruct[SW_CAP1ROTZ_OFS]
          : capStruct[SW_CAP1ROTZ]);

  state.Subsection = RectF(static_cast<float>(capStruct[SW_CAP1SX] +
                                              capOfsStruct[SW_CAP1SX_OFS]),
                           static_cast<float>(capStruct[SW_CAP1SY] +
                                              capOfsStruct[SW_CAP1SY_OFS]),
                           static_cast<float>(capStruct[SW_CAP1LX] +
                                              capOfsStruct[SW_CAP1LX_OFS]),
                           static_cast<float>(capStruct[SW_CAP1LY] +
                                              capOfsStruct[SW_CAP1LY_OFS]))
                         .Scale(resolutionScale, glm::vec2(0.0f));

  return state;
}

void Background2D::SetTransformState(int dispMode, BgTransformState state) {
  TransformState = BgTransformState();

  const bool newEngine = ScrWorkBgStructSize >= 40;

  const auto scaleToFullscreen = [&]() {
    TransformState.Scale =
        glm::vec2(Profile::DesignWidth, Profile::DesignHeight) /
        RenderSprite.ScaledBounds().GetSize();
  };

  switch (dispMode) {
    case 0: {
      TransformState.Position = -state.Position;
    } break;

    case 1: {
      const RectF scaledSubsection = state.Subsection.Scale(
          glm::vec2(1.0f) / RenderSprite.BaseScale, glm::vec2(0.0f));
      RenderSprite.Bounds =
          RectF::Intersection(RenderSprite.Bounds, scaledSubsection);

      if (newEngine) scaleToFullscreen();
    } break;

    case 2: {
      TransformState.Scale = state.Scale;

      if (newEngine) {
        TransformState.Position = state.Position - RenderSprite.Center();
        TransformState.Origin = RenderSprite.Center();
      } else {
        TransformState.Position =
            glm::vec2(Profile::DesignWidth, Profile::DesignHeight) / 2.0f -
            state.Position * TransformState.Scale;
      }
    } break;

    case 3: {
      const RectF scaledSubsection = state.Subsection.Scale(
          glm::vec2(1.0f) / RenderSprite.BaseScale, glm::vec2(0.0f));
      RenderSprite.Bounds =
          RectF::Intersection(RenderSprite.Bounds, scaledSubsection);

      if (newEngine) {
        TransformState.Position = state.Position;
      } else {
        scaleToFullscreen();
      }
    } break;

    case 4: {
      TransformState.Position = -state.Position / 1000.0f;
    } break;

    case 6: {
      TransformState.Origin = RenderSprite.Center();
      TransformState.Position = state.Position - TransformState.Origin;
      TransformState.Scale = state.Scale;
      TransformState.Rotation = state.Rotation;
    } break;

    default: {
      RenderSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
    } break;
  }
}

void Background2D::UpdateState(const int bgId) {
  const size_t structOffset = ScrWorkBgStructSize * bgId;
  const size_t structOfsOffset = ScrWorkBgOffsetStructSize * bgId;

  Layers = {ScrWork[SW_BG1PRI + structOffset],
            ScrWork[SW_BG1PRI2 + structOffset]};
  Show = GetFlag(SF_BG1DISP + bgId);
  RenderType = ScrWork[SW_BG1FADETYPE + structOffset];

  MaskNumber = ScrWork[SW_BG1MASKNO + structOffset];
  FadeRange = ScrWork[SW_BG1MASKFADERANGE + structOffset];
  FadeCount = ScrWork[SW_BG1FADECT + structOffset];

  RenderSprite = BgSprite;
  for (int i = static_cast<int>(Profile::ScreenCaptureCount) - 1; i >= 0; i--) {
    const int capOffset = i * Profile::Vm::ScrWorkCaptureEffectInfoStructSize;
    if (ScrWork[SW_EFF_CAP_BUF + capOffset] == GetScriptBufferId(bgId + 1)) {
      RenderSprite = Screencaptures[i].BgSprite;
    }
  }

  SetTransformState(ScrWork[SW_BG1DISPMODE + structOffset],
                    BgTransformState::GetBgTransformState(bgId));

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
}

void Background2D::Render(const int layer) {
  if (!Show || !OnLayer(layer) ||
      (Status != LoadStatus::Loaded &&
       RenderSprite.Sheet.Texture == BgSprite.Sheet.Texture)) {
    return;
  }

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
    Renderer->DrawPrimitives(frameSprite.Sheet, &bgEff->BgEffSprite.Sheet,
                             bgEff->Shader, vertices, indices);
  }

  LastRenderedBackground = nullptr;
}

void Capture2D::UpdateState(const int capId) {
  const size_t structOffset = ScrWorkCaptureStructSize * capId;
  const size_t structOfsOffset = ScrWorkCaptureOffsetStructSize * capId;

  Layers = {ScrWork[SW_CAP1PRI + structOffset],
            ScrWork[SW_CAP1PRI2 + structOffset]};
  Show = GetFlag(SF_CAP1DISP + capId);
  RenderType = ScrWork[SW_CAP1FADETYPE + structOffset];

  MaskNumber = ScrWork[SW_CAP1MASKNO + structOffset];
  FadeRange = ScrWork[SW_CAP1MASKFADERANGE + structOffset];
  FadeCount = ScrWork[SW_CAP1FADECT + structOffset];

  SetTransformState(ScrWork[SW_CAP1DISPMODE + structOffset],
                    BgTransformState::GetCapTransformState(capId));

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
}

void Capture2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) {
    return;
  }

  std::invoke(BackgroundRenderTable[RenderType], this);
}

Background2D::BgTransformState
Background2D::BgTransformState::GetBgEffTransformState(int bgEffId) {
  assert(ScrWorkBgEffStructSize >= 30);
  assert(ScrWorkBgEffOffsetStructSize >= 20);

  const glm::vec2 resolutionScale = {Profile::DesignWidth / 1280.0f,
                                     Profile::DesignHeight / 720.0f};

  const std::span<const int> bgEffStruct = std::span(
      ScrWork.begin() + ScrWorkBgEffStructSize * bgEffId, ScrWork.end());
  const std::span<const int> bgEffOfsStruct = std::span(
      ScrWork.begin() + ScrWorkBgEffOffsetStructSize * bgEffId, ScrWork.end());

  BgTransformState state;

  state.Position =
      glm::vec2(bgEffStruct[SW_BGEFF1_POSX] + bgEffOfsStruct[SW_BGEFF1_OFSX],
                bgEffStruct[SW_BGEFF1_POSY] + bgEffOfsStruct[SW_BGEFF1_OFSY]) *
      glm::vec2(Profile::DesignWidth / 1280.0f, Profile::DesignHeight / 720.0f);

  state.Scale =
      glm::vec2(static_cast<float>(bgEffStruct[SW_BGEFF1_SIZE] +
                                   bgEffOfsStruct[SW_BGEFF1_SIZE_OFS])) /
      1000.0f;

  state.Rotation = ScrWorkAnglesToQuaternion(
      bgEffStruct[SW_BGEFF1_ROTX] + bgEffOfsStruct[SW_BGEFF1_ROTX_OFS],
      bgEffStruct[SW_BGEFF1_ROTY] + bgEffOfsStruct[SW_BGEFF1_ROTY_OFS],
      bgEffStruct[SW_BGEFF1_ROTZ] + bgEffOfsStruct[SW_BGEFF1_ROTZ_OFS]);

  state.Subsection = RectF(static_cast<float>(bgEffStruct[SW_BGEFF1_SX] +
                                              bgEffOfsStruct[SW_BGEFF1_SX_OFS]),
                           static_cast<float>(bgEffStruct[SW_BGEFF1_SY] +
                                              bgEffOfsStruct[SW_BGEFF1_SY_OFS]),
                           1280.0f, 720.0f)
                         .Scale(resolutionScale, glm::vec2(0.0f));

  return state;
}

void BackgroundEffect2D::SetTransformState(const int dispMode,
                                           BgTransformState state) {
  TransformState = state;
  RenderSprite = BgSprite;

  state.Subsection.Scale(glm::vec2(1.0f) / RenderSprite.BaseScale,
                         glm::vec2(0.0f));
  RenderSprite.Bounds = {state.Subsection.X, state.Subsection.Y,
                         RenderSprite.Bounds.Width - state.Subsection.X,
                         RenderSprite.Bounds.Height - state.Subsection.Y};

  const glm::vec2 centerOfMass =
      std::reduce(Vertices.begin(), Vertices.begin() + VertexCount) /
      (float)VertexCount;

  TransformState.Origin = centerOfMass - Vertices[0];

  StencilTransformation = TransformationMatrix(
      centerOfMass, TransformState.Scale, {centerOfMass, 0.0f},
      TransformState.Rotation, -Vertices[0] + state.Position);
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

  SetTransformState(maskType, BgTransformState::GetBgEffTransformState(bgId));
}

void BackgroundEffect2D::Render(const int layer) {
  if (Status != LoadStatus::Loaded || !OnLayer(layer) || !Show) {
    return;
  }

  // Draw
  Renderer->SetStencilMode(StencilBufferMode::Write);
  Renderer->ClearStencilBuffer();

  Renderer->DrawConvexShape(
      std::span(Vertices.begin(), Vertices.begin() + VertexCount),
      StencilTransformation, glm::vec4(1.0f));

  Renderer->SetStencilMode(StencilBufferMode::Test);

  std::invoke(BackgroundRenderTable[RenderType], this);

  Renderer->SetStencilMode(StencilBufferMode::Off);
}

template <bool PhaseZero>
void Background2D::RenderBgWave() {
  if (FadeCount == 0) {
    return;
  }

  if constexpr (PhaseZero) {
    Effects::WaveBG.CalcPos(0);
  } else {
    // maybe that's it, maybe not
    Effects::WaveBG.CalcPos(MaskNumber);
  }

  PrimitiveData primitives = Effects::WaveBG.GetPrimitives();

  Renderer->DrawPrimitives(
      BgSprite.Sheet, &Masks2D[MaskNumber].MaskSprite.Sheet,
      ShaderProgramType::MaskedSprite, primitives.Vertices, primitives.Indices,
      TransformState.ToMatrix(), glm::mat4(1.0f), false,
      TopologyMode::TriangleStrips);
}

void Background2D::RenderRegular() {
  Renderer->DrawSprite(RenderSprite, TransformState.ToMatrix(), Tint);

  for (int i = 0; i < MaxLinkedBgBuffers; i++) {
    if (Links[i].Direction != LinkDirection::Off &&
        Links[i].LinkedBuffer != nullptr) {
      const glm::mat4 linkTransformation =
          TransformationMatrix(TransformState.Origin, TransformState.Scale,
                               {TransformState.Origin, 0.0f},
                               TransformState.Rotation, Links[i].DisplayCoords);
      Renderer->DrawSprite(Links[i].LinkedBuffer->RenderSprite,
                           linkTransformation, Tint, false);
    }
  }
}

void Background2D::RenderMasked() {
  Renderer->DrawMaskedSprite(RenderSprite, Masks2D[MaskNumber].MaskSprite,
                             FadeCount, FadeRange, TransformState.ToMatrix(),
                             Tint, false, false);
}

void Background2D::RenderCaptureMasked() {
  Renderer->DrawMaskedBinarySprite(RenderSprite, MaskCapture.BgSprite,
                                   TransformState.ToMatrix(), std::nullopt,
                                   Tint, false);
}

void Background2D::RenderMaskedInverted() {
  Renderer->DrawMaskedSprite(RenderSprite, Masks2D[MaskNumber].MaskSprite,
                             FadeCount, FadeRange, TransformState.ToMatrix(),
                             Tint, true, false);
}

void Background2D::RenderFade() {
  Tint.a *= FadeCount / 256.0f;

  RenderRegular();
}

constexpr size_t ExplodeGridWidth = 64;
constexpr size_t ExplodeGridHeight = 36;
struct ExplodeTri {
  std::array<glm::vec2, 3> SpritePositions{};
  std::array<glm::vec2, 3> VertexOffsets{};

  glm::quat Rotation = {1.0f, 0.0f, 0.0f, 0.0f};
  glm::quat RotationSpeed = {1.0f, 0.0f, 0.0f, 0.0f};

  glm::vec2 DisplayPosition{};
  glm::vec2 InitialDisplayPosition{};
  glm::vec2 TranslationSpeed{};

  float Alpha = 1.0f;
  uint8_t StartFadeOutTime = 0;
};
static std::array<ExplodeTri, ExplodeGridWidth * ExplodeGridHeight * 2>
    ExplodeTris;

void ResetExplodeTris(const Sprite& renderSprite) {
  const glm::vec2 renderDimensions = renderSprite.ScaledBounds().GetSize();
  const glm::vec2 cellDimensions =
      renderDimensions / glm::vec2(ExplodeGridWidth, ExplodeGridHeight);

  // Initialize gridpoint positions
  std::array<glm::vec2, (ExplodeGridWidth + 1) * (ExplodeGridHeight + 1)> grid;

  for (size_t y = 0; y <= ExplodeGridHeight; y++) {
    for (size_t x = 0; x <= ExplodeGridWidth; x++) {
      const glm::ivec2 randomOffsetLimits =
          glm::ivec2(cellDimensions + glm::vec2(0.5f)) * 10;
      const glm::vec2 randomOffset = glm::vec2(CALCrnd(randomOffsetLimits.x),
                                               CALCrnd(randomOffsetLimits.y)) /
                                     10.0f;
      glm::vec2 gridPointPos =
          cellDimensions * (glm::vec2(x, y) - glm::vec2(0.5f)) + randomOffset;

      // Clamp edge grid points to the edge of the sprite
      if (x == 0)
        gridPointPos.x = 0.0f;
      else if (x == ExplodeGridWidth)
        gridPointPos.x = renderDimensions.x;

      if (y == 0)
        gridPointPos.y = 0.0f;
      else if (y == ExplodeGridHeight)
        gridPointPos.y = renderDimensions.y;

      const size_t idx = y * (ExplodeGridWidth + 1) + x;
      grid[idx] = glm::clamp(gridPointPos, {0.0f, 0.0f}, renderDimensions);
    }
  }

  // Fill the tri array
  ExplodeTris.fill(ExplodeTri());
  for (size_t y = 0; y < ExplodeGridHeight; y++) {
    for (size_t x = 0; x < ExplodeGridWidth; x++) {
      for (uint8_t triIdxInSquare = 0; triIdxInSquare < 2; triIdxInSquare++) {
        const size_t triIdx = (y * ExplodeGridWidth + x) * 2 + triIdxInSquare;
        ExplodeTri& tri = ExplodeTris[triIdx];

        tri.VertexOffsets =
            triIdxInSquare
                ? std::array{grid[y * (ExplodeGridWidth + 1) + x + 1],
                             grid[(y + 1) * (ExplodeGridWidth + 1) + x],
                             grid[(y + 1) * (ExplodeGridWidth + 1) + x + 1]}
                : std::array{grid[y * (ExplodeGridWidth + 1) + x],
                             grid[y * (ExplodeGridWidth + 1) + x + 1],
                             grid[(y + 1) * (ExplodeGridWidth + 1) + x]};

        // Set the UVs for each vertex by "overlaying" the vertex grid onto the
        // sprite's texture
        const auto getSpritePosition = [&](glm::vec2 gridPosition) {
          const glm::vec2 texturePos = gridPosition / renderSprite.BaseScale +
                                       renderSprite.Bounds.GetPos();
          const glm::vec2 normalizedTexturePos =
              texturePos / renderSprite.Sheet.GetDimensions();
          return normalizedTexturePos;
        };
        std::ranges::transform(tri.VertexOffsets, tri.SpritePositions.begin(),
                               getSpritePosition);

        // Set the InitialDisplayPosition to the center of the bounding square
        // and make VertexOffsets the offsets from this center
        const glm::vec2 topLeft =
            glm::min(tri.VertexOffsets[0],
                     glm::min(tri.VertexOffsets[1], tri.VertexOffsets[2]));
        const glm::vec2 bottomRight =
            glm::max(tri.VertexOffsets[0],
                     glm::max(tri.VertexOffsets[1], tri.VertexOffsets[2]));
        const glm::vec2 center = (topLeft + bottomRight) / 2.0f;
        tri.InitialDisplayPosition = center;
        std::ranges::transform(
            tri.VertexOffsets, tri.VertexOffsets.begin(),
            [center](glm::vec2 pos) { return pos - center; });

        // TranslationSpeed is not based on design dimensions in the
        // MAGES. engine either
        tri.TranslationSpeed =
            glm::vec2(CALCrnd(1200), CALCrnd(1200)) / 100.0f - glm::vec2(6.0f);
        tri.RotationSpeed = ScrWorkAnglesToQuaternion(CALCrnd(0x400) - 0x200,
                                                      CALCrnd(0x400) - 0x200,
                                                      CALCrnd(0x400) - 0x200);
        tri.StartFadeOutTime = static_cast<uint8_t>(CALCrnd(20) + 10);
      }
    }
  }
}

static void UpdateExplode(float fadeCount) {
  // Don't run this every frame; only when the fadeCount changed
  static float lastFadeCount = -1.0f;
  if (lastFadeCount == fadeCount) return;
  lastFadeCount = fadeCount;

  bool anyTriVisible = false;
  for (ExplodeTri& tri : ExplodeTris) {
    tri.DisplayPosition =
        tri.InitialDisplayPosition + tri.TranslationSpeed * fadeCount;
    tri.Rotation = glm::pow(tri.RotationSpeed, fadeCount);

    [[maybe_unused]] static constexpr float fadeCountMax = 64.0f;
    static constexpr float fadeOutDuration = 32.0f;
    const float fadeOutProgress =
        (fadeCount - tri.StartFadeOutTime) / fadeOutDuration;
    tri.Alpha = std::clamp(1.0f - fadeOutProgress, 0.0f, 1.0f);
    assert(tri.StartFadeOutTime + fadeOutDuration <= fadeCountMax);

    anyTriVisible |= tri.Alpha > 0.0f;
  }

  SetFlag(SF_BGEXPLOSIONVISIBLE, anyTriVisible);
}

void Background2D::RenderExplode() {
  if (FadeCount == -1) {
    ResetExplodeTris(RenderSprite);
    return;
  }

  UpdateExplode(static_cast<float>(FadeCount));

  static std::array<VertexBufferSprites, ExplodeTris.size() * 3> vertices;
  size_t vertexIdx = 0;
  for (const ExplodeTri& tri : ExplodeTris) {
    const glm::mat4 transformationMatrix =
        TransformationMatrix({0.0f, 0.0f}, {1.0f, 1.0f}, glm::vec3(0.0f),
                             tri.Rotation, tri.DisplayPosition);

    for (size_t i = 0; i < 3; i++) {
      vertices[vertexIdx + i] = VertexBufferSprites{
          .Position = transformationMatrix *
                      glm::vec4(tri.VertexOffsets[i], 0.0f, 1.0f),
          .UV = tri.SpritePositions[i],
          .Tint = {1.0f, 1.0f, 1.0f, tri.Alpha},
      };
    }

    vertexIdx += 3;
  }

  const static std::array<uint16_t, vertices.size()> indices = []() {
    std::array<uint16_t, vertices.size()> tempIndices;
    std::iota(tempIndices.begin(), tempIndices.end(), 0);
    return tempIndices;
  }();

  Renderer->DrawPrimitives(RenderSprite.Sheet, ShaderProgramType::Sprite,
                           vertices, indices, glm::mat4(1.0f));
}

bool IsBgWaveEffectActive() {
  return std::ranges::any_of(Backgrounds, [](const auto& currentBg) {
    return currentBg.RenderType == 14 || currentBg.RenderType == 28;
  });
}

}  // namespace Impacto