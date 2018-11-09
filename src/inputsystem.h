#pragma once

#include "impacto.h"
#include <glm/glm.hpp>

namespace Impacto {
namespace Input {
enum InputDevice { IDEV_Mouse, IDEV_Keyboard, IDEV_Touch, IDEV_Controller };

float const ControllerAxisLightThreshold = 0.3f;
float const ControllerAxisHeavyThreshold = 0.8f;

int const MouseButtonsMax = SDL_BUTTON_X2 + 1;

void BeginFrame();
void EndFrame();
bool HandleEvent(SDL_Event const* ev);

extern InputDevice CurrentInputDevice;

extern glm::vec2 PrevMousePos;
extern glm::vec2 CurMousePos;

extern int MouseWheelDeltaX;
extern int MouseWheelDeltaY;

// TODO multitouch
extern glm::vec2 PrevTouchPos;
extern glm::vec2 CurTouchPos;

extern float ControllerAxis[SDL_CONTROLLER_AXIS_MAX];

extern bool MouseButtonWentDown[MouseButtonsMax];
extern bool MouseButtonIsDown[MouseButtonsMax];
extern bool ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_MAX];
extern bool ControllerButtonIsDown[SDL_CONTROLLER_BUTTON_MAX];
extern bool ControllerAxisIsDownLight[SDL_CONTROLLER_AXIS_MAX];
extern bool ControllerAxisWentDownLight[SDL_CONTROLLER_AXIS_MAX];
extern bool ControllerAxisIsDownHeavy[SDL_CONTROLLER_AXIS_MAX];
extern bool ControllerAxisWentDownHeavy[SDL_CONTROLLER_AXIS_MAX];
extern bool KeyboardButtonWentDown[SDL_NUM_SCANCODES];
extern bool KeyboardButtonIsDown[SDL_NUM_SCANCODES];

// TODO multitouch
extern bool TouchIsDown;
extern bool TouchWentDown;

}  // namespace Input
}  // namespace Impacto