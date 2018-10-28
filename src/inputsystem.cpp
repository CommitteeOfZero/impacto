#include "inputsystem.h"
#include "window.h"

namespace Impacto {

void InputSystem::BeginFrame() {
  memset(ControllerButtonWentDown, false, sizeof(ControllerButtonWentDown));
  memset(MouseButtonWentDown, false, sizeof(MouseButtonWentDown));
  memset(KeyboardButtonWentDown, false, sizeof(KeyboardButtonWentDown));

  PrevMousePos = CurMousePos;
  PrevTouchPos = CurTouchPos;

  MouseWheelDeltaX = MouseWheelDeltaY = 0;
}

void InputSystem::EndFrame() {
  if (CurrentInputDevice == IDEV_Mouse) {
    SDL_ShowCursor(SDL_ENABLE);
  } else {
    SDL_ShowCursor(SDL_DISABLE);
  }
}

static glm::vec2 SDLMouseCoordsToDesign(int x, int y) {
  RectF viewport = WindowGetViewport();
  glm::vec2 result;
  result.x = ((float)x * (viewport.Width / g_DesignWidth)) - viewport.X;
  result.y = ((float)y * (viewport.Height / g_DesignHeight)) + viewport.Y;
  return result;
}

bool InputSystem::HandleEvent(SDL_Event const* ev) {
  switch (ev->type) {
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
      // TODO value range of triggers?
    case SDL_CONTROLLERAXISMOTION: {
      SDL_ControllerAxisEvent const* evt = &ev->caxis;
      CurrentInputDevice = IDEV_Controller;
      ControllerAxis[evt->axis] = (float)evt->value / (float)INT16_MAX;
      return true;
      break;
    }
    case SDL_FINGERMOTION: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = IDEV_Touch;
      if (CurrentFinger == evt->fingerId && TouchIsDown) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)g_WindowWidth),
                                   (int)(evt->y * (float)g_WindowHeight));
      }
      return true;
      break;
    }
    case SDL_FINGERDOWN: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = IDEV_Touch;
      if (!TouchIsDown) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)g_WindowWidth),
                                   (int)(evt->y * (float)g_WindowHeight));
        CurrentFinger = evt->fingerId;
        TouchIsDown = true;
      }
      return true;
      break;
    }
    case SDL_FINGERUP: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = IDEV_Touch;
      if (CurrentFinger == evt->fingerId && TouchIsDown) {
        CurTouchPos =
            SDLMouseCoordsToDesign((int)(evt->x * (float)g_WindowWidth),
                                   (int)(evt->y * (float)g_WindowHeight));
        TouchIsDown = false;
      }
      return true;
      break;
    }

    default:
      return false;
  }
}

}  // namespace Impacto