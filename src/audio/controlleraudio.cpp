#include "controlleraudio.h"
#include "audiosystem.h"
#include "../log.h"

#include <SDL3/SDL_hidapi.h>

namespace Impacto {
namespace Audio {
namespace ControllerAudio {

static constexpr std::string_view DualShock4DeviceName = "Wireless Controller";
static constexpr std::string_view DualSenseDeviceName = "DualSense";

static constexpr uint16_t SonyVendorId = 0x054C;
static constexpr uint16_t DualShock4ProductId = 0x09CC;

static constexpr uint8_t DualShock4InputReportId = 0x01;
static constexpr int DualShock4StatusOffset = 30;
static constexpr uint8_t DualShock4HeadphonesBit = 0x20;

static constexpr float JackDebounceDuration = 0.25f;

static std::string ControllerDevice;
static bool ControllerEnabled = false;
static bool ControllerUsable = false;

static SDL_hid_device* DualShock4Device = nullptr;
static bool HeadphonesReported = false;
static bool HeadphonesPlugged = false;
static float JackStableTime = 0.0f;

static void CloseDualShock4() {
  if (!DualShock4Device) return;

  SDL_hid_close(DualShock4Device);
  SDL_hid_exit();
  DualShock4Device = nullptr;
  HeadphonesReported = false;
  HeadphonesPlugged = false;
  JackStableTime = 0.0f;
}

static void OpenDualShock4() {
  if (DualShock4Device || SDL_hid_init() != 0) return;

  DualShock4Device = SDL_hid_open(SonyVendorId, DualShock4ProductId, nullptr);
  if (!DualShock4Device) {
    ImpLog(LogLevel::Warning, LogChannel::Audio,
           "Could not open DualShock 4 to check its headphone jack\n");
    SDL_hid_exit();
    return;
  }
  SDL_hid_set_nonblocking(DualShock4Device, 1);
}

static void FindControllerDevice() {
  ControllerDevice.clear();
  if (ControllerEnabled) {
    for (std::string const& device : Backend->GetPlaybackDevices()) {
      if (device.find(DualShock4DeviceName) != std::string::npos &&
          device.find(DualSenseDeviceName) == std::string::npos) {
        ControllerDevice = device;
        break;
      }
    }
  }

  if (!ControllerDevice.empty()) {
    OpenDualShock4();
  } else {
    CloseDualShock4();
  }
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
  ControllerEnabled = false;
  ControllerUsable = false;
  FindControllerDevice();
}

void Update(float dt, bool enabled, bool deviceListChanged) {
  if (enabled != ControllerEnabled || deviceListChanged) {
    ControllerEnabled = enabled;
    FindControllerDevice();
  }

  ControllerUsable = !ControllerDevice.empty() && UpdateHeadphones(dt);
}

void Shutdown() { CloseDualShock4(); }

std::string const& GetDevice() {
  static const std::string noDevice;
  return ControllerUsable ? ControllerDevice : noDevice;
}

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto
