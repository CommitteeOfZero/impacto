#include "inputsystem.h"
#include <SDL3/SDL_timer.h>
// #include "window.h"
#include <ankerl/unordered_dense.h>
#include "renderer/renderer.h"

#include "profile/game.h"

namespace Impacto {
namespace Input {

static std::array<SDL_FingerID, 2> CurrentFingers{};

void BeginFrame() {
  memset(ControllerButtonWentDown, false, sizeof(ControllerButtonWentDown));
  memset(ControllerAxisWentDownLight, false,
         sizeof(ControllerAxisWentDownLight));
  memset(ControllerAxisWentDownHeavy, false,
         sizeof(ControllerAxisWentDownHeavy));
  memset(MouseButtonWentDown, false, sizeof(MouseButtonWentDown));
  memset(KeyboardButtonWentDown, false, sizeof(KeyboardButtonWentDown));
  TouchWentDown[0] = false;
  TouchWentDown[1] = false;

  PrevMousePos = CurMousePos;
  PrevTouchPos = CurTouchPos;

  MouseWheelDeltaX = MouseWheelDeltaY = 0;
}

static glm::vec2 SDLMouseCoordsToDesign(int x, int y) {
  RectF viewport =
      Window->GetLogicalViewport();  // aspect-fit box in logical/window space
  glm::vec2 result;
  result.x =
      ((float)x - viewport.X) * (Profile::Game::DesignWidth / viewport.Width);
  result.y =
      ((float)y - viewport.Y) * (Profile::Game::DesignHeight / viewport.Height);
  return result;
}

bool HandleEvent(SDL_Event const* ev) {
  switch (ev->type) {
    case SDL_EVENT_GAMEPAD_ADDED: {
      SDL_GamepadDeviceEvent const* evt = &ev->gdevice;
      CurrentInputDevice = Device::Controller;
      SDL_OpenGamepad(evt->which);
      return true;
      break;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      SDL_MouseMotionEvent const* evt = &ev->motion;
      CurMousePos = SDLMouseCoordsToDesign((int)evt->x, (int)evt->y);
      CurrentInputDevice = Device::Mouse;
      return true;
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      SDL_MouseButtonEvent const* evt = &ev->button;
      CurMousePos = SDLMouseCoordsToDesign((int)evt->x, (int)evt->y);
      CurrentInputDevice = Device::Mouse;
      MouseButtonWentDown[evt->button] =
          (evt->down && !MouseButtonIsDown[evt->button]);
      MouseButtonIsDown[evt->button] = evt->down;
      return true;
      break;
    }
    // TODO respect direction?
    case SDL_EVENT_MOUSE_WHEEL: {
      SDL_MouseWheelEvent const* evt = &ev->wheel;
      CurrentInputDevice = Device::Mouse;
      MouseWheelDeltaX += evt->x;
      MouseWheelDeltaY += evt->y;
      return true;
      break;
    }
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
      SDL_KeyboardEvent const* evt = &ev->key;
      CurrentInputDevice = Device::Keyboard;
      KeyboardButtonWentDown[evt->scancode] =
          (evt->down && !KeyboardButtonIsDown[evt->scancode]);
      KeyboardButtonIsDown[evt->scancode] = evt->down;
      return true;
      break;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP: {
      SDL_GamepadButtonEvent const* evt = &ev->gbutton;
      CurrentInputDevice = Device::Controller;
      ControllerButtonWentDown[evt->button] =
          (evt->down && !ControllerButtonIsDown[evt->button]);
      ControllerButtonIsDown[evt->button] = evt->down;
      return true;
      break;
    }
    case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
      SDL_GamepadAxisEvent const* evt = &ev->gaxis;
      float newVal = (float)evt->value / (float)INT16_MAX;
      float newWeight = fabsf(newVal);
      float oldWeight = fabsf(ControllerAxisValue[evt->axis]);
      const bool axisIsDownLight = newWeight >= ControllerAxisLightThreshold;
      const bool axisIsDownHeavy = newWeight >= ControllerAxisHeavyThreshold;
      if (oldWeight < ControllerAxisLightThreshold && axisIsDownLight) {
        ControllerAxisWentDownLight[evt->axis] = true;
      }

      if (oldWeight < ControllerAxisHeavyThreshold && axisIsDownHeavy) {
        ControllerAxisWentDownHeavy[evt->axis] = true;
      }

      if (axisIsDownLight) {
        CurrentInputDevice = Device::Controller;
      }

      ControllerAxisIsDownLight[evt->axis] = axisIsDownLight;
      ControllerAxisIsDownHeavy[evt->axis] = axisIsDownHeavy;
      ControllerAxisValue[evt->axis] = newVal;
      return true;
      break;
    }
    case SDL_EVENT_FINGER_MOTION: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      if (CurrentFingers[0] == evt->fingerID &&
          TouchIsDown[0 && TouchIsDown[1]]) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)Window->WindowWidth),
                                   (int)(evt->y * (float)Window->WindowHeight));
      }
      return true;
      break;
    }
    case SDL_EVENT_FINGER_DOWN: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      for (int8_t i = 0; i < FingerTapMax; ++i) {
        if (!TouchIsDown[i]) {
          CurTouchPos = SDLMouseCoordsToDesign(
              (int)(evt->x * (float)Window->WindowWidth),
              (int)(evt->y * (float)Window->WindowHeight));
          CurrentFingers[i] = evt->fingerID;
          TouchIsDown[i] = true;
          TouchWentDown[i] = true;
          break;
        }
      }
      return true;
      break;
    }
    case SDL_EVENT_FINGER_UP: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      for (int8_t i = 0; i < FingerTapMax; ++i) {
        if (CurrentFingers[i] == evt->fingerID && TouchIsDown[i]) {
          CurTouchPos = SDLMouseCoordsToDesign(
              (int)(evt->x * (float)Window->WindowWidth),
              (int)(evt->y * (float)Window->WindowHeight));
          TouchIsDown[i] = false;
        }
      }
      return true;
      break;
    }

    default:
      return false;
  }
}

}  // namespace Input
}  // namespace Impacto