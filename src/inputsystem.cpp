#include "inputsystem.h"
#include <SDL3/SDL_timer.h>
// #include "window.h"
#include <ankerl/unordered_dense.h>
#include "renderer/renderer.h"

#include "profile/game.h"
#include "log.h"

namespace Impacto {
namespace Input {
static constexpr std::chrono::milliseconds LongPressTime(500);
// nonscaled pixels
static constexpr float MaxTapSlop = 8.0f;
// nonscaled pixels per second
static constexpr float FlingMinVelocity = 50.0f;
static constexpr float FlingMaxVelocity = 5000.0f;
static constexpr float PinchInMaxScale = 0.7f;
static constexpr std::chrono::milliseconds PinchInMaxTime(500);
static constexpr std::chrono::milliseconds TapGroupWindow(80);

struct TouchState {
  SDL_FingerID FingerId;
  glm::vec2 StartPos;
  glm::vec2 LastPos;
  std::chrono::nanoseconds StartTime;
  bool Tappable;
  bool Flickable;
};
static std::array<std::optional<TouchState>, 3> CurrentFingers{};

struct PendingTapGroup {
  std::chrono::nanoseconds FirstUpTime;
  int8_t Count = 0;
};
static std::optional<PendingTapGroup> PendingTaps;
static bool NoPinchGesture = false;

void BeginFrame() {
  using std::chrono::nanoseconds;
  memset(ControllerButtonWentDown, false, sizeof(ControllerButtonWentDown));
  memset(ControllerAxisWentDownLight, false,
         sizeof(ControllerAxisWentDownLight));
  memset(ControllerAxisWentDownHeavy, false,
         sizeof(ControllerAxisWentDownHeavy));
  memset(MouseButtonWentDown, false, sizeof(MouseButtonWentDown));
  memset(KeyboardButtonWentDown, false, sizeof(KeyboardButtonWentDown));
  PrevMousePos = CurMousePos;

  MouseWheelDeltaX = MouseWheelDeltaY = 0;

  TouchTapCount = 0;
  if (PendingTaps && nanoseconds(SDL_GetTicksNS()) - PendingTaps->FirstUpTime >
                         TapGroupWindow) {
    TouchTapCount = PendingTaps->Count;
    PendingTaps.reset();
  }
  const bool isTouchHeld = CurrentFingers[0].has_value() &&
                           ((CurrentFingers[0]->StartTime + LongPressTime <=
                             nanoseconds(SDL_GetTicksNS())) ||
                            !CurrentFingers[0]->Tappable);
  if (!TouchHeldDown && isTouchHeld) {
    CurrentFingers[0]->Tappable = false;
    InitMousePos = CurMousePos;
  }
  TouchHeldDown = isTouchHeld;
  TouchFlickLeft = false;
  TouchFlickRight = false;
  TouchFlickDown = false;
  TouchFlickUp = false;
  TouchPinchIn = false;
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

static void HandleTaps(SDL_TouchFingerEvent const& evt, bool tappable,
                       TouchState const& liftedFinger) {
  using namespace std::chrono;
  const nanoseconds now = nanoseconds(evt.timestamp);
  const nanoseconds elapsedTime = now - liftedFinger.StartTime;
  if (tappable && elapsedTime < LongPressTime) {
    if (!PendingTaps || (now - PendingTaps->FirstUpTime) > TapGroupWindow) {
      PendingTaps = PendingTapGroup{now, 1};
    } else {
      PendingTaps->Count++;
    }
    if (PendingTaps->Count == 1) {
      CurMousePos = liftedFinger.StartPos;
      InitMousePos = liftedFinger.StartPos;
    }
  }
}

static void HandleFlicks(SDL_TouchFingerEvent const& evt, bool tappable,
                         TouchState const& liftedFinger) {
  using namespace std::chrono;
  using SecondsFlt = duration<float>;

  const nanoseconds elapsedTime =
      nanoseconds(evt.timestamp) - liftedFinger.StartTime;

  const glm::vec2 liftedPos =
      SDLMouseCoordsToDesign((int)(evt.x * (float)Window->WindowWidth),
                             (int)(evt.y * (float)Window->WindowHeight));

  const glm::vec2 deltaPos = liftedPos - liftedFinger.StartPos;
  const glm::vec2 deltaPosPerSecond =
      deltaPos / duration_cast<SecondsFlt>(elapsedTime).count();
  const glm::vec2 deltaPosPerSecondAbs = glm::abs(deltaPosPerSecond);

  if (deltaPosPerSecondAbs.x > deltaPosPerSecondAbs.y &&
      deltaPosPerSecondAbs.x >= FlingMinVelocity * Window->DpiScale &&
      deltaPosPerSecondAbs.x <= FlingMaxVelocity * Window->DpiScale) {
    if (deltaPosPerSecond.x < 0)
      TouchFlickLeft = true;
    else if (deltaPosPerSecond.x > 0)
      TouchFlickRight = true;
  }
  if (deltaPosPerSecondAbs.y > deltaPosPerSecondAbs.x &&
      deltaPosPerSecondAbs.y >= FlingMinVelocity * Window->DpiScale &&
      deltaPosPerSecondAbs.y <= FlingMaxVelocity * Window->DpiScale) {
    if (deltaPosPerSecond.y < 0)
      TouchFlickUp = true;
    else if (deltaPosPerSecond.y > 0)
      TouchFlickDown = true;
  }
}

bool HandleEvent(SDL_Event const* ev) {
  switch (ev->type) {
    case SDL_EVENT_GAMEPAD_ADDED: {
      SDL_GamepadDeviceEvent const* evt = &ev->gdevice;
      CurrentInputDevice = Device::Controller;
      SDL_OpenGamepad(evt->which);
      return true;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      SDL_MouseMotionEvent const* evt = &ev->motion;
      CurMousePos = SDLMouseCoordsToDesign((int)evt->x, (int)evt->y);
      CurrentInputDevice = Device::Mouse;
      return true;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      SDL_MouseButtonEvent const* evt = &ev->button;
      if (evt->which == SDL_TOUCH_MOUSEID) return true;
      if (!MouseButtonIsDown[evt->button] && evt->down) {
        InitMousePos = SDLMouseCoordsToDesign((int)evt->x, (int)evt->y);
      }
      CurMousePos = SDLMouseCoordsToDesign((int)evt->x, (int)evt->y);
      CurrentInputDevice = Device::Mouse;
      MouseButtonWentDown[evt->button] =
          (evt->down && !MouseButtonIsDown[evt->button]);
      MouseButtonIsDown[evt->button] = evt->down;
      return true;
    }
    // TODO respect direction?
    case SDL_EVENT_MOUSE_WHEEL: {
      SDL_MouseWheelEvent const* evt = &ev->wheel;
      CurrentInputDevice = Device::Mouse;
      MouseWheelDeltaX += evt->x;
      MouseWheelDeltaY += evt->y;
      return true;
    }
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
      SDL_KeyboardEvent const* evt = &ev->key;
      CurrentInputDevice = Device::Keyboard;
      KeyboardButtonWentDown[evt->scancode] =
          (evt->down && !KeyboardButtonIsDown[evt->scancode]);
      KeyboardButtonIsDown[evt->scancode] = evt->down;
      return true;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP: {
      SDL_GamepadButtonEvent const* evt = &ev->gbutton;
      CurrentInputDevice = Device::Controller;
      ControllerButtonWentDown[evt->button] =
          (evt->down && !ControllerButtonIsDown[evt->button]);
      ControllerButtonIsDown[evt->button] = evt->down;
      return true;
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
    }
    case SDL_EVENT_FINGER_MOTION: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      int fingerCount = 0;
      TouchState* touchState = nullptr;
      for (auto& finger : CurrentFingers) {
        if (!finger.has_value()) continue;
        fingerCount++;
        if (finger->FingerId == evt->fingerID) {
          touchState = std::addressof(*finger);
          touchState->LastPos = SDLMouseCoordsToDesign(
              (int)(evt->x * (float)Window->WindowWidth),
              (int)(evt->y * (float)Window->WindowHeight));
          touchState->Tappable &=
              glm::distance(touchState->StartPos, touchState->LastPos) <=
              MaxTapSlop * Window->DpiScale;
        }
      }
      if (CurrentPinch.has_value()) {
        ClearFlicks();
        if (CurrentFingers[0] && CurrentFingers[1]) {
          CurrentPinch->MidPoint =
              (CurrentFingers[0]->LastPos + CurrentFingers[1]->LastPos) / 2.0f;
        }
      }
      if (touchState && fingerCount == 1) {
        CurMousePos = touchState->LastPos;
      }

      return true;
    }
    case SDL_EVENT_FINGER_DOWN: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;

      auto freeFingerSlot =
          std::find_if(CurrentFingers.begin(), CurrentFingers.end(),
                       [](std::optional<TouchState> const& finger) {
                         return !finger.has_value();
                       });
      if (freeFingerSlot == CurrentFingers.end()) {
        return true;
      }
      freeFingerSlot->emplace(TouchState{
          .FingerId = evt->fingerID,
          .StartPos = SDLMouseCoordsToDesign(
              (int)(evt->x * (float)Window->WindowWidth),
              (int)(evt->y * (float)Window->WindowHeight)),
          .LastPos = SDLMouseCoordsToDesign(
              (int)(evt->x * (float)Window->WindowWidth),
              (int)(evt->y * (float)Window->WindowHeight)),
          .StartTime = std::chrono::nanoseconds(evt->timestamp),
          .Tappable = true,
          .Flickable = true,
      });
      return true;
    }
    case SDL_EVENT_FINGER_UP: {
      SDL_TouchFingerEvent const* evt = &ev->tfinger;
      CurrentInputDevice = Device::Touch;
      auto liftedFingerItr = CurrentFingers.end();
      bool tappable = true;
      bool flickable = true;
      for (auto itr = CurrentFingers.begin(); itr != CurrentFingers.end();
           ++itr) {
        if (!itr->has_value()) continue;
        if ((*itr)->FingerId == evt->fingerID) liftedFingerItr = itr;

        tappable &= (*itr)->Tappable;
        flickable &= (*itr)->Flickable;
      }
      if (liftedFingerItr != CurrentFingers.end()) {
        if (tappable)
          HandleTaps(*evt, tappable, *(*liftedFingerItr));
        else if (!tappable && flickable) {
          HandleFlicks(*evt, flickable, *(*liftedFingerItr));
        }

        ImpLog(LogLevel::Debug, LogChannel::General,
               "Touch event: tap count: {}, flick left: {}, flick right: {}, "
               "flick up: {}, flick down: {}",
               TouchTapCount, TouchFlickLeft, TouchFlickRight, TouchFlickUp,
               TouchFlickDown);

        liftedFingerItr->reset();
        // Shift all fingers after the lifted one to the left
        std::rotate(liftedFingerItr, std::next(liftedFingerItr),
                    CurrentFingers.end());

        // Avoid sudden jump when lifting finger.
        if (CurrentFingers[0].has_value() && !CurrentFingers[1].has_value()) {
          CurMousePos = CurrentFingers[0]->LastPos;
          PrevMousePos = CurMousePos;
        }
      }
      return true;
    }
    case SDL_EVENT_PINCH_BEGIN: {
      SDL_PinchFingerEvent const* evt = &ev->pinch;
      CurrentInputDevice = Device::Touch;
      CurrentPinch.emplace(PinchState{
          .StartTime = std::chrono::nanoseconds(evt->timestamp),
      });
      return true;
    }
    case SDL_EVENT_PINCH_UPDATE: {
      SDL_PinchFingerEvent const* evt = &ev->pinch;
      CurrentInputDevice = Device::Touch;
      CurrentPinch.value().CurrentScale = evt->scale;
      CurrentPinch.value().CummulativeScale *= evt->scale;
      return true;
    }
    case SDL_EVENT_PINCH_END: {
      using namespace std::chrono;
      SDL_PinchFingerEvent const* evt = &ev->pinch;
      CurrentInputDevice = Device::Touch;
      const nanoseconds elapsedTime =
          nanoseconds(evt->timestamp) - CurrentPinch.value().StartTime;
      if (elapsedTime < PinchInMaxTime &&
          CurrentPinch->CummulativeScale <= PinchInMaxScale &&
          !NoPinchGesture) {
        TouchPinchIn = true;
      }
      CurrentPinch.reset();
      NoPinchGesture = false;
      return true;
    }
    default:
      return false;
  }
}

void ClearFlicks() {
  for (auto& finger : CurrentFingers) {
    if (!finger.has_value()) continue;
    finger->Flickable = false;
  }
}

void ClearPinchGesture() { NoPinchGesture = true; }

}  // namespace Input
}  // namespace Impacto