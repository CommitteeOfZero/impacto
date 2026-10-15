#include "blur.h"
#include "../texture/texture.h"
#include "../profile/game.h"
#include "../profile/vm.h"
#include "../renderer/renderer.h"
#include "../vm/vm.h"
#include "../profile/scriptvars.h"

namespace Impacto {
namespace Effects {

using namespace Impacto::Profile::ScriptVars;

void BlurEffect::Render(int iterations) {
  if (Profile::Vm::GameInstructionSet == Vm::InstructionSet::CHLCC) {
    iterations = std::min(iterations / 2, 8);
  } else {
    iterations = std::min(iterations, 32);
  }

  for (int i = 0; i < iterations; i++) {
    RendererBlurDirection direction = i % 2 == 0
                                          ? RendererBlurDirection::Vertical
                                          : RendererBlurDirection::Horizontal;

    Renderer->CaptureScreenCap(BlurSprite);
    Renderer->DrawBlurredSprite(
        {*BlurSprite, RectF{0.0f, 0.0f, Profile::Game::DesignWidth,
                            Profile::Game::DesignHeight}},
        {.BlurDirection = direction});
  }
}

}  // namespace Effects
}  // namespace Impacto