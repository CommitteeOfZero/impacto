#include "commonmenu.h"

#include "../../profile/game.h"
#include "../../profile/scriptvars.h"
#include "../../profile/games/cclcc/systemmenu.h"
#include "../../profile/games/cclcc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::CCLCC::SystemMenu;
using namespace Impacto::Profile::CCLCC::TitleMenu;

CommonMenu::CommonMenu(Animation& fadeAnimation) {
  fadeAnimation.Direction = AnimationDirection::In;
  fadeAnimation.LoopMode = AnimationLoopMode::Stop;
}

void CommonMenu::OnShow(float fadeInDuration, float fadeOutDuration,
                        Animation& fadeAnimation) {
  OpenedAsDirect = GetFlag(SF_SYSTEMMENUDIRECT);
  if (OpenedAsDirect) {
    fadeAnimation.DurationIn = FadeInDirectDuration;
    fadeAnimation.DurationOut = FadeOutDirectDuration;
  } else {
    fadeAnimation.DurationIn = fadeInDuration;
    fadeAnimation.DurationOut = fadeOutDuration;
  }
}

void CommonMenu::Init() {
  for (size_t layer = 0; layer < SmokeLayerCount; layer++) {
    Animation& animation = SmokeAnimations[layer];
    animation.SetDuration(SmokeAnimationDurations[layer]);
    animation.LoopMode = AnimationLoopMode::Loop;
    animation.StartIn();
  }

  const RectF viewport = Window->GetViewport();
  Texture captureTexture;
  captureTexture.LoadSolidColor(static_cast<int>(viewport.Width),
                                static_cast<int>(viewport.Height), 0x00000000);
  CaptureSprite.Sheet.Texture = captureTexture.Submit();
  CaptureSprite.Sheet.DesignWidth = viewport.Width;
  CaptureSprite.Sheet.DesignHeight = viewport.Height;
}

void CommonMenu::Update(const float dt) {
  for (Animation& animation : SmokeAnimations) {
    animation.Update(dt);
  }
}

void CommonMenu::DrawSmoke(const float alpha) {
  const glm::vec4 col = {1.0f, 1.0f, 1.0f, alpha};

  for (size_t layer = 0; layer < SmokeLayerCount; layer++) {
    const glm::vec2 pos =
        SmokePosition + glm::vec2(SmokeSprites[layer].ScaledWidth() *
                                      SmokeAnimations[layer].Progress,
                                  0.0f);
    Renderer->DrawSprite(SmokeSprites[layer], pos, col);
    Renderer->DrawSprite(
        SmokeSprites[layer],
        pos - glm::vec2(SmokeSprites[layer].ScaledWidth(), 0.0f), col);
  }
}

void CommonMenu::DrawOverlay(const float alpha) {
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, alpha};

  const RectF maskUvBounds = OverlaySprite.NormalizedBounds();
  std::array<VertexBufferSprites, 4> vertices{
      VertexBufferSprites{
          .Position = {0.0f, 0.0f},
          .UV = {0.0f, 0.0f},
          .Tint = tint,
          .MaskUV = {maskUvBounds.Left(), 1.0f - maskUvBounds.Top()},
      },
      VertexBufferSprites{
          .Position = {Profile::DesignWidth, 0.0f},
          .UV = {1.0f, 0.0f},
          .Tint = tint,
          .MaskUV = {maskUvBounds.Right(), 1.0f - maskUvBounds.Top()},
      },
      VertexBufferSprites{
          .Position = {0.0f, Profile::DesignHeight},
          .UV = {0.0f, 1.0f},
          .Tint = tint,
          .MaskUV = {maskUvBounds.Left(), 1.0f - maskUvBounds.Bottom()},
      },
      VertexBufferSprites{
          .Position = {Profile::DesignWidth, Profile::DesignHeight},
          .UV = {1.0f, 1.0f},
          .Tint = tint,
          .MaskUV = {maskUvBounds.Right(), 1.0f - maskUvBounds.Bottom()},
      },
  };
  constexpr static std::array<uint16_t, 6> indices{0, 1, 2, 1, 2, 3};

  Renderer->CaptureScreencap(CaptureSprite);
  Renderer->DrawPrimitives(CaptureSprite.Sheet, &OverlaySprite.Sheet,
                           ShaderProgramType::HardLightMaskedSprite, vertices,
                           indices, {0.0f, 0.0f});
}

