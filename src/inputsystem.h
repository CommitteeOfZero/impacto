#pragma once

#include "impacto.h"
#include <glm/glm.hpp>

namespace Impacto {
namespace Input {
enum class Device { Mouse, Keyboard, Touch, Controller };

inline float constexpr ControllerAxisLightThreshold = 0.3f;
inline float constexpr ControllerAxisHeavyThreshold = 0.8f;

inline int constexpr MouseButtonsMax = SDL_BUTTON_X2 + 1;
inline int constexpr FingerTapMax = 3;

void BeginFrame();
bool HandleEvent(SDL_Event const* ev);
void ClearFlicks();
void ClearPinchGesture();
void ClearTouch();

inline Device CurrentInputDevice = Device::Mouse;

inline glm::vec2 InitMousePos = glm::vec2(0.0f);
inline glm::vec2 PrevMousePos = glm::vec2(0.0f);
inline glm::vec2 CurMousePos = glm::vec2(0.0f);

inline float MouseWheelDeltaX = 0;
inline float MouseWheelDeltaY = 0;

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

inline int8_t TouchTapCount = 0;
inline bool TouchHeldDown = false;
inline bool TouchFlickLeft = false;
inline bool TouchFlickRight = false;
inline bool TouchFlickDown = false;
inline bool TouchFlickUp = false;
inline bool TouchPinchIn = false;

struct PinchState {
  std::chrono::nanoseconds StartTime;
  float CurrentScale = 1.0f;
  float CummulativeScale = 1.0f;
  glm::vec2 MidPoint = {};
};
inline std::optional<PinchState> CurrentPinch{};

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