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
bool HandleEvent(SDL_Event const* ev);

inline Device CurrentInputDevice = Device::Mouse;

inline glm::vec2 PrevMousePos = glm::vec2(0.0f);
inline glm::vec2 CurMousePos = glm::vec2(0.0f);

inline int MouseWheelDeltaX = 0;
inline int MouseWheelDeltaY = 0;

// TODO multitouch
inline glm::vec2 PrevTouchPos = glm::vec2(0.0f);
inline glm::vec2 CurTouchPos = glm::vec2(0.0f);

inline float ControllerAxisValue[SDL_GAMEPAD_AXIS_COUNT];

inline bool MouseButtonWentDown[MouseButtonsMax] = {false};
inline bool MouseButtonIsDown[MouseButtonsMax] = {false};
inline bool ControllerButtonWentDown[SDL_GAMEPAD_BUTTON_COUNT] = {false};
inline bool ControllerButtonIsDown[SDL_GAMEPAD_BUTTON_COUNT] = {false};
inline bool ControllerAxisIsDownLight[SDL_GAMEPAD_AXIS_COUNT] = {false};
inline bool ControllerAxisWentDownLight[SDL_GAMEPAD_AXIS_COUNT] = {false};
inline bool ControllerAxisIsDownHeavy[SDL_GAMEPAD_AXIS_COUNT] = {false};
inline bool ControllerAxisWentDownHeavy[SDL_GAMEPAD_AXIS_COUNT] = {false};
inline bool KeyboardButtonWentDown[SDL_SCANCODE_COUNT] = {false};
inline bool KeyboardButtonIsDown[SDL_SCANCODE_COUNT] = {false};

// TODO multitouch
inline bool TouchIsDown[FingerTapMax]{};
inline bool TouchWentDown[FingerTapMax]{};

// Using statements to ensure that types are coming from this header (for
// consistent magic enum range specialization).
using KeyboardScanCode = SDL_Scancode;
using ControllerButton = SDL_GamepadButton;
using ControllerAxis = SDL_GamepadAxis;

}  // namespace Input
}  // namespace Impacto

namespace magic_enum::customize {
template <>
struct enum_range<Impacto::Input::KeyboardScanCode> {
  static constexpr int min = 0;
  static constexpr int max = 512;
  // keep underscore because numbers can't be at the start of an identifier
  static constexpr size_t prefix_length =
      std::string_view("SDL_SCANCODE").size();
};

template <>
constexpr customize_t
enum_type_name<Impacto::Input::KeyboardScanCode>() noexcept {
  return "KeyboardScanCode";
};

template <>
struct enum_range<Impacto::Input::ControllerButton> {
  static constexpr size_t prefix_length =
      std::string_view("SDL_GAMEPAD_BUTTON_").size();
};
template <>
constexpr customize_t
enum_type_name<Impacto::Input::ControllerButton>() noexcept {
  return "ControllerButton";
};

template <>
struct enum_range<Impacto::Input::ControllerAxis> {
  static constexpr size_t prefix_length =
      std::string_view("SDL_GAMEPAD_AXIS_").size();
};
template <>
constexpr customize_t
enum_type_name<Impacto::Input::ControllerAxis>() noexcept {
  return "ControllerAxis";
};
}  // namespace magic_enum::customize