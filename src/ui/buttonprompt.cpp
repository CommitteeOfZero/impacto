#include "buttonprompt.h"

#include "../renderer/renderer.h"
#include "../inputsystem.h"
#include "../profile/patch.h"
#include "../profile/scriptinput.h"
#include "../profile/ui/buttonprompticons.h"
#include "../vm/interface/input.h"
#include "../profile/sprites.h"

namespace Impacto {
namespace UI {

static const Sprite* GetControllerIcon(ButtonPromptIcon::KeyType key) {
  struct {
    const Sprite* operator()(std::string const& key) const {
      static const auto& icons = Profile::ButtonPromptIcons::ControllerIcons;
      auto icon = icons.find(key);
      return icon != icons.end() ? &icon->second : nullptr;
    }

    const Sprite* operator()(Vm::Interface::PADinput button) const {
      static const auto& icons = Profile::ButtonPromptIcons::ControllerIcons;

      using Profile::ScriptInput::PADToController;
      auto mapping = PADToController.find(button);
      if (mapping == PADToController.end()) return nullptr;

      Input::ControllerButton controllerButton = mapping->second;
      if (Input::FaceButtonsFlipped()) {
        if (controllerButton == SDL_GAMEPAD_BUTTON_SOUTH)
          controllerButton = SDL_GAMEPAD_BUTTON_EAST;
        else if (controllerButton == SDL_GAMEPAD_BUTTON_EAST)
          controllerButton = SDL_GAMEPAD_BUTTON_SOUTH;
      }
      auto icon = icons.find(controllerButton);
      return icon != icons.end() ? &icon->second : nullptr;
    }
  } visitor;

  return std::visit(visitor, key);
}

static const Sprite* GetKeyboardIcon(ButtonPromptIcon::KeyType key) {
  struct {
    const Sprite* operator()(std::string const& key) const {
      static const auto& icons = Profile::ButtonPromptIcons::KeyboardIcons;
      auto icon = icons.find(key);
      return icon != icons.end() ? &icon->second : nullptr;
    }

    const Sprite* operator()(Vm::Interface::PADinput button) const {
      using Profile::ScriptInput::PADToKBcustom;
      auto mapping = PADToKBcustom.find(button);
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
  } visitor;

  return std::visit(visitor, key);
}

void DrawButtonPromptIcons(const ButtonPrompt& prompt, glm::vec2 pos,
                           glm::vec4 tint, float maxWidth) {
  if (!Profile::Patch::UniversalButtonIcons) return;

  const bool useControllerIcons =
      Input::CurrentInputDevice == Input::Device::Controller;
  for (const auto& icon : prompt.Icons) {
    const Sprite* iconSprite = useControllerIcons ? GetControllerIcon(icon.Key)
                                                  : GetKeyboardIcon(icon.Key);
    if (!iconSprite) continue;

    glm::vec2 iconPos = pos + icon.Position;
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
