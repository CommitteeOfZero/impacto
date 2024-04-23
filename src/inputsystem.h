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

#ifdef IMPACTO_SDL1_COMPAT
#define SDL_CONTROLLER_AXIS_MAX 6
#define SDL_CONTROLLER_BUTTON_MAX 15
#define SDL_NUM_SCANCODES SDLK_LAST

#define SDL_SCANCODE_LEFT SDLK_LEFT
#define SDL_SCANCODE_RIGHT SDLK_RIGHT
#define SDL_SCANCODE_D SDLK_d

#endif

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

#ifndef IMPACTO_SDL1_COMPAT
// TODO multitouch
extern bool TouchIsDown;
extern bool TouchWentDown;
#endif

}  // namespace Input
}  // namespace Impacto