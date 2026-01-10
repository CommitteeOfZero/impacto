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

BlurEffect::BlurEffect()
    : BlurSprite(SpriteSheet(Profile::DesignWidth, Profile::DesignHeight), 0.0f,
                 0.0f, Profile::DesignWidth, Profile::DesignHeight) {
  BlurSprite.Sheet.IsScreenCap = true;
}

void BlurEffect::Init() {
  if (BlurSprite.Sheet.Texture) {
    Renderer->FreeTexture(BlurSprite.Sheet.Texture);
  }

  Texture texture{};
  texture.LoadSolidColor(static_cast<int>(Profile::DesignWidth),
                         static_cast<int>(Profile::DesignHeight), 0);
  BlurSprite.Sheet.Texture = texture.Submit();
}

BlurEffect::~BlurEffect() { Renderer->FreeTexture(BlurSprite.Sheet.Texture); }

void BlurEffect::Render(int iterations) {
  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::CHLCC) {
    iterations = std::min(iterations / 2, 8);
  } else {
    iterations = std::min(iterations, 32);
  }

  for (int i = 0; i < iterations; i++) {
    RendererBlurDirection direction = i % 2 == 0
                                          ? RendererBlurDirection::Vertical
                                          : RendererBlurDirection::Horizontal;

    Renderer->CaptureScreencap(BlurSprite);
    Renderer->DrawBlurredSprite(
        BlurSprite,
        RectF{0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight},
        glm::mat4(1.0f), direction, glm::vec4(1.0f));
  }
}

}  // namespace Effects
}  // namespace Impacto