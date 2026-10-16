#include "buttonprompt.h"

#include "../renderer/renderer.h"
#include "../inputsystem.h"
#include "../profile/patch.h"
#include "../profile/scriptinput.h"
#include "../profile/ui/buttonprompticons.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace UI {

static const Sprite* GetControllerIcon(uint32_t button) {
  using Profile::ScriptInput::PADToController;
  auto mapping = PADToController.find(static_cast<int>(button));
  if (mapping == PADToController.end()) return nullptr;

  Input::ControllerButton controllerButton = mapping->second;
  if (Input::FaceButtonsFlipped()) {
    if (controllerButton == SDL_GAMEPAD_BUTTON_SOUTH)
      controllerButton = SDL_GAMEPAD_BUTTON_EAST;
    else if (controllerButton == SDL_GAMEPAD_BUTTON_EAST)
      controllerButton = SDL_GAMEPAD_BUTTON_SOUTH;
  }

  const auto& icons = Profile::ButtonPromptIcons::ControllerIcons;
  auto icon = icons.find(controllerButton);
  return icon != icons.end() ? &icon->second : nullptr;
}

static const Sprite* GetKeyboardIcon(uint32_t button) {
  using Profile::ScriptInput::PADToKBcustom;
  auto mapping = PADToKBcustom.find(static_cast<int>(button));
  if (mapping == PADToKBcustom.end()) return nullptr;

  const auto& icons = Profile::ButtonPromptIcons::KeyboardIcons;
  for (const auto& kbMapping : mapping->second) {
    auto keys = Vm::Interface::KBcustom.find(kbMapping.Id);
    if (keys == Vm::Interface::KBcustom.end()) continue;

    for (auto key : keys->second) {
      auto icon = icons.find(key);
      if (icon != icons.end()) return &icon->second;
    }
  }
  return nullptr;
}

void DrawButtonPromptIcons(const ButtonPrompt& prompt, glm::vec2 pos,
                           glm::vec4 tint, float maxWidth) {
  if (!Profile::Patch::UniversalButtonIcons) return;

  const bool useControllerIcons =
      Input::CurrentInputDevice == Input::Device::Controller;
  for (const auto& icon : prompt.Icons) {
    const Sprite* iconSprite = useControllerIcons
                                   ? GetControllerIcon(icon.Button)
                                   : GetKeyboardIcon(icon.Button);
    if (!iconSprite) continue;

    glm::vec2 iconPos = pos + icon.Position - iconSprite->Center();
    if (iconPos.x + iconSprite->ScaledWidth() > pos.x + maxWidth) continue;

    Renderer->DrawSprite(*iconSprite, iconPos, tint);
  }
}

void DrawButtonPrompt(const ButtonPrompt& prompt, glm::vec2 pos,
                      glm::vec4 tint) {
  Renderer->DrawSprite(prompt.PromptSprite, pos, tint);
  DrawButtonPromptIcons(prompt, pos, tint);
}

}  // namespace UI
}  // namespace Impacto
