#include "inputsystem.h"
//#include "window.h"
#include "renderer/renderer.h"

#include "profile/game.h"

namespace Impacto {
namespace Input {

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
bool ControllerAxisIsDownLight[SDL_CONTROLLER_AXIS_MAX] = {0};
bool ControllerAxisWentDownLight[SDL_CONTROLLER_AXIS_MAX] = {0};
bool ControllerAxisIsDownHeavy[SDL_CONTROLLER_AXIS_MAX] = {0};
bool ControllerAxisWentDownHeavy[SDL_CONTROLLER_AXIS_MAX] = {0};
bool KeyboardButtonWentDown[SDL_NUM_SCANCODES] = {0};
bool KeyboardButtonIsDown[SDL_NUM_SCANCODES] = {0};

// TODO multitouch
bool TouchIsDown = false;
bool TouchWentDown = false;

static SDL_FingerID CurrentFinger = 0;

void BeginFrame() {
  memset(ControllerButtonWentDown, false, sizeof(ControllerButtonWentDown));
  memset(ControllerAxisWentDownLight, false,
         sizeof(ControllerAxisWentDownLight));
  memset(ControllerAxisWentDownHeavy, false,
         sizeof(ControllerAxisWentDownHeavy));
  memset(MouseButtonWentDown, false, sizeof(MouseButtonWentDown));
  memset(KeyboardButtonWentDown, false, sizeof(KeyboardButtonWentDown));
  TouchWentDown = false;

  PrevMousePos = CurMousePos;
  PrevTouchPos = CurTouchPos;

  MouseWheelDeltaX = MouseWheelDeltaY = 0;
}

void EndFrame() {
  if (CurrentInputDevice == IDEV_Mouse) {
    SDL_ShowCursor(SDL_ENABLE);
  } else {
    SDL_ShowCursor(SDL_DISABLE);
  }
}

static glm::vec2 SDLMouseCoordsToDesign(int x, int y) {
  RectF viewport = Window->GetViewport();
  glm::vec2 result;
  result.x = ((float)x *
              (Profile::DesignWidth / (viewport.Width * Window->DpiScaleX))) -
             (viewport.X * Window->DpiScaleX);
  result.y = ((float)y *
              (Profile::DesignHeight / (viewport.Height * Window->DpiScaleY))) +
             (viewport.Y * Window->DpiScaleY);
  return result;
}

bool HandleEvent(SDL_Event const* ev) {
  switch (ev->type) {
    case SDL_CONTROLLERDEVICEADDED: {
      SDL_ControllerDeviceEvent const* evt = &ev->cdevice;
      CurrentInputDevice = IDEV_Controller;
      SDL_GameControllerOpen(evt->which);
      return true;
      break;
    }
    case SDL_MOUSEMOTION: {
      SDL_MouseMotionEvent const* evt = &ev->motion;
      CurMousePos = SDLMouseCoordsToDesign(evt->x, evt->y);
      CurrentInputDevice = IDEV_Mouse;
      return true;
      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      SDL_MouseButtonEvent const* evt = &ev->button;
      CurMousePos = SDLMouseCoordsToDesign(evt->x, evt->y);
      CurrentInputDevice = IDEV_Mouse;
      MouseButtonWentDown[evt->button] =
          (evt->state == SDL_PRESSED && !MouseButtonIsDown[evt->button]);
      MouseButtonIsDown[evt->button] = evt->state == SDL_PRESSED;
      return true;
      break;
    }
    // TODO respect direction?
    case SDL_MOUSEWHEEL: {
      SDL_MouseWheelEvent const* evt = &ev->wheel;
      CurrentInputDevice = IDEV_Mouse;
      MouseWheelDeltaX += evt->x;
      MouseWheelDeltaY += evt->y;
      return true;
      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      SDL_KeyboardEvent const* evt = &ev->key;
      CurrentInputDevice = IDEV_Keyboard;
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
      CurrentInputDevice = IDEV_Controller;
      ControllerButtonWentDown[evt->button] =
          (evt->state == SDL_PRESSED && !ControllerButtonIsDown[evt->button]);
      ControllerButtonIsDown[evt->button] = evt->state == SDL_PRESSED;
      return true;
      break;
    }
    case SDL_CONTROLLERAXISMOTION: {
      SDL_ControllerAxisEvent const* evt = &ev->caxis;
      CurrentInputDevice = IDEV_Controller;
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
      CurrentInputDevice = IDEV_Touch;
      if (CurrentFinger == evt->fingerId && TouchIsDown) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)Window->WindowWidth),
                                   (int)(evt->y * (float)Window->WindowHeight));
      }
      return true;
      break;
    }
    case SDL_FINGERDOWN: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = IDEV_Touch;
      if (!TouchIsDown) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)Window->WindowWidth),
                                   (int)(evt->y * (float)Window->WindowHeight));
        CurrentFinger = evt->fingerId;
        TouchIsDown = true;
        TouchWentDown = true;
      }
      return true;
      break;
    }
    case SDL_FINGERUP: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = IDEV_Touch;
      if (CurrentFinger == evt->fingerId && TouchIsDown) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)Window->WindowWidth),
                                   (int)(evt->y * (float)Window->WindowHeight));
        TouchIsDown = false;
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