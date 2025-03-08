#pragma once

#include "impacto.h"
#include <glm/glm.hpp>

namespace Impacto {
namespace Input {
enum class Device { Mouse, Keyboard, Touch, Controller };

float constexpr ControllerAxisLightThreshold = 0.3f;
float constexpr ControllerAxisHeavyThreshold = 0.8f;

int constexpr MouseButtonsMax = SDL_BUTTON_X2 + 1;
int constexpr FingerTapMax = 2;

void BeginFrame();
void EndFrame();
bool HandleEvent(SDL_Event const* ev);

inline Device CurrentInputDevice = Device::Mouse;

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

// Controller settings

// Advance text on L/R stick, arrow keys, etc.
inline bool AdvanceTextOnDirectionalInput = false;

// Interact with trigger using left and right input in addition
// to their regular counterparts
inline bool DirectionalInputForTrigger = false;

}  // namespace Input
}  // namespace Impacto