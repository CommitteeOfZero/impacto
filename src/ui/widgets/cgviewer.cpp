#include "cgviewer.h"
#include "../../renderer/renderer.h"
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
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.SetDuration(0.5f);
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
    Position[CurrentVariation] += Input::CurMousePos - Input::PrevMousePos;
  }

  if (PADinputButtonIsDown & PAD1LEFT) {
    Position[CurrentVariation].x -= CgMovementStep;
  }
  if (PADinputButtonIsDown & PAD1RIGHT) {
    Position[CurrentVariation].x += CgMovementStep;
  }
  if (PADinputButtonIsDown & PAD1UP) {
    Position[CurrentVariation].y -= CgMovementStep;
  }
  if (PADinputButtonIsDown & PAD1DOWN) {
    Position[CurrentVariation].y += CgMovementStep;
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
  auto pointBefore = screen.Center() - Position[CurrentVariation];
  auto pointAfter =
      pointBefore * (Scale / CgSprites[CurrentVariation][0].BaseScale.x);
  auto diff = pointAfter - pointBefore;
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    CgSprites[CurrentVariation][i].BaseScale.x = Scale;
    CgSprites[CurrentVariation][i].BaseScale.y = Scale;
  }
  Position[CurrentVariation] -= diff;
}

void CgViewer::Render() {
  glm::vec4 col(1.0f, 1.0f, 1.0f,
                glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
  Renderer->DrawQuad(
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(0.0f, 0.0f, 0.0f, col.a));
  glm::vec2 pos;
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    if (i == 0)
      pos = Position[CurrentVariation];
    else {
      pos =
          HorizontalRendering[CurrentVariation]
              ? glm::vec2(Position[CurrentVariation].x +
                              CgSprites[CurrentVariation][i - 1].ScaledWidth(),
                          Position[CurrentVariation].y)
              : glm::vec2(
                    Position[CurrentVariation].x,
                    Position[CurrentVariation].y +
                        CgSprites[CurrentVariation][i - 1].ScaledHeight());
    }
    Renderer->DrawSprite(CgSprites[CurrentVariation][i], pos, col);
  }
}

void CgViewer::LoadCgSprites(
    size_t evId, std::string mountPoint,
    uint16_t loadIds[][Profile::SaveSystem::MaxCGSprites]) {
  Clear();

  EvId = evId;

  int variationIdx = 0;
  while (loadIds[variationIdx][0] != 0xFFFF) {
    int idx = 0;
    float totalHeight = 0.0f;
    bool sideways = false;

    while (loadIds[variationIdx][idx] != 0xFFFF) {
      if (loadIds[variationIdx][idx] == 0x51D3) {
        sideways = true;
        idx += 1;
        continue;
      }
      Io::Stream* stream;
      int64_t err =
          Io::VfsOpen(mountPoint, loadIds[variationIdx][idx], &stream);
      if (err != IoError_OK) return;
      CgTexture.Load(stream);
      delete stream;
      CgSpriteSheets[variationIdx][idx].Texture = CgTexture.Submit();
      CgSpriteSheets[variationIdx][idx].DesignWidth = (float)CgTexture.Width;
      CgSpriteSheets[variationIdx][idx].DesignHeight = (float)CgTexture.Height;
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
    CgCount[variationIdx] = idx - (int)(sideways);
    HorizontalRendering[variationIdx] = sideways;
    CgSprites[variationIdx][0].BaseScale.x = MinScale[variationIdx];
    CgSprites[variationIdx][0].BaseScale.y = MinScale[variationIdx];
    Position[variationIdx] =
        HorizontalRendering[variationIdx]
            ? glm::vec2(0.0f, (Profile::DesignHeight -
                               CgSprites[variationIdx][0].ScaledHeight()) /
                                  2)
            : glm::vec2((Profile::DesignWidth -
                         CgSprites[variationIdx][0].ScaledWidth()) /
                            2,
                        0.0f);
    variationIdx += 1;
  }

  Scale = MinScale[0];
  VariationCount = variationIdx;
  CurrentVariation = 0;
}

bool CgViewer::isOnLastVariation() const {
  return CurrentVariation == VariationCount - 1;
}

void CgViewer::Clear() {
  for (int i = 0; i < VariationCount; i++) {
    for (int j = 0; j < CgCount[i]; j++) {
      if (CgSpriteSheets[i][j].DesignWidth != 0.0f &&
          CgSpriteSheets[i][j].DesignHeight != 0.0f) {
        Renderer->FreeTexture(CgSpriteSheets[i][j].Texture);
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