static auto GenerateMatrix(CornersQuad const& corners) {
  auto get2DIndex = [](int x, int y) { return x + (GridColCount + 1) * y; };
  constexpr int xVerticesCount = GridColCount + 1;
  constexpr int yVerticesCount = GridRowCount + 1;
  std::array<float, xVerticesCount> topRowX;
  std::array<float, xVerticesCount> bottomRowX;
  const float topRowWidthDelta =
      (corners.TopRight.x - corners.TopLeft.x) / GridColCount;
  const float bottomRowWidthDelta =
      (corners.BottomRight.x - corners.BottomLeft.x) / GridColCount;
  for (int i = 0; i < xVerticesCount; i++) {
    topRowX[i] = corners.TopLeft.x + topRowWidthDelta * i;
    bottomRowX[i] = corners.BottomLeft.x + bottomRowWidthDelta * i;
  }
  std::array<float, yVerticesCount> leftColY;
  std::array<float, yVerticesCount> rightColY;
  const float leftColHeightDelta =
      (corners.BottomLeft.y - corners.TopLeft.y) / GridRowCount;
  const float rightColHeightDelta =
      (corners.BottomRight.y - corners.TopRight.y) / GridRowCount;
  for (int i = 0; i < yVerticesCount; i++) {
    leftColY[i] = corners.TopLeft.y + leftColHeightDelta * i;
    rightColY[i] = corners.TopRight.y + rightColHeightDelta * i;
  }

  std::array<float, xVerticesCount> xDeltas;
  for (int col = 0; col < std::ssize(xDeltas); col++) {
    xDeltas[col] = (bottomRowX[col] - topRowX[col]) / GridRowCount;
  }

  std::array<float, yVerticesCount> yDeltas;
  for (int row = 0; row < std::ssize(yDeltas); row++) {
    yDeltas[row] = (rightColY[row] - leftColY[row]) / GridColCount;
  }

  std::array<glm::vec2, yVerticesCount * xVerticesCount> matrix;
  for (int row = 0; row < yVerticesCount; row++) {
    for (int col = 0; col < xVerticesCount; col++) {
      glm::vec2 pos = {topRowX[col] + xDeltas[col] * row,
                       leftColY[row] + yDeltas[row] * col};
      matrix[get2DIndex(col, row)] = pos;
    }
  }
  return matrix;
}

glm::vec2 CommonMenu::TransformImageVertex(const glm::vec2 vertex,
                                           glm::mat4 const& transformation,
                                           glm::vec2 const& origin) {
  glm::vec4 transformedVertex = {vertex, 0.0f, 1.0f};
  transformedVertex = transformation * transformedVertex;
  const float perspective = (transformedVertex.z / 2000.0f) + 1.0f;
  transformedVertex *= perspective;
  transformedVertex += glm::vec4(origin, 0.0f, 0.0f);

  return transformedVertex;
}

void CommonMenu::TransformImage(CornersQuad const& sprCorners,
                                CornersQuad const& destCorners,
                                glm::mat4 const& transformation,
                                glm::vec2 origin, glm::vec2 sheetBounds,
                                CommonMenu::GridVertices& vertices,
                                glm::vec4 tint) {
  auto spriteVertices = GenerateMatrix(sprCorners);
  auto displayVertices = GenerateMatrix(destCorners);

  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i] = VertexBufferSprites{
        .Position =
            TransformImageVertex(displayVertices[i], transformation, origin),
        .UV = spriteVertices[i] / sheetBounds,
        .Tint = tint,
    };
  }
}

template <bool IsSystem>
void CommonMenu::DrawBgSprite(MenuState state, const Animation& fadeAnimation,
                              std::optional<Sprite> screenCap) {
  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  // Alpha goes from 0 to 1 in half the time
  float alpha = glm::smoothstep(0.0f, 1.0f, fadeAnimation.Progress);
  // Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f,
  // 720.0f),

  // PrevCTs are saved cause when direct menu is hiding both CTs turn into 0
  // which moves BG to a different offest
  if (state != Hiding || !OpenedAsDirect) {
    PrevSubMenuCt = ScrWork[SW_SYSSUBMENUCT];
    PrevSysMenuCt = ScrWork[SW_SYSMENUCT];
  }

  if constexpr (!IsSystem) {
    if (OpenedAsDirect && state == Shown && ScrWork[SW_SYSSUBMENUCT] < 32) {
      PrevSubMenuCt = 32;
      PrevSysMenuCt = 32;
    }
  }

  float bgOffset = (PrevSubMenuCt * 3000.0f * 0.03125f);
  RectF bgSpriteBounds = SystemMenuBG.Bounds;
  bgSpriteBounds.Translate(
      glm::vec2(BGPosition.x - 0.5f * bgOffset, BGPosition.y));

  const float scale = (1000.0f - (PrevSysMenuCt * 400.0f / 32.0f)) / 1000.0f;
  const glm::quat rotation =
      ScrWorkAnglesToQuaternion(((int)(PrevSysMenuCt * AngleMultiplier.x)),
                                ((int)(PrevSysMenuCt * AngleMultiplier.y)),
                                ((int)(PrevSysMenuCt * AngleMultiplier.z)));
  const glm::mat4 transformation = TransformationMatrix(
      BGTranslationOffset, {scale, scale}, {BGTranslationOffset, 0.0f},
      rotation, -BGTranslationOffset);
  const CornersQuad bgDisp = {
      BGDispOffsetTopLeft,
      BGDispOffsetBottomLeft,
      BGDispOffsetTopRight,
      BGDispOffsetBottomRight,
  };
  TransformImage(bgSpriteBounds, bgDisp, transformation, BGTranslationOffset,
                 SystemMenuBG.Sheet.GetDimensions(), Vertices,
                 glm::vec4(tint, (OpenedAsDirect ? alpha : 1.0f)));

  static constexpr auto indices = []() {
    constexpr uint16_t width = GridColCount + 1;
    constexpr int totalSize = GridRowCount * width * 2 + (GridRowCount - 1) * 2;

    std::array<uint16_t, totalSize> result{};
    size_t index = 0;

    for (uint16_t row = 0; row < GridRowCount; row++) {
      // degenerate triangle here
      if (row > 0) {
        result[index] = result[index - 1];
        index++;
        result[index++] = row * width;
      }
      for (uint16_t col = 0; col < width; col++) {
        result[index++] = row * width + col;
        result[index++] = (row + 1) * width + col;
      }
    }
    return result;
  }();

  Renderer->DrawPrimitives(SystemMenuBG.Sheet, nullptr,
                           ShaderProgramType::Sprite, Vertices, indices,
                           glm::mat4(1.0f), glm::mat4(1.0f), false,
                           TopologyMode::TriangleStrips, true);

  if constexpr (IsSystem) {
    CornersQuad frameDisp = {
        glm::vec2{bgOffset, 0} + FrameOffsetTopLeft,
        glm::vec2{bgOffset, 0} + FrameOffsetBottomLeft,
        glm::vec2{bgOffset, 0} + FrameOffsetTopRight,
        glm::vec2{bgOffset, 0} + FrameOffsetBottomRight,
    };
    frameDisp.Transform([&](glm::vec2 corner) {
      return TransformImageVertex(corner, transformation, BGTranslationOffset);
    });
    Renderer->DrawSprite(SystemMenuFrame, frameDisp);

    CornersQuad screenCapDisp = {
        glm::vec2{bgOffset + 0, 0},
        glm::vec2{bgOffset + 0, Profile::DesignHeight},
        glm::vec2{bgOffset + Profile::DesignWidth, 0},
        glm::vec2{bgOffset + Profile::DesignWidth, Profile::DesignHeight},
    };
    screenCapDisp.Transform([&](glm::vec2 corner) {
      return TransformImageVertex(corner, transformation, BGTranslationOffset);
    });
    if (screenCap.has_value()) {
      Renderer->DrawSprite(*screenCap, screenCapDisp, glm::vec4(1.0f),
                           glm::vec3(0.0f), false, true);
    }
  }
}
template void CommonMenu::DrawBgSprite<true>(MenuState, const Animation&,
                                             std::optional<Sprite>);
template void CommonMenu::DrawBgSprite<false>(MenuState, const Animation&,
                                              std::optional<Sprite>);

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto