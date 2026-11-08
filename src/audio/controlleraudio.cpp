#include "controlleraudio.h"
#include "audiosystem.h"
#include "../log.h"

#include <atomic>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_hidapi.h>

namespace Impacto {
namespace Audio {
namespace ControllerAudio {

static constexpr uint8_t DualShock4InputReportId = 0x01;
static constexpr int DualShock4StatusOffset = 30;
static constexpr uint8_t DualShock4HeadphonesBit = 0x20;

static constexpr float JackDebounceDuration = 0.25f;

static std::string ControllerDevice;
static bool ControllerEnabled = false;
static bool ControllerUsable = false;
static std::atomic<bool> GamepadListChanged = false;

static SDL_hid_device* DualShock4Device = nullptr;
static bool HeadphonesReported = false;
static bool HeadphonesPlugged = false;
static float JackStableTime = 0.0f;

static bool GamepadEventCallback(void* userdata, SDL_Event* event) {
  if (event->type == SDL_EVENT_GAMEPAD_ADDED ||
      event->type == SDL_EVENT_GAMEPAD_REMOVED) {
    GamepadListChanged = true;
  }
  return true;
}

static void CloseDualShock4() {
  if (!DualShock4Device) return;

  SDL_hid_close(DualShock4Device);
  SDL_hid_exit();
  DualShock4Device = nullptr;
  HeadphonesReported = false;
  HeadphonesPlugged = false;
  JackStableTime = 0.0f;
}

static std::string GetProductName(SDL_hid_device_info const* info) {
  char* productName = SDL_iconv_wchar_utf8(info->product_string);
  if (!productName) return {};

  std::string result = productName;
  SDL_free(productName);
  return result;
}

static std::string FindAudioDevice(SDL_hid_device* device) {
  SDL_hid_device_info* info = SDL_hid_get_device_info(device);
  if (!info || !info->product_string ||
      info->bus_type == SDL_HID_API_BUS_BLUETOOTH) {
    return {};
  }

  const std::string productName = GetProductName(info);
  if (productName.empty()) return {};

  const std::string bracketedName = fmt::format("({})", productName);
  for (std::string const& audioDevice : Backend->GetPlaybackDevices()) {
    if (audioDevice.starts_with(productName) ||
        audioDevice.find(bracketedName) != std::string::npos) {
      return audioDevice;
    }
  }
  return {};
}

static void FindControllerDevice() {
  if (ControllerEnabled && DualShock4Device) {
    ControllerDevice = FindAudioDevice(DualShock4Device);
    if (!ControllerDevice.empty()) return;
  }

  ControllerDevice.clear();
  CloseDualShock4();
  if (!ControllerEnabled || SDL_hid_init() != 0) return;

  int count = 0;
  if (SDL_JoystickID* ids = SDL_GetGamepads(&count)) {
    for (int i = 0; i < count && !DualShock4Device; i++) {
      if (SDL_GetRealGamepadTypeForID(ids[i]) != SDL_GAMEPAD_TYPE_PS4) continue;

      const char* path = SDL_GetGamepadPathForID(ids[i]);
      SDL_hid_device* device = path ? SDL_hid_open_path(path) : nullptr;
      if (!device) {
        ImpLog(LogLevel::Warning, LogChannel::Audio,
               "Could not open DualShock 4 to check its headphone jack\n");
        continue;
      }

      ControllerDevice = FindAudioDevice(device);
      if (ControllerDevice.empty()) {
        SDL_hid_close(device);
      } else {
        DualShock4Device = device;
      }
    }
    SDL_free(ids);
  }

  if (!DualShock4Device) {
    SDL_hid_exit();
    return;
  }
  SDL_hid_set_nonblocking(DualShock4Device, 1);
}

static bool UpdateHeadphones(float dt) {
  if (!DualShock4Device) return false;

  bool reported = HeadphonesReported;
  uint8_t report[64];
  while (true) {
    const int bytesRead =
        SDL_hid_read(DualShock4Device, report, sizeof(report));
    if (bytesRead == 0) break;
    if (bytesRead < 0) {
      CloseDualShock4();
      return false;
    }

    if (report[0] == DualShock4InputReportId &&
        bytesRead > DualShock4StatusOffset) {
      reported =
          (report[DualShock4StatusOffset] & DualShock4HeadphonesBit) != 0;
    }
  }

  if (reported != HeadphonesReported) {
    HeadphonesReported = reported;
    JackStableTime = 0.0f;
  }
  JackStableTime += dt;
  if (JackStableTime >= JackDebounceDuration) {
    HeadphonesPlugged = HeadphonesReported;
  }
  return HeadphonesPlugged;
}

void Init() {
  ControllerDevice.clear();
  ControllerEnabled = false;
  ControllerUsable = false;
  GamepadListChanged = false;
  SDL_AddEventWatch(GamepadEventCallback, nullptr);
}

void Update(float dt, bool enabled, bool deviceListChanged) {
  const bool gamepadListChanged = GamepadListChanged.exchange(false);
  if (enabled != ControllerEnabled || deviceListChanged || gamepadListChanged) {
    ControllerEnabled = enabled;
    FindControllerDevice();
  }

  ControllerUsable = !ControllerDevice.empty() && UpdateHeadphones(dt);
}

void Shutdown() {
  SDL_RemoveEventWatch(GamepadEventCallback, nullptr);
  CloseDualShock4();
}

std::string const& GetDevice() {
  static const std::string noDevice;
  return ControllerUsable ? ControllerDevice : noDevice;
}

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto
