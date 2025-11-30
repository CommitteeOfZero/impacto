#include "inputsystem.h"
#include <SDL_timer.h>
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
  RectF viewport = Window->GetViewport();
  glm::vec2 result;
  result.x = ((float)x - viewport.X) *
             (Profile::DesignWidth / (viewport.Width * Window->DpiScaleX));
  result.y = ((float)y - viewport.Y) *
             (Profile::DesignHeight / (viewport.Height * Window->DpiScaleY));
  return result;
}

bool HandleEvent(SDL_Event const* ev) {
  switch (ev->type) {
    case SDL_CONTROLLERDEVICEADDED: {
      SDL_ControllerDeviceEvent const* evt = &ev->cdevice;
      CurrentInputDevice = Device::Controller;
      SDL_GameControllerOpen(evt->which);
      return true;
      break;
    }
    case SDL_MOUSEMOTION: {
      SDL_MouseMotionEvent const* evt = &ev->motion;
      CurMousePos = SDLMouseCoordsToDesign(evt->x, evt->y);
      CurrentInputDevice = Device::Mouse;
      return true;
      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      SDL_MouseButtonEvent const* evt = &ev->button;
      CurMousePos = SDLMouseCoordsToDesign(evt->x, evt->y);
      CurrentInputDevice = Device::Mouse;
      MouseButtonWentDown[evt->button] =
          (evt->state == SDL_PRESSED && !MouseButtonIsDown[evt->button]);
      MouseButtonIsDown[evt->button] = evt->state == SDL_PRESSED;
      return true;
      break;
    }
    // TODO respect direction?
    case SDL_MOUSEWHEEL: {
      SDL_MouseWheelEvent const* evt = &ev->wheel;
      CurrentInputDevice = Device::Mouse;
      MouseWheelDeltaX += evt->x;
      MouseWheelDeltaY += evt->y;
      return true;
      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      SDL_KeyboardEvent const* evt = &ev->key;
      CurrentInputDevice = Device::Keyboard;
      KeyboardButtonWentDown[evt->keysym.scancode] =
          (evt->state == SDL_PRESSED &&
           !KeyboardButtonIsDown[evt->keysym.scancode]);
      KeyboardButtonIsDown[evt->keysym.scancode] = evt->state == SDL_PRESSED;
      return true;
      break;
    }
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP: {
      SDL_ControllerButtonEvent const* evt = &ev->cbutton;
      CurrentInputDevice = Device::Controller;
      ControllerButtonWentDown[evt->button] =
          (evt->state == SDL_PRESSED && !ControllerButtonIsDown[evt->button]);
      ControllerButtonIsDown[evt->button] = evt->state == SDL_PRESSED;
      return true;
      break;
    }
    case SDL_CONTROLLERAXISMOTION: {
      SDL_ControllerAxisEvent const* evt = &ev->caxis;
      CurrentInputDevice = Device::Controller;
      float newVal = (float)evt->value / (float)INT16_MAX;
      float newWeight = fabsf(newVal);
      float oldWeight = fabsf(ControllerAxis[evt->axis]);
      if (oldWeight < ControllerAxisLightThreshold &&
          newWeight >= ControllerAxisLightThreshold) {
        ControllerAxisWentDownLight[evt->axis] = true;
      }
      if (oldWeight < ControllerAxisHeavyThreshold &&
          newWeight >= ControllerAxisHeavyThreshold) {
        ControllerAxisWentDownHeavy[evt->axis] = true;
      }
      ControllerAxisIsDownLight[evt->axis] =
          newWeight >= ControllerAxisLightThreshold;
      ControllerAxisIsDownHeavy[evt->axis] =
          newWeight >= ControllerAxisHeavyThreshold;
      ControllerAxis[evt->axis] = newVal;
      return true;
      break;
    }
    case SDL_FINGERMOTION: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      if (CurrentFingers[0] == evt->fingerId &&
          TouchIsDown[0 && TouchIsDown[1]]) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)Window->WindowWidth),
                                   (int)(evt->y * (float)Window->WindowHeight));
      }
      return true;
      break;
    }
    case SDL_FINGERDOWN: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      for (int8_t i = 0; i < FingerTapMax; ++i) {
        if (!TouchIsDown[i]) {
          CurTouchPos = SDLMouseCoordsToDesign(
              (int)(evt->x * (float)Window->WindowWidth),
              (int)(evt->y * (float)Window->WindowHeight));
          CurrentFingers[i] = evt->fingerId;
          TouchIsDown[i] = true;
          TouchWentDown[i] = true;
          break;
        }
      }
      return true;
      break;
    }
    case SDL_FINGERUP: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      for (int8_t i = 0; i < FingerTapMax; ++i) {
        if (CurrentFingers[i] == evt->fingerId && TouchIsDown[i]) {
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