#pragma once

#include "impacto.h"
#include <glm/glm.hpp>

namespace Impacto {

enum InputDevice { IDEV_Mouse, IDEV_Keyboard, IDEV_Touch, IDEV_Controller };

class InputSystem {
 private:
  static int const MouseButtonsMax = SDL_BUTTON_X2 + 1;

 public:
  void BeginFrame();
  void EndFrame();

  bool HandleEvent(SDL_Event const* ev);

  InputDevice CurrentInputDevice = IDEV_Mouse;

  glm::vec2 PrevMousePos = glm::vec2(0.0f);
  glm::vec2 CurMousePos = glm::vec2(0.0f);

  int MouseWheelDeltaX = 0;
  int MouseWheelDeltaY = 0;

  // TODO multitouch
  glm::vec2 PrevTouchPos = glm::vec2(0.0f);
  glm::vec2 CurTouchPos = glm::vec2(0.0f);

  float ControllerAxis[SDL_CONTROLLER_AXIS_MAX] = {0.0f};

  bool MouseButtonWentDown[MouseButtonsMax] = {0};
  bool MouseButtonIsDown[MouseButtonsMax] = {0};
  bool ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_MAX] = {0};
  bool ControllerButtonIsDown[SDL_CONTROLLER_BUTTON_MAX] = {0};
  bool KeyboardButtonWentDown[SDL_NUM_SCANCODES] = {0};
  bool KeyboardButtonIsDown[SDL_NUM_SCANCODES] = {0};

  // TODO multitouch
  bool TouchIsDown = false;
  bool TouchWentDown = false;

 private:
  SDL_FingerID CurrentFinger = 0;
};

}  // namespace Impacto