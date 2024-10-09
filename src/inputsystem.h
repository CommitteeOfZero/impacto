#pragma once

#include "impacto.h"
#include <glm/glm.hpp>

namespace Impacto {
namespace Input {
enum InputDevice { IDEV_Mouse, IDEV_Keyboard, IDEV_Touch, IDEV_Controller };

float constexpr ControllerAxisLightThreshold = 0.3f;
float constexpr ControllerAxisHeavyThreshold = 0.8f;

int constexpr MouseButtonsMax = SDL_BUTTON_X2 + 1;
int constexpr FingerTapMax = 2;

void BeginFrame();
void EndFrame();
bool HandleEvent(SDL_Event const* ev);

inline InputDevice CurrentInputDevice = IDEV_Mouse;

inline glm::vec2 PrevMousePos = glm::vec2(0.0f);
inline glm::vec2 CurMousePos = glm::vec2(0.0f);

inline int MouseWheelDeltaX = 0;
inline int MouseWheelDeltaY = 0;

// TODO multitouch
inline glm::vec2 PrevTouchPos = glm::vec2(0.0f);
inline glm::vec2 CurTouchPos = glm::vec2(0.0f);

inline float ControllerAxis[SDL_CONTROLLER_AXIS_MAX];

inline bool MouseButtonWentDown[MouseButtonsMax] = {false};
inline bool MouseButtonIsDown[MouseButtonsMax] = {false};
inline bool ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_MAX] = {false};
inline bool ControllerButtonIsDown[SDL_CONTROLLER_BUTTON_MAX] = {false};
inline bool ControllerAxisIsDownLight[SDL_CONTROLLER_AXIS_MAX] = {false};
inline bool ControllerAxisWentDownLight[SDL_CONTROLLER_AXIS_MAX] = {false};
inline bool ControllerAxisIsDownHeavy[SDL_CONTROLLER_AXIS_MAX] = {false};
inline bool ControllerAxisWentDownHeavy[SDL_CONTROLLER_AXIS_MAX] = {false};
inline bool KeyboardButtonWentDown[SDL_NUM_SCANCODES] = {false};
inline bool KeyboardButtonIsDown[SDL_NUM_SCANCODES] = {false};

// TODO multitouch
inline bool TouchIsDown[FingerTapMax]{};
inline bool TouchWentDown[FingerTapMax]{};

}  // namespace Input
}  // namespace Impacto