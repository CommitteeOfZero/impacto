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
float const MouseAdvanceTime = 0.2f;

CgViewer::CgViewer() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = 0.5f;
  FadeAnimation.DurationOut = 0.5f;
}

void CgViewer::Show() {
  FadeAnimation.StartIn();
  Widget::Show();
}

void CgViewer::Hide() {
  FadeAnimation.StartOut();
  Widget::Hide();
}

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

  bool mouseAdvance = false;
  if (PADinputMouseWentDown & PAD1A) {
    if (MouseDownTime == 0.0f) {
      MouseDownTime =
          SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
    }
  }

  if (MouseDownTime != 0.0f && !(PADinputMouseIsDown & PAD1A)) {
    float currentTime =
        SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
    if (currentTime - MouseDownTime < MouseAdvanceTime) {
      mouseAdvance = true;
    }
    MouseDownTime = 0.0f;
  }

  if (PADinputButtonWentDown & PAD1A || mouseAdvance) {
    while (true) {
      CurrentVariation += 1;
      if (CurrentVariation == VariationCount) {
        if (OnVariationEndHandler) OnVariationEndHandler(this);
        CurrentVariation = 0;
      }
      if (SaveSystem::GetEVVariationIsUnlocked(EvId, CurrentVariation)) break;
    }
    for (int i = 0; i < CgCount[CurrentVariation]; i++) {
      CgSprites[CurrentVariation][i].BaseScale.x = Scale;
      CgSprites[CurrentVariation][i].BaseScale.y = Scale;
    }
  }
}

void CgViewer::Update(float dt) {
  Widget::Update(dt);
  FadeAnimation.Update(dt);
  RectF screen = RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
  auto pointBefore = screen.Center() - Position;
  auto pointAfter =
      pointBefore * (Scale / CgSprites[CurrentVariation][0].BaseScale.x);
  auto diff = pointAfter - pointBefore;
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    CgSprites[CurrentVariation][i].BaseScale.x = Scale;
    CgSprites[CurrentVariation][i].BaseScale.y = Scale;
  }
  Position -= diff;
}

void CgViewer::Render() {
  glm::vec4 col(1.0f, 1.0f, 1.0f,
                glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
  Renderer2D::DrawRect(
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(0.0f, 0.0f, 0.0f, col.a));
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    auto pos =
        i == 0
            ? Position
            : glm::vec2(Position.x,
                        Position.y +
                            CgSprites[CurrentVariation][i - 1].ScaledHeight());
    Renderer2D::DrawSprite(CgSprites[CurrentVariation][i], pos, col);
  }
}

void CgViewer::LoadCgSprites(
    int evId, std::string mountPoint,
    uint16_t loadIds[][Profile::SaveSystem::MaxCGSprites]) {
  Clear();

  EvId = evId;

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