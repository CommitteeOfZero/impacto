#include "cgviewer.h"
#include "../../renderer2d.h"
#include "../../io/vfs.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Vm::Interface;

float const CgMovementStep = 10.0f;
float const ScrollwheelDeltaDivider = 10.0f;
float const ScaleStep = 0.01f;

CgViewer::CgViewer() {}

void CgViewer::UpdateInput() {
  if (PADinputMouseIsDown & PAD1A) {
    Position += Input::CurMousePos - Input::PrevMousePos;
  }

  if (PADinputButtonIsDown & PAD1LEFT) {
    Position.x -= CgMovementStep;
  }
  if (PADinputButtonIsDown & PAD1RIGHT) {
    Position.x += CgMovementStep;
  }
  if (PADinputButtonIsDown & PAD1UP) {
    Position.y -= CgMovementStep;
  }
  if (PADinputButtonIsDown & PAD1DOWN) {
    Position.y += CgMovementStep;
  }

  Scale += Input::MouseWheelDeltaY / ScrollwheelDeltaDivider;
  if (PADinputButtonIsDown & PAD1L1) {
    Scale -= ScaleStep;
  }
  if (PADinputButtonIsDown & PAD1R1) {
    Scale += ScaleStep;
  }
  Scale =
      Scale < MinScale[CurrentVariation] ? MinScale[CurrentVariation] : Scale;

  if (PADinputButtonWentDown & PAD1A || PADinputMouseWentDown & PAD1A) {
    CurrentVariation += 1;
    if (CurrentVariation == VariationCount) {
      CurrentVariation = 0;
    }
  }
}

void CgViewer::Update(float dt) {
  Widget::Update(dt);
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    CgSprites[CurrentVariation][i].BaseScale.x = Scale;
    CgSprites[CurrentVariation][i].BaseScale.y = Scale;
  }
}

void CgViewer::Render() {
  Renderer2D::DrawRect(
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    auto pos =
        i == 0
            ? Position
            : glm::vec2(Position.x,
                        Position.y +
                            CgSprites[CurrentVariation][i - 1].ScaledHeight());
    Renderer2D::DrawSprite(CgSprites[CurrentVariation][i], pos);
  }
}

void CgViewer::LoadCgSprites(
    std::string mountPoint,
    uint16_t loadIds[][Profile::SaveSystem::MaxCGSprites]) {
  Clear();

  int variationIdx = 0;
  while (loadIds[variationIdx][0] != 0xFFFF) {
    int idx = 0;
    float totalHeight = 0.0f;

    while (loadIds[variationIdx][idx] != 0xFFFF) {
      Io::InputStream* stream;
      int64_t err =
          Io::VfsOpen(mountPoint, loadIds[variationIdx][idx], &stream);
      if (err != IoError_OK) return;
      CgTexture.Load(stream);
      delete stream;
      CgSpriteSheets[variationIdx][idx].Texture = CgTexture.Submit();
      CgSpriteSheets[variationIdx][idx].DesignWidth = CgTexture.Width;
      CgSpriteSheets[variationIdx][idx].DesignHeight = CgTexture.Height;
      CgSprites[variationIdx][idx].Sheet = CgSpriteSheets[variationIdx][idx];
      CgSprites[variationIdx][idx].BaseScale = glm::vec2(1.0f);
      CgSprites[variationIdx][idx].Bounds =
          RectF(0.0f, 0.0f, CgSpriteSheets[variationIdx][idx].DesignWidth,
                CgSpriteSheets[variationIdx][idx].DesignHeight);
      idx += 1;
      totalHeight += CgTexture.Height;
      if (idx == MaxCgViewerCgs) break;
    }

    MinScale[variationIdx] = Profile::DesignHeight / totalHeight;
    CgCount[variationIdx] = idx;
    variationIdx += 1;
  }

  Scale = MinScale[0];
  CgSprites[CurrentVariation][0].BaseScale.x = Scale;
  CgSprites[CurrentVariation][0].BaseScale.y = Scale;
  Position = glm::vec2(
      (Profile::DesignWidth - CgSprites[CurrentVariation][0].ScaledWidth()) / 2,
      0.0f);
  VariationCount = variationIdx;
  CurrentVariation = 0;
}

void CgViewer::Clear() {
  for (int i = 0; i < VariationCount; i++) {
    for (int j = 0; j < CgCount[i]; j++) {
      if (CgSpriteSheets[i][j].DesignWidth != 0.0f &&
          CgSpriteSheets[i][j].DesignHeight != 0.0f) {
        glDeleteTextures(1, &CgSpriteSheets[i][j].Texture);
        CgSpriteSheets[i][j].DesignHeight = 0.0f;
        CgSpriteSheets[i][j].DesignWidth = 0.0f;
        CgSpriteSheets[i][j].Texture = 0;
      }
    }
  }
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto