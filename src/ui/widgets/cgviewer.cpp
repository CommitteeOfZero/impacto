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

CgViewer::CgViewer(std::optional<float> fadeDuration) {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.SetDuration(fadeDuration.value_or(0.5f));
}

void CgViewer::Show() {
  if (State != Shown && State != Showing) {
    FadeAnimation.StartIn();
    Widget::Show();
    State = Showing;
  }
}

void CgViewer::Hide() {
  if (State != Hidden && State != Hiding) {
    FadeAnimation.StartOut(true);
    Widget::Hide();
    State = Hiding;
  }
}

void CgViewer::UpdateInput(float dt) {
  const auto isLMBDown = PADinputMouseIsDown & PAD1A;
  if (isLMBDown) {
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
  Scale = std::max(Scale, MinScale[CurrentVariation]);

  if ((PADinputButtonIsDown & (PAD1LEFT | PAD1RIGHT | PAD1UP | PAD1DOWN)) ||
      isLMBDown) {
    const glm::vec2 halfScreen =
        glm::vec2(Profile::DesignWidth, Profile::DesignHeight) * 0.5f;

    auto currentSize = glm::vec2(0.0f);
    if (HorizontalRendering[CurrentVariation]) {
      currentSize.y = CgSprites[CurrentVariation][0].ScaledHeight();
      for (int i = 0; i < CgCount[CurrentVariation]; i++) {
        currentSize.x += CgSprites[CurrentVariation][i].ScaledWidth();
      }
    } else {
      currentSize.x = CgSprites[CurrentVariation][0].ScaledWidth();
      for (int i = 0; i < CgCount[CurrentVariation]; i++) {
        currentSize.y += CgSprites[CurrentVariation][i].ScaledHeight();
      }
    }

    Position[CurrentVariation] = glm::clamp(
        Position[CurrentVariation], halfScreen - currentSize, halfScreen);
  }

  bool mouseAdvance = false;
  if (PADinputMouseWentDown & PAD1A) {
    if (MouseDownTime == 0.0f) {
      MouseDownTime = SDL_GetPerformanceCounter() /
                      static_cast<float>(SDL_GetPerformanceFrequency());
    }
  }

  if (MouseDownTime != 0.0f) {
    const float timeDiff =
        SDL_GetPerformanceCounter() /
            static_cast<float>(SDL_GetPerformanceFrequency()) -
        MouseDownTime;
    if (!isLMBDown) {
      if (timeDiff < MouseAdvanceTime) {
        mouseAdvance = true;
      }
      MouseDownTime = 0.0f;
    } else if (timeDiff >= MouseAdvanceTime ||
               ((Input::CurMousePos - Input::PrevMousePos) != glm::vec2(0))) {
      RequestCursor(CursorType::Pointer);
    }
  }

  if (PADinputButtonWentDown & PAD1A || mouseAdvance) {
    while (true) {
      if (static_cast<int>(CurrentVariation + 1) == VariationCount) {
        if (OnVariationEndHandler) {
          OnVariationEndHandler(this);
          return;
        }
      }
      CurrentVariation += 1;
      FadeAnimation.StartIn(true);
      if (SaveSystem::GetEVVariationIsUnlocked(EvId, CurrentVariation)) break;
    }

    const bool sameOrientation =
        CgCount[CurrentVariation] == CgCount[CurrentVariation - 1] &&
        HorizontalRendering[CurrentVariation] ==
            HorizontalRendering[CurrentVariation - 1];

    if (sameOrientation) {
      Position[CurrentVariation] = Position[CurrentVariation - 1];
      Scale = std::max(PrevScale, MinScale[CurrentVariation]);
    } else {
      Position[CurrentVariation] =
          HorizontalRendering[CurrentVariation]
              ? glm::vec2(0.0f,
                          (Profile::DesignHeight -
                           CgSprites[CurrentVariation][0].ScaledHeight()) /
                              2)
              : glm::vec2((Profile::DesignWidth -
                           CgSprites[CurrentVariation][0].ScaledWidth()) /
                              2,
                          0.0f);
      Scale = MinScale[CurrentVariation];
    }
    PrevScale = Scale;
    for (int i = 0; i < CgCount[CurrentVariation]; i++) {
      CgSprites[CurrentVariation][i].BaseScale.x = Scale;
      CgSprites[CurrentVariation][i].BaseScale.y = Scale;
    }
  }
}

void CgViewer::Update(float dt) {
  Widget::Update(dt);
  FadeAnimation.Update(dt);
  if (FadeAnimation.IsIn() && State == Showing) {
    State = Shown;
  } else if (FadeAnimation.IsOut() && State == Hiding) {
    State = Hidden;
  }
  RectF screen = RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
  auto pointBefore = screen.Center() - Position[CurrentVariation];
  auto pointAfter = pointBefore * (Scale / PrevScale);
  auto diff = pointAfter - pointBefore;
  for (int i = 0; i < CgCount[CurrentVariation]; i++) {
    CgSprites[CurrentVariation][i].BaseScale.x = Scale;
    CgSprites[CurrentVariation][i].BaseScale.y = Scale;
  }
  Position[CurrentVariation] -= diff;
  PrevScale = Scale;
}

void CgViewer::Render() {
  if (State == Hidden) return;
  glm::vec4 col(1.0f);
  if (State != Shown) {
    col.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  }

  Renderer->DrawQuad(
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(0.0f, 0.0f, 0.0f, col.a));

  if (State != Shown || CurrentVariation == 0) {
    RenderVariation(CurrentVariation, col);
  } else {
    if (FadeAnimation.IsPlaying() &&
        FadeAnimation.Direction == AnimationDirection::In) {
      RenderVariation(CurrentVariation - 1, glm::vec4(1.0f));
    }
    glm::vec4 tint(1.0f, 1.0f, 1.0f,
                   glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
    RenderVariation(CurrentVariation, tint);
  }
}

void CgViewer::RenderVariation(size_t variation, glm::vec4 col) const {
  glm::vec2 pos;
  for (int i = 0; i < CgCount[variation]; i++) {
    if (i == 0)
      pos = Position[variation];
    else {
      pos = HorizontalRendering[variation]
                ? glm::vec2(Position[variation].x +
                                CgSprites[variation][i - 1].ScaledWidth(),
                            Position[variation].y)
                : glm::vec2(Position[variation].x,
                            Position[variation].y +
                                CgSprites[variation][i - 1].ScaledHeight());
    }
    Renderer->DrawSprite(CgSprites[variation][i], pos, col);
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
    float totalWidth = 0.0f;
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
      CgSpriteSheets[variationIdx][idx].DesignWidth =
          static_cast<float>(CgTexture.Width);
      CgSpriteSheets[variationIdx][idx].DesignHeight =
          static_cast<float>(CgTexture.Height);
      CgSprites[variationIdx][idx].Sheet = CgSpriteSheets[variationIdx][idx];
      CgSprites[variationIdx][idx].BaseScale = glm::vec2(1.0f);
      CgSprites[variationIdx][idx].Bounds =
          RectF(0.0f, 0.0f, CgSpriteSheets[variationIdx][idx].DesignWidth,
                CgSpriteSheets[variationIdx][idx].DesignHeight);
      idx += 1;
      totalHeight += CgTexture.Height;
      totalWidth += CgTexture.Width;
      if (idx == MaxCgViewerCgs) break;
    }

    MinScale[variationIdx] = sideways ? Profile::DesignWidth / totalWidth
                                      : Profile::DesignHeight / totalHeight;
    CgCount[variationIdx] = idx - static_cast<int>(sideways);
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
  PrevScale = Scale;
  VariationCount = variationIdx;
  CurrentVariation = 0;
}

bool CgViewer::IsOnLastVariation() const {
  return static_cast<int>(CurrentVariation) == VariationCount - 1;
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