#include "controlleraudio.h"
#include "../log.h"

#if !defined(IMPACTO_DISABLE_FFMPEG) && IMPACTO_HAVE_THREADS

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_endian.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_hidapi.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <opus/opus.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

namespace Impacto {
namespace Audio {
namespace ControllerAudio {

static constexpr uint16_t SonyVendorId = 0x054C;
static constexpr uint16_t DualShock4V2ProductId = 0x09CC;
static constexpr std::array<uint16_t, 3> DualShock4BluetoothProductIds = {
    0x05C4, DualShock4V2ProductId, 0x05C5};
static constexpr std::array<uint16_t, 2> DualShock4UsbAudioProductIds = {
    DualShock4V2ProductId, 0x0BA0};
static constexpr std::array<uint16_t, 2> DualSenseProductIds = {0x0CE6, 0x0DF2};

static constexpr int DualShock4SampleRate = 32000;
static constexpr int DualSenseUsbSampleRate = 48000;
static constexpr int DualSenseBluetoothSampleRate = 45000;
static constexpr int DualSenseOpusSampleRate = 48000;

static constexpr int SbcSamplesPerFrame = 128;
static constexpr size_t SbcFramesPerReport = 4;
static constexpr size_t DualShock4SamplesPerReport =
    static_cast<size_t>(SbcSamplesPerFrame) * SbcFramesPerReport;
static constexpr int64_t SbcTargetBitRate = 216000;
static constexpr uint64_t DualShock4BluetoothReportIntervalNs = 16'000'000ULL;
static constexpr int DualShock4BluetoothTailSilentReports = 3;
static constexpr int DualShock4UsbTailSilentReports = 4;

static constexpr size_t DualSenseOpusFrameSamples = 480;
static constexpr size_t DualSenseOpusFrameBytes = 200;
static constexpr opus_int32 DualSenseOpusBitRate = 160000;
static constexpr uint64_t DualSenseBluetoothReportIntervalNs = 10'666'667ULL;
static constexpr int DualSenseBluetoothTailSilentReports = 10;
static constexpr int DualSenseUsbTailSilentReports = 6;

static constexpr size_t UsbChunkSamples = 512;
static constexpr size_t MaxUsbAudioChannels = 4;
static constexpr size_t MaxChunkSamples = std::max(
    {DualShock4SamplesPerReport, DualSenseOpusFrameSamples, UsbChunkSamples});
static constexpr size_t UsbAudioTargetQueueMs = 32;
static constexpr uint64_t DeviceRetryCooldownMs = 2000;

static constexpr size_t DualShock4ControlReportSize = 78;
static constexpr size_t DualShock4StreamReportSize = 462;
static constexpr size_t DualShock4StreamReportHeaderSize = 6;
static constexpr size_t BluetoothReportCrcSize = 4;
static constexpr size_t DualShock4StreamReportPayloadMax =
    DualShock4StreamReportSize - DualShock4StreamReportHeaderSize -
    BluetoothReportCrcSize;
static constexpr size_t MaxSbcFrameSize =
    DualShock4StreamReportPayloadMax / SbcFramesPerReport;
static constexpr uint8_t BluetoothCrcOutputPrefix = 0xA2;

static constexpr uint8_t DualShock4AudioModeDisabled = 0x00;
static constexpr uint8_t DualShock4AudioModeSpeaker = 0xA0;
static constexpr uint8_t DualShock4AudioTargetInternalSpeaker = 0x02;
static constexpr uint8_t DualShock4AudioArmHandshake = 0x85;
static constexpr uint8_t DualShock4AudioDisarmHandshake = 0x00;
static constexpr uint8_t DualShock4MaxHardwareSpeakerVolume = 0x4F;

static constexpr size_t DualShock4UsbOutputReportSize = 32;
static constexpr uint8_t DualShock4UsbFlagSpeakerVolume = 0x80;
static constexpr size_t DualShock4UsbSpeakerVolumeOffset = 22;
static constexpr size_t DualShock4UsbSpeakerConfigOffset = 23;
static constexpr uint8_t DualShock4UsbAudioRouteReportId = 0xE0;
static constexpr uint8_t DualShock4UsbAudioRouteStereoHeadset = 0x00;
static constexpr uint8_t DualShock4UsbAudioRouteSpeaker = 0x03;

static constexpr size_t DualSenseUsbControlReportSize = 48;
static constexpr size_t DualSenseBluetoothControlReportSize = 78;
static constexpr size_t DualSenseBluetoothStreamReportSize = 334;
static constexpr uint8_t DualSenseBluetoothHeaderTag = 0x10;
static constexpr uint8_t DualSenseFlag0SpeakerVolume = 0x20;
static constexpr uint8_t DualSenseFlag0AudioControl = 0x80;
static constexpr uint8_t DualSenseFlag1AudioControl2 = 0x80;
static constexpr uint8_t DualSenseMaxHardwareSpeakerVolume = 0x64;
static constexpr uint8_t DualSenseAudioPathHeadset = 0x00;
static constexpr uint8_t DualSenseAudioPathSpeaker = 0x30;
static constexpr uint8_t DualSenseSpeakerPreGain = 0x03;
static constexpr uint8_t DualSensePacketHasLengthFlag = 0x80;
static constexpr uint8_t DualSensePacketIdSession = 0x11;
static constexpr uint8_t DualSensePacketIdSpeaker = 0x13;
static constexpr uint8_t DualSenseSessionPayloadSize = 7;
static constexpr uint8_t DualSenseMicSessionClosed = 0xFE;
static constexpr uint8_t DualSenseBluetoothAudioBufferLength = 48;

enum class ControllerType {
  None,
  DualShock4,
  DualSense,
};

enum class TransportMode {
  None,
  Bluetooth,
  Usb,
};

struct UsbAudioEndpoint {
  SDL_AudioDeviceID DeviceId = 0;
  int Channels = 0;
};

struct TargetDevice {
  ControllerType Type = ControllerType::None;
  TransportMode Mode = TransportMode::None;
  std::string HidPath;
  SDL_AudioDeviceID AudioDeviceId = 0;
  int UsbAudioChannels = 0;

  bool operator==(TargetDevice const&) const = default;
};

static std::mutex StateMutex;
static std::condition_variable StateCv;
static std::thread WorkerThread;

static TargetDevice Target;
static uint64_t DeviceGeneration = 0;
static int ActiveSampleRate = DualShock4SampleRate;
static int PlayedSampleRate = DualShock4SampleRate;
static std::deque<int16_t> PcmQueue;
static uint64_t QueueGeneration = 0;
static uint64_t PlayedSamples = 0;
static bool QueueFinished = false;
static bool PlaybackFinished = true;
static bool Paused = false;
static bool ShuttingDown = false;

static std::atomic<bool> DeviceReady = false;
static std::atomic<bool> DeviceListChanged = false;
static std::atomic<bool> DeviceError = false;
static std::atomic<float> OutputGain = 1.0f;

static bool Initialized = false;
static bool HidInitialized = false;
static bool AudioInitialized = false;
static bool WasEnabled = false;
static uint64_t NextRetryTicksMs = 0;

static SwrContext* Resampler = nullptr;
static int ResamplerInRate = 0;
static int ResamplerOutRate = 0;
static int ResamplerChannels = 0;
static int ResamplerBitDepth = 0;
static std::vector<int16_t> ResampleBuffer;

static int16_t ScaleSample(int16_t sample, float gain) {
  const float scaled =
      std::clamp(static_cast<float>(sample) * gain, -32768.0f, 32767.0f);
  return static_cast<int16_t>(std::lround(scaled));
}

static char const* ControllerName(ControllerType type) {
  return type == ControllerType::DualSense ? "DualSense" : "DualShock 4";
}

static int TargetSampleRateFor(ControllerType type, TransportMode mode) {
  if (type == ControllerType::DualSense) {
    return mode == TransportMode::Bluetooth ? DualSenseBluetoothSampleRate
                                            : DualSenseUsbSampleRate;
  }
  return DualShock4SampleRate;
}

static size_t ReportSamplesFor(ControllerType type, TransportMode mode) {
  if (mode == TransportMode::Usb) return UsbChunkSamples;
  if (type == ControllerType::DualSense) return DualSenseOpusFrameSamples;
  return DualShock4SamplesPerReport;
}

static int TailSilentReportsFor(ControllerType type, TransportMode mode) {
  if (type == ControllerType::DualSense) {
    return mode == TransportMode::Usb ? DualSenseUsbTailSilentReports
                                      : DualSenseBluetoothTailSilentReports;
  }
  return mode == TransportMode::Usb ? DualShock4UsbTailSilentReports
                                    : DualShock4BluetoothTailSilentReports;
}

static bool IsBluetoothHidDevice(SDL_hid_device_info const* dev) {
  if (!dev || !dev->path) return false;
  if (dev->bus_type == SDL_HID_API_BUS_BLUETOOTH) return true;
  if (dev->bus_type != SDL_HID_API_BUS_UNKNOWN) return false;

  return SDL_strcasestr(dev->path, "00001124") != nullptr ||
         SDL_strcasestr(dev->path, "bth") != nullptr ||
         SDL_strcasestr(dev->path, "bluetooth") != nullptr ||
         SDL_strcasestr(dev->path, "0002054c") != nullptr ||
         SDL_strcasestr(dev->path, "0001054c") != nullptr;
}

static bool IsUsbHidDevice(SDL_hid_device_info const* dev) {
  if (!dev || !dev->path) return false;
  if (dev->bus_type == SDL_HID_API_BUS_USB) return true;
  if (dev->bus_type != SDL_HID_API_BUS_UNKNOWN) return false;
  return !IsBluetoothHidDevice(dev);
}

static bool IsUsbAudioDualShock4Product(uint16_t productId) {
  return std::ranges::find(DualShock4UsbAudioProductIds, productId) !=
         DualShock4UsbAudioProductIds.end();
}

static bool IsDualSenseProduct(uint16_t productId) {
  return std::ranges::find(DualSenseProductIds, productId) !=
         DualSenseProductIds.end();
}

static UsbAudioEndpoint FindControllerUsbAudioDevice(ControllerType type) {
  if (!AudioInitialized || type == ControllerType::None) return {};

  int deviceCount = 0;
  SDL_AudioDeviceID* devices = SDL_GetAudioPlaybackDevices(&deviceCount);
  if (!devices) return {};

  UsbAudioEndpoint matched;
  for (int i = 0; i < deviceCount; ++i) {
    char const* name = SDL_GetAudioDeviceName(devices[i]);
    if (!name) continue;

    const bool hasDualSense = SDL_strcasestr(name, "DualSense") != nullptr;
    const bool hasDualShock = SDL_strcasestr(name, "DUALSHOCK") != nullptr;
    const bool hasWirelessController =
        SDL_strcasestr(name, "Wireless Controller") != nullptr;

    SDL_AudioSpec spec{};
    const bool hasSpec = SDL_GetAudioDeviceFormat(devices[i], &spec, nullptr);

    if (type == ControllerType::DualShock4) {
      if (hasDualSense || (!hasDualShock && !hasWirelessController)) continue;
      if (hasSpec && spec.channels != 2) continue;

      if (hasSpec && spec.freq == DualShock4SampleRate) {
        matched = {devices[i], 2};
        break;
      }
      if (matched.DeviceId == 0) {
        matched = {devices[i], 2};
      }
    } else if (type == ControllerType::DualSense) {
      if (hasDualShock || (!hasDualSense && !hasWirelessController)) continue;
      if (hasSpec) {
        if (hasDualSense) {
          if (spec.channels != 4 && spec.channels != 2) continue;
        } else if (spec.channels != 4) {
          continue;
        }
      } else if (!hasDualSense) {
        continue;
      }

      const int channels = (hasSpec && spec.channels == 2) ? 2 : 4;
      if (channels == 4 && hasSpec && spec.freq == DualSenseUsbSampleRate) {
        matched = {devices[i], 4};
        break;
      }
      if (matched.DeviceId == 0 || (channels == 4 && matched.Channels != 4)) {
        matched = {devices[i], channels};
      }
    }
  }
  SDL_free(devices);
  return matched;
}

static TargetDevice FindControllerTarget() {
  const UsbAudioEndpoint dualShock4UsbAudio =
      FindControllerUsbAudioDevice(ControllerType::DualShock4);
  const UsbAudioEndpoint dualSenseUsbAudio =
      FindControllerUsbAudioDevice(ControllerType::DualSense);

  int gamepadCount = 0;
  if (SDL_JoystickID* gamepads = SDL_GetGamepads(&gamepadCount)) {
    TargetDevice fallbackBluetooth;
    TargetDevice fallbackUsb;
    for (int i = 0; i < gamepadCount; ++i) {
      const SDL_GamepadType gamepadType = SDL_GetGamepadTypeForID(gamepads[i]);
      ControllerType controllerType = ControllerType::None;
      if (gamepadType == SDL_GAMEPAD_TYPE_PS4) {
        controllerType = ControllerType::DualShock4;
      } else if (gamepadType == SDL_GAMEPAD_TYPE_PS5) {
        controllerType = ControllerType::DualSense;
      } else {
        continue;
      }

      char const* padPath = SDL_GetGamepadPathForID(gamepads[i]);
      const uint16_t vendor = SDL_GetGamepadVendorForID(gamepads[i]);
      const uint16_t product = SDL_GetGamepadProductForID(gamepads[i]);
      if (vendor == 0 || product == 0) continue;

      if (controllerType == ControllerType::DualSense &&
          (vendor != SonyVendorId || !IsDualSenseProduct(product))) {
        continue;
      }

      UsbAudioEndpoint usbAudio;
      if (controllerType == ControllerType::DualShock4) {
        if (vendor == SonyVendorId && IsUsbAudioDualShock4Product(product)) {
          usbAudio = dualShock4UsbAudio;
        }
      } else {
        usbAudio = dualSenseUsbAudio;
      }

      if (SDL_hid_device_info* devs = SDL_hid_enumerate(vendor, product)) {
        TargetDevice exactMatch;
        for (SDL_hid_device_info* dev = devs; dev != nullptr; dev = dev->next) {
          if (IsBluetoothHidDevice(dev)) {
            if (padPath && SDL_strcasecmp(dev->path, padPath) == 0) {
              exactMatch = {controllerType, TransportMode::Bluetooth, dev->path,
                            0, 0};
              break;
            }
            if (fallbackBluetooth.Mode == TransportMode::None) {
              fallbackBluetooth = {controllerType, TransportMode::Bluetooth,
                                   dev->path, 0, 0};
            }
          } else if (usbAudio.DeviceId != 0 && IsUsbHidDevice(dev)) {
            if (padPath && SDL_strcasecmp(dev->path, padPath) == 0) {
              exactMatch = {controllerType, TransportMode::Usb, dev->path,
                            usbAudio.DeviceId, usbAudio.Channels};
              break;
            }
            if (fallbackUsb.Mode == TransportMode::None) {
              fallbackUsb = {controllerType, TransportMode::Usb, dev->path,
                             usbAudio.DeviceId, usbAudio.Channels};
            }
          }
        }
        SDL_hid_free_enumeration(devs);
        if (exactMatch.Mode != TransportMode::None) {
          SDL_free(gamepads);
          return exactMatch;
        }
      }
    }
    SDL_free(gamepads);
    if (fallbackBluetooth.Mode != TransportMode::None) return fallbackBluetooth;
    if (fallbackUsb.Mode != TransportMode::None) return fallbackUsb;
  }

  if (SDL_hid_device_info* devs = SDL_hid_enumerate(SonyVendorId, 0)) {
    TargetDevice matchedUsb;
    for (SDL_hid_device_info* dev = devs; dev != nullptr; dev = dev->next) {
      if (IsBluetoothHidDevice(dev)) {
        if (std::ranges::find(DualShock4BluetoothProductIds, dev->product_id) !=
            DualShock4BluetoothProductIds.end()) {
          TargetDevice matchedBluetooth = {ControllerType::DualShock4,
                                           TransportMode::Bluetooth, dev->path,
                                           0, 0};
          SDL_hid_free_enumeration(devs);
          return matchedBluetooth;
        }
        if (IsDualSenseProduct(dev->product_id)) {
          TargetDevice matchedBluetooth = {ControllerType::DualSense,
                                           TransportMode::Bluetooth, dev->path,
                                           0, 0};
          SDL_hid_free_enumeration(devs);
          return matchedBluetooth;
        }
      } else if (IsUsbHidDevice(dev) &&
                 matchedUsb.Mode == TransportMode::None) {
        if (dualShock4UsbAudio.DeviceId != 0 &&
            dev->product_id == DualShock4V2ProductId) {
          matchedUsb = {ControllerType::DualShock4, TransportMode::Usb,
                        dev->path, dualShock4UsbAudio.DeviceId,
                        dualShock4UsbAudio.Channels};
        } else if (dualSenseUsbAudio.DeviceId != 0 &&
                   IsDualSenseProduct(dev->product_id)) {
          matchedUsb = {ControllerType::DualSense, TransportMode::Usb,
                        dev->path, dualSenseUsbAudio.DeviceId,
                        dualSenseUsbAudio.Channels};
        }
      }
    }
    SDL_hid_free_enumeration(devs);
    return matchedUsb;
  }

  return {};
}

static void AppendBluetoothCrc32(uint8_t* report, size_t totalSize) {
  uint32_t crc = SDL_crc32(0, &BluetoothCrcOutputPrefix, 1);
  crc = SDL_crc32(crc, report, totalSize - BluetoothReportCrcSize);
  UnalignedWrite<uint32_t>(report + totalSize - BluetoothReportCrcSize,
                           SDL_Swap32LE(crc));
}

static bool SendDualShock4UsbVolumeReport(SDL_hid_device* device) {
  std::array<uint8_t, DualShock4UsbOutputReportSize> report{};
  report[0] = 0x05;
  report[1] = DualShock4UsbFlagSpeakerVolume;
  report[DualShock4UsbSpeakerVolumeOffset] = DualShock4MaxHardwareSpeakerVolume;
  report[DualShock4UsbSpeakerConfigOffset] = DualShock4AudioArmHandshake;
  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

static bool SendDualShock4UsbRouteReport(SDL_hid_device* device,
                                         uint8_t route) {
  std::array<uint8_t, 3> report = {DualShock4UsbAudioRouteReportId, 0x00,
                                   route};
  return SDL_hid_send_feature_report(device, report.data(), report.size()) >= 0;
}

static bool SendDualShock4BluetoothControlReport(SDL_hid_device* device,
                                                 bool enable) {
  std::array<uint8_t, DualShock4ControlReportSize> report{};
  report[0] = 0x11;
  report[1] = 0xC0;
  report[2] = enable ? DualShock4AudioModeSpeaker : DualShock4AudioModeDisabled;
  report[3] = enable ? 0xF3 : 0xF0;
  report[24] = enable ? DualShock4MaxHardwareSpeakerVolume : 0x00;
  report[25] =
      enable ? DualShock4AudioArmHandshake : DualShock4AudioDisarmHandshake;
  AppendBluetoothCrc32(report.data(), report.size());

  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

static bool SendDualShock4BluetoothStreamReport(
    SDL_hid_device* device, uint16_t frameNumber,
    std::array<std::array<uint8_t, MaxSbcFrameSize>, SbcFramesPerReport> const&
        sbcFrames,
    size_t frameSize) {
  std::array<uint8_t, DualShock4StreamReportSize> report{};
  report[0] = 0x17;
  report[1] = 0x40;
  report[2] = DualShock4AudioModeSpeaker;
  UnalignedWrite<uint16_t>(report.data() + 3, SDL_Swap16LE(frameNumber));
  report[5] = DualShock4AudioTargetInternalSpeaker;
  size_t offset = DualShock4StreamReportHeaderSize;
  for (auto const& frame : sbcFrames) {
    std::memcpy(report.data() + offset, frame.data(), frameSize);
    offset += frameSize;
  }
  AppendBluetoothCrc32(report.data(), report.size());

  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

static void FillDualSenseAudioPayload(uint8_t* common, bool enable) {
  common[0] = enable ? static_cast<uint8_t>(DualSenseFlag0SpeakerVolume |
                                            DualSenseFlag0AudioControl)
                     : DualSenseFlag0AudioControl;
  common[1] = DualSenseFlag1AudioControl2;
  common[5] = enable ? DualSenseMaxHardwareSpeakerVolume : 0x00;
  common[7] = enable ? DualSenseAudioPathSpeaker : DualSenseAudioPathHeadset;
  common[37] = enable ? DualSenseSpeakerPreGain : 0x00;
}

static bool SendDualSenseUsbControlReport(SDL_hid_device* device, bool enable) {
  std::array<uint8_t, DualSenseUsbControlReportSize> report{};
  report[0] = 0x02;
  FillDualSenseAudioPayload(report.data() + 1, enable);
  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

static bool SendDualSenseBluetoothControlReport(SDL_hid_device* device,
                                                uint8_t sequence, bool enable) {
  std::array<uint8_t, DualSenseBluetoothControlReportSize> report{};
  report[0] = 0x31;
  report[1] = static_cast<uint8_t>((sequence & 0x0F) << 4);
  report[2] = DualSenseBluetoothHeaderTag;
  FillDualSenseAudioPayload(report.data() + 3, enable);
  AppendBluetoothCrc32(report.data(), report.size());

  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

static bool SendDualSenseBluetoothStreamReport(
    SDL_hid_device* device, uint8_t sequence, uint8_t packetCounter,
    std::array<uint8_t, DualSenseOpusFrameBytes> const& opusFrame,
    size_t opusSize) {
  std::array<uint8_t, DualSenseBluetoothStreamReportSize> report{};
  report[0] = 0x35;
  report[1] = static_cast<uint8_t>((sequence & 0x0F) << 4);
  report[2] = DualSensePacketIdSession | DualSensePacketHasLengthFlag;
  report[3] = DualSenseSessionPayloadSize;
  report[4] = DualSenseMicSessionClosed;
  report[9] = DualSenseBluetoothAudioBufferLength;
  report[10] = packetCounter;
  report[11] = DualSensePacketIdSpeaker | DualSensePacketHasLengthFlag;
  report[12] = static_cast<uint8_t>(DualSenseOpusFrameBytes);
  std::memcpy(report.data() + 13, opusFrame.data(),
              std::min(opusSize, DualSenseOpusFrameBytes));
  AppendBluetoothCrc32(report.data(), report.size());

  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

class SbcEncoder {
 public:
  SbcEncoder() = default;
  ~SbcEncoder() { Shutdown(); }

  SbcEncoder(SbcEncoder const&) = delete;
  SbcEncoder& operator=(SbcEncoder const&) = delete;

  bool Init() {
    if (CodecCtx) return true;

    AVCodec const* codec = avcodec_find_encoder(AV_CODEC_ID_SBC);
    if (!codec) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "FFmpeg SBC encoder not available for DualShock 4 speaker\n");
      return false;
    }

    if (!Frame) {
      Frame = av_frame_alloc();
      if (!Frame) return false;
      Frame->nb_samples = SbcSamplesPerFrame;
      Frame->format = AV_SAMPLE_FMT_S16;
      Frame->sample_rate = DualShock4SampleRate;
      av_channel_layout_default(&Frame->ch_layout, 2);
      if (av_frame_get_buffer(Frame, 0) < 0) {
        Shutdown();
        return false;
      }
    }

    if (!Packet) {
      Packet = av_packet_alloc();
      if (!Packet) {
        Shutdown();
        return false;
      }
    }

    int actualSize = 0;
    if (!ProbeFrameSize(codec, actualSize) || actualSize <= 0 ||
        static_cast<size_t>(actualSize) > MaxSbcFrameSize) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "DualShock 4 SBC encoder produced unexpected frame size {:d}\n",
             actualSize);
      Shutdown();
      return false;
    }

    if (!OpenContext(codec)) {
      Shutdown();
      return false;
    }
    FrameSize = static_cast<size_t>(actualSize);
    return true;
  }

  size_t GetFrameSize() const { return FrameSize; }

  bool Reset() {
    if (FrameSize == 0) return Init();
    AVCodec const* codec = avcodec_find_encoder(AV_CODEC_ID_SBC);
    if (!codec) return false;
    return OpenContext(codec);
  }

  bool EncodeMonoFrame(int16_t const* monoSamples, float gain,
                       std::array<uint8_t, MaxSbcFrameSize>& outFrame) {
    if (!CodecCtx || !Frame || !Packet || FrameSize == 0) return false;
    if (av_frame_make_writable(Frame) < 0) return false;

    int16_t* dst = reinterpret_cast<int16_t*>(Frame->data[0]);
    for (size_t i = 0; i < static_cast<size_t>(SbcSamplesPerFrame); ++i) {
      const int16_t sample = ScaleSample(monoSamples[i], gain);
      dst[2 * i] = sample;
      dst[2 * i + 1] = sample;
    }

    if (avcodec_send_frame(CodecCtx, Frame) < 0) return false;
    if (avcodec_receive_packet(CodecCtx, Packet) < 0) return false;

    const bool ok = Packet->size == static_cast<int>(FrameSize);
    if (ok) {
      std::memcpy(outFrame.data(), Packet->data, FrameSize);
    }
    av_packet_unref(Packet);
    return ok;
  }

  void Shutdown() {
    if (CodecCtx) avcodec_free_context(&CodecCtx);
    if (Frame) av_frame_free(&Frame);
    if (Packet) av_packet_free(&Packet);
    FrameSize = 0;
  }

 private:
  bool OpenContext(AVCodec const* codec) {
    if (CodecCtx) avcodec_free_context(&CodecCtx);
    CodecCtx = avcodec_alloc_context3(codec);
    if (!CodecCtx) return false;

    CodecCtx->sample_rate = DualShock4SampleRate;
    CodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    av_channel_layout_default(&CodecCtx->ch_layout, 2);
    CodecCtx->bit_rate = SbcTargetBitRate;
    av_opt_set_int(CodecCtx->priv_data, "sbc_delay", 13000, 0);

    if (avcodec_open2(CodecCtx, codec, nullptr) < 0 ||
        CodecCtx->frame_size != SbcSamplesPerFrame) {
      avcodec_free_context(&CodecCtx);
      return false;
    }
    return true;
  }

  bool ProbeFrameSize(AVCodec const* codec, int& outSize) {
    if (!OpenContext(codec)) return false;
    if (av_frame_make_writable(Frame) < 0) return false;
    std::memset(Frame->data[0], 0,
                static_cast<size_t>(SbcSamplesPerFrame) * 2 * sizeof(int16_t));
    if (avcodec_send_frame(CodecCtx, Frame) < 0) return false;
    if (avcodec_receive_packet(CodecCtx, Packet) < 0) return false;
    outSize = Packet->size;
    av_packet_unref(Packet);
    return true;
  }

  AVCodecContext* CodecCtx = nullptr;
  AVFrame* Frame = nullptr;
  AVPacket* Packet = nullptr;
  size_t FrameSize = 0;
};

class OpusFrameEncoder {
 public:
  OpusFrameEncoder() = default;
  ~OpusFrameEncoder() { Shutdown(); }

  OpusFrameEncoder(OpusFrameEncoder const&) = delete;
  OpusFrameEncoder& operator=(OpusFrameEncoder const&) = delete;

  bool Init() {
    if (Encoder) return true;

    int err = OPUS_OK;
    Encoder = opus_encoder_create(DualSenseOpusSampleRate, 2,
                                  OPUS_APPLICATION_AUDIO, &err);
    if (err != OPUS_OK || !Encoder) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "Failed to create Opus encoder for DualSense speaker ({:d})\n",
             err);
      Shutdown();
      return false;
    }

    if (opus_encoder_ctl(Encoder, OPUS_SET_BITRATE(DualSenseOpusBitRate)) !=
            OPUS_OK ||
        opus_encoder_ctl(Encoder, OPUS_SET_VBR(0)) != OPUS_OK) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "Failed to configure Opus encoder for DualSense speaker\n");
      Shutdown();
      return false;
    }
    return true;
  }

  bool Reset() {
    if (!Encoder) return Init();
    return opus_encoder_ctl(Encoder, OPUS_RESET_STATE) == OPUS_OK;
  }

  bool EncodeMonoFrame(int16_t const* monoSamples, float gain,
                       std::array<uint8_t, DualSenseOpusFrameBytes>& outFrame,
                       size_t& outSize) {
    if (!Encoder) return false;

    std::array<int16_t, DualSenseOpusFrameSamples * 2> stereo{};
    for (size_t i = 0; i < DualSenseOpusFrameSamples; ++i) {
      const int16_t sample = ScaleSample(monoSamples[i], gain);
      stereo[2 * i] = sample;
      stereo[2 * i + 1] = sample;
    }

    const opus_int32 encoded = opus_encode(
        Encoder, stereo.data(), static_cast<int>(DualSenseOpusFrameSamples),
        outFrame.data(), static_cast<opus_int32>(outFrame.size()));
    if (encoded <= 0 || static_cast<size_t>(encoded) > outFrame.size()) {
      return false;
    }
    outSize = static_cast<size_t>(encoded);
    return true;
  }

  void Shutdown() {
    if (Encoder) {
      opus_encoder_destroy(Encoder);
      Encoder = nullptr;
    }
  }

 private:
  OpusEncoder* Encoder = nullptr;
};

static bool DeviceEventCallback(void* userdata, SDL_Event* event) {
  const bool isGamepadEvent = event->type == SDL_EVENT_GAMEPAD_ADDED ||
                              event->type == SDL_EVENT_GAMEPAD_REMOVED;
  const bool isPlaybackAudioEvent =
      (event->type == SDL_EVENT_AUDIO_DEVICE_ADDED ||
       event->type == SDL_EVENT_AUDIO_DEVICE_REMOVED) &&
      !event->adevice.recording;
  if (isGamepadEvent || isPlaybackAudioEvent) {
    DeviceListChanged.store(true, std::memory_order_release);
  }
  return true;
}

static AVSampleFormat ToAvSampleFormat(int bitDepth) {
  switch (bitDepth) {
    case 8:
      return AV_SAMPLE_FMT_U8;
    case 16:
      return AV_SAMPLE_FMT_S16;
    case 32:
      return AV_SAMPLE_FMT_FLT;
    default:
      return AV_SAMPLE_FMT_NONE;
  }
}

static void ResetResampler() {
  if (Resampler) swr_free(&Resampler);
  ResamplerInRate = 0;
  ResamplerOutRate = 0;
  ResamplerChannels = 0;
  ResamplerBitDepth = 0;
}

static bool EnsureResampler(int sampleRate, int channelCount, int bitDepth,
                            int outSampleRate) {
  if (Resampler && ResamplerInRate == sampleRate &&
      ResamplerOutRate == outSampleRate && ResamplerChannels == channelCount &&
      ResamplerBitDepth == bitDepth) {
    return true;
  }

  ResetResampler();
  const AVSampleFormat inFmt = ToAvSampleFormat(bitDepth);
  if (inFmt == AV_SAMPLE_FMT_NONE || sampleRate <= 0 || outSampleRate <= 0 ||
      channelCount <= 0) {
    return false;
  }

  AVChannelLayout outLayout{};
  AVChannelLayout inLayout{};
  av_channel_layout_default(&outLayout, 1);
  av_channel_layout_default(&inLayout, channelCount);

  const int rc = swr_alloc_set_opts2(&Resampler, &outLayout, AV_SAMPLE_FMT_S16,
                                     outSampleRate, &inLayout, inFmt,
                                     sampleRate, 0, nullptr);
  av_channel_layout_uninit(&outLayout);
  av_channel_layout_uninit(&inLayout);
  if (rc < 0 || !Resampler) {
    ResetResampler();
    return false;
  }
  if (swr_init(Resampler) < 0) {
    ResetResampler();
    return false;
  }

  ResamplerInRate = sampleRate;
  ResamplerOutRate = outSampleRate;
  ResamplerChannels = channelCount;
  ResamplerBitDepth = bitDepth;
  return true;
}

static void StreamWorkerLoop() {
  ControllerType openType = ControllerType::None;
  TransportMode openMode = TransportMode::None;
  int openSampleRate = DualShock4SampleRate;
  int openUsbChannels = 0;
  SDL_hid_device* hidDevice = nullptr;
  SDL_AudioStream* usbAudioStream = nullptr;
  uint64_t openDeviceGeneration = 0;
  bool armed = false;
  uint16_t dualShock4FrameNumber = 0;
  uint8_t dualSenseControlSequence = 0;
  uint8_t dualSenseStreamSequence = 0;
  uint8_t dualSensePacketCounter = 0;
  uint64_t activeQueueGeneration = 0;
  uint64_t nextReportTimeNs = 0;
  int tailRemaining = 0;
  SbcEncoder sbcEncoder;
  OpusFrameEncoder opusEncoder;

  auto disarmHid = [&]() -> bool {
    if (!hidDevice) return true;
    if (openType == ControllerType::DualSense) {
      if (openMode == TransportMode::Usb) {
        return SendDualSenseUsbControlReport(hidDevice, false);
      }
      const uint8_t seq = dualSenseControlSequence;
      dualSenseControlSequence =
          static_cast<uint8_t>((dualSenseControlSequence + 1) & 0x0F);
      return SendDualSenseBluetoothControlReport(hidDevice, seq, false);
    }
    if (openMode == TransportMode::Usb) {
      return SendDualShock4UsbRouteReport(hidDevice,
                                          DualShock4UsbAudioRouteStereoHeadset);
    }
    return SendDualShock4BluetoothControlReport(hidDevice, false);
  };

  auto closeDevice = [&](bool notifyError, bool tryDisarm) {
    if (usbAudioStream) {
      SDL_PauseAudioStreamDevice(usbAudioStream);
      SDL_ClearAudioStream(usbAudioStream);
      SDL_DestroyAudioStream(usbAudioStream);
      usbAudioStream = nullptr;
    }
    if (hidDevice) {
      if (armed && tryDisarm) {
        disarmHid();
      }
      SDL_hid_close(hidDevice);
      hidDevice = nullptr;
    }
    openType = ControllerType::None;
    openMode = TransportMode::None;
    openUsbChannels = 0;
    armed = false;
    tailRemaining = 0;
    DeviceReady.store(false, std::memory_order_release);
    if (notifyError) {
      DeviceError.store(true, std::memory_order_release);
    }
    {
      std::lock_guard<std::mutex> stateLock(StateMutex);
      PlaybackFinished = true;
    }
  };

  auto publishOpenDevice = [&](ControllerType type, TransportMode mode,
                               int sampleRate, int usbChannels) -> bool {
    std::lock_guard<std::mutex> stateLock(StateMutex);
    if (ShuttingDown || DeviceGeneration != openDeviceGeneration) {
      return false;
    }
    if (ActiveSampleRate != sampleRate) {
      PcmQueue.clear();
      if (PlayedSamples > 0 && PlayedSampleRate > 0) {
        PlayedSamples = (PlayedSamples * static_cast<uint64_t>(sampleRate)) /
                        static_cast<uint64_t>(PlayedSampleRate);
      }
      ActiveSampleRate = sampleRate;
      PlayedSampleRate = sampleRate;
    }
    openType = type;
    openMode = mode;
    openSampleRate = sampleRate;
    openUsbChannels = usbChannels;
    DeviceReady.store(true, std::memory_order_release);
    return true;
  };

  while (true) {
    std::unique_lock<std::mutex> lock(StateMutex);
    if (ShuttingDown) break;

    if (DeviceGeneration != openDeviceGeneration) {
      const TargetDevice newTarget = Target;
      const uint64_t newGen = DeviceGeneration;
      lock.unlock();

      closeDevice(false, true);
      openDeviceGeneration = newGen;

      if (newTarget.Type != ControllerType::None &&
          newTarget.Mode == TransportMode::Bluetooth) {
        hidDevice = SDL_hid_open_path(newTarget.HidPath.c_str());
        const bool encoderReady =
            hidDevice != nullptr &&
            (newTarget.Type == ControllerType::DualSense ? opusEncoder.Init()
                                                         : sbcEncoder.Init());
        if (hidDevice && encoderReady) {
          const int sampleRate =
              TargetSampleRateFor(newTarget.Type, newTarget.Mode);
          if (publishOpenDevice(newTarget.Type, newTarget.Mode, sampleRate,
                                0)) {
            if (newTarget.Type == ControllerType::DualSense) {
              ImpLog(LogLevel::Info, LogChannel::Audio,
                     "DualSense Bluetooth speaker ready\n");
            } else {
              ImpLog(
                  LogLevel::Info, LogChannel::Audio,
                  "DualShock 4 Bluetooth speaker ready (SBC frame size {:d})\n",
                  sbcEncoder.GetFrameSize());
            }
          } else {
            SDL_hid_close(hidDevice);
            hidDevice = nullptr;
          }
        } else {
          if (hidDevice) {
            SDL_hid_close(hidDevice);
            hidDevice = nullptr;
          }
          ImpLog(LogLevel::Warning, LogChannel::Audio,
                 "Failed to initialize {:s} Bluetooth speaker\n",
                 ControllerName(newTarget.Type));
        }
      } else if (newTarget.Type != ControllerType::None &&
                 newTarget.Mode == TransportMode::Usb) {
        const int sampleRate =
            TargetSampleRateFor(newTarget.Type, newTarget.Mode);
        hidDevice = SDL_hid_open_path(newTarget.HidPath.c_str());
        SDL_AudioSpec srcSpec{};
        srcSpec.format = SDL_AUDIO_S16;
        srcSpec.channels = newTarget.UsbAudioChannels;
        srcSpec.freq = sampleRate;
        if (hidDevice) {
          usbAudioStream = SDL_OpenAudioDeviceStream(
              newTarget.AudioDeviceId, &srcSpec, nullptr, nullptr);
        }
        if (hidDevice && usbAudioStream) {
          if (publishOpenDevice(newTarget.Type, newTarget.Mode, sampleRate,
                                newTarget.UsbAudioChannels)) {
            ImpLog(LogLevel::Info, LogChannel::Audio,
                   "{:s} USB speaker ready ({:d} ch, {:d} Hz)\n",
                   ControllerName(newTarget.Type), newTarget.UsbAudioChannels,
                   sampleRate);
          } else {
            SDL_DestroyAudioStream(usbAudioStream);
            usbAudioStream = nullptr;
            SDL_hid_close(hidDevice);
            hidDevice = nullptr;
          }
        } else {
          const std::string error = SDL_GetError();
          if (usbAudioStream) {
            SDL_DestroyAudioStream(usbAudioStream);
            usbAudioStream = nullptr;
          }
          if (hidDevice) {
            SDL_hid_close(hidDevice);
            hidDevice = nullptr;
          }
          ImpLog(LogLevel::Warning, LogChannel::Audio,
                 "Failed to initialize {:s} USB speaker: {:s}\n",
                 ControllerName(newTarget.Type), error);
        }
      }
      continue;
    }

    if (!armed) {
      StateCv.wait(lock, [&]() {
        return ShuttingDown || DeviceGeneration != openDeviceGeneration ||
               (QueueFinished && PcmQueue.empty() && !PlaybackFinished) ||
               (openMode != TransportMode::None && !Paused &&
                !PcmQueue.empty());
      });
      if (ShuttingDown || DeviceGeneration != openDeviceGeneration) {
        continue;
      }
      if (QueueFinished && PcmQueue.empty()) {
        PlaybackFinished = true;
        continue;
      }
      if (openMode == TransportMode::None || Paused || PcmQueue.empty()) {
        continue;
      }

      activeQueueGeneration = QueueGeneration;
      tailRemaining = 0;
      lock.unlock();

      if (openMode == TransportMode::Usb) {
        const bool streamReady = SDL_ClearAudioStream(usbAudioStream) &&
                                 SDL_ResumeAudioStreamDevice(usbAudioStream);
        bool hidArmed = false;
        if (streamReady) {
          if (openType == ControllerType::DualSense) {
            hidArmed = SendDualSenseUsbControlReport(hidDevice, true);
          } else {
            hidArmed = SendDualShock4UsbVolumeReport(hidDevice) &&
                       SendDualShock4UsbRouteReport(
                           hidDevice, DualShock4UsbAudioRouteSpeaker);
          }
        }
        if (!streamReady || !hidArmed) {
          ImpLog(LogLevel::Warning, LogChannel::Audio,
                 "Failed to arm {:s} USB speaker: {:s}\n",
                 ControllerName(openType), SDL_GetError());
          closeDevice(true, false);
          continue;
        }
      } else if (openType == ControllerType::DualSense) {
        const uint8_t seq = dualSenseControlSequence;
        dualSenseControlSequence =
            static_cast<uint8_t>((dualSenseControlSequence + 1) & 0x0F);
        if (!opusEncoder.Reset() ||
            !SendDualSenseBluetoothControlReport(hidDevice, seq, true)) {
          ImpLog(LogLevel::Warning, LogChannel::Audio,
                 "Failed to arm DualSense Bluetooth speaker\n");
          closeDevice(true, false);
          continue;
        }
        dualSenseStreamSequence = 0;
        dualSensePacketCounter = 0;
        nextReportTimeNs = SDL_GetTicksNS();
      } else {
        if (!sbcEncoder.Reset() ||
            !SendDualShock4BluetoothControlReport(hidDevice, true)) {
          ImpLog(LogLevel::Warning, LogChannel::Audio,
                 "Failed to arm DualShock 4 Bluetooth speaker\n");
          closeDevice(true, false);
          continue;
        }
        dualShock4FrameNumber = 0;
        nextReportTimeNs = SDL_GetTicksNS();
      }
      armed = true;
      continue;
    }

    const bool stopRequested =
        Paused || activeQueueGeneration != QueueGeneration;
    if (stopRequested ||
        (PcmQueue.empty() && QueueFinished && tailRemaining == 0)) {
      if (!stopRequested && activeQueueGeneration == QueueGeneration) {
        PlaybackFinished = true;
      }
      lock.unlock();
      if (openMode == TransportMode::Usb) {
        SDL_PauseAudioStreamDevice(usbAudioStream);
        SDL_ClearAudioStream(usbAudioStream);
      }
      if (!disarmHid()) {
        closeDevice(true, false);
      } else {
        armed = false;
      }
      continue;
    }

    if (openMode == TransportMode::Usb) {
      const int queuedBytes = SDL_GetAudioStreamQueued(usbAudioStream);
      if (queuedBytes < 0 || SDL_GetAudioStreamDevice(usbAudioStream) == 0) {
        lock.unlock();
        closeDevice(true, true);
        continue;
      }
      const size_t targetQueuedBytes =
          (static_cast<size_t>(openSampleRate) *
           static_cast<size_t>(openUsbChannels) * sizeof(int16_t) *
           UsbAudioTargetQueueMs) /
          1000;
      if (static_cast<size_t>(queuedBytes) >= targetQueuedBytes) {
        StateCv.wait_for(lock, std::chrono::milliseconds(4), [&]() {
          return ShuttingDown || Paused ||
                 activeQueueGeneration != QueueGeneration ||
                 DeviceGeneration != openDeviceGeneration;
        });
        continue;
      }
    }

    const size_t reportSamples = ReportSamplesFor(openType, openMode);
    if (PcmQueue.size() < reportSamples && !QueueFinished &&
        activeQueueGeneration == QueueGeneration) {
      StateCv.wait_for(lock, std::chrono::milliseconds(4), [&]() {
        return ShuttingDown || Paused ||
               activeQueueGeneration != QueueGeneration ||
               DeviceGeneration != openDeviceGeneration ||
               PcmQueue.size() >= reportSamples || QueueFinished;
      });
      if (ShuttingDown || Paused || activeQueueGeneration != QueueGeneration ||
          DeviceGeneration != openDeviceGeneration) {
        continue;
      }
    }

    std::array<int16_t, MaxChunkSamples> chunk{};
    size_t consumedSamples = 0;
    bool sendTailSilence = false;
    if (!PcmQueue.empty()) {
      consumedSamples = std::min(PcmQueue.size(), reportSamples);
      for (size_t i = 0; i < consumedSamples; ++i) {
        chunk[i] = PcmQueue.front();
        PcmQueue.pop_front();
      }
      if (PcmQueue.empty() && QueueFinished) {
        tailRemaining = TailSilentReportsFor(openType, openMode);
      }
    } else if (tailRemaining > 0) {
      --tailRemaining;
      sendTailSilence = true;
    }
    const float gain = OutputGain.load(std::memory_order_relaxed);
    const bool finishedAfterReport =
        PcmQueue.empty() && QueueFinished && tailRemaining == 0;
    lock.unlock();

    if (openMode == TransportMode::Usb) {
      const size_t usbSampleCount = consumedSamples > 0
                                        ? consumedSamples
                                        : (sendTailSilence ? reportSamples : 0);
      if (usbSampleCount == 0) {
        continue;
      }
      const size_t channels = static_cast<size_t>(openUsbChannels);
      std::array<int16_t, UsbChunkSamples * MaxUsbAudioChannels> usbChunk{};
      for (size_t i = 0; i < usbSampleCount; ++i) {
        usbChunk[i * channels + 1] = ScaleSample(chunk[i], gain);
      }
      const int chunkBytes =
          static_cast<int>(usbSampleCount * channels * sizeof(int16_t));
      if (!SDL_PutAudioStreamData(usbAudioStream, usbChunk.data(),
                                  chunkBytes)) {
        ImpLog(LogLevel::Warning, LogChannel::Audio,
               "{:s} USB speaker write failed: {:s}\n",
               ControllerName(openType), SDL_GetError());
        closeDevice(true, true);
        continue;
      }
    } else if (openType == ControllerType::DualSense) {
      std::array<uint8_t, DualSenseOpusFrameBytes> opusFrame{};
      size_t opusSize = 0;
      if (!opusEncoder.EncodeMonoFrame(chunk.data(), gain, opusFrame,
                                       opusSize)) {
        closeDevice(true, true);
        continue;
      }

      uint64_t nowNs = SDL_GetTicksNS();
      if (nowNs < nextReportTimeNs) {
        SDL_DelayPrecise(nextReportTimeNs - nowNs);
        nowNs = SDL_GetTicksNS();
      }
      nextReportTimeNs += DualSenseBluetoothReportIntervalNs;
      if (nextReportTimeNs < nowNs + DualSenseBluetoothReportIntervalNs / 2) {
        nextReportTimeNs = nowNs + DualSenseBluetoothReportIntervalNs;
      }

      if (!SendDualSenseBluetoothStreamReport(
              hidDevice, dualSenseStreamSequence, dualSensePacketCounter,
              opusFrame, opusSize)) {
        ImpLog(LogLevel::Warning, LogChannel::Audio,
               "DualSense Bluetooth speaker write failed\n");
        closeDevice(true, false);
        continue;
      }
      dualSenseStreamSequence =
          static_cast<uint8_t>((dualSenseStreamSequence + 1) & 0x0F);
      ++dualSensePacketCounter;
    } else {
      std::array<std::array<uint8_t, MaxSbcFrameSize>, SbcFramesPerReport>
          sbcFrames;
      bool encodedOk = true;
      for (size_t i = 0; i < SbcFramesPerReport; ++i) {
        if (!sbcEncoder.EncodeMonoFrame(
                chunk.data() + i * static_cast<size_t>(SbcSamplesPerFrame),
                gain, sbcFrames[i])) {
          encodedOk = false;
          break;
        }
      }
      if (!encodedOk) {
        closeDevice(true, true);
        continue;
      }

      uint64_t nowNs = SDL_GetTicksNS();
      if (nowNs < nextReportTimeNs) {
        SDL_DelayPrecise(nextReportTimeNs - nowNs);
        nowNs = SDL_GetTicksNS();
      }
      nextReportTimeNs += DualShock4BluetoothReportIntervalNs;
      if (nextReportTimeNs + DualShock4BluetoothReportIntervalNs * 4 < nowNs) {
        nextReportTimeNs = nowNs + DualShock4BluetoothReportIntervalNs;
      }

      if (!SendDualShock4BluetoothStreamReport(hidDevice, dualShock4FrameNumber,
                                               sbcFrames,
                                               sbcEncoder.GetFrameSize())) {
        ImpLog(LogLevel::Warning, LogChannel::Audio,
               "DualShock 4 Bluetooth speaker write failed\n");
        closeDevice(true, false);
        continue;
      }
      dualShock4FrameNumber =
          static_cast<uint16_t>(dualShock4FrameNumber + SbcFramesPerReport);
    }

    {
      std::lock_guard<std::mutex> postLock(StateMutex);
      if (activeQueueGeneration == QueueGeneration) {
        PlayedSamples += consumedSamples;
        PlayedSampleRate = openSampleRate;
        if (finishedAfterReport) {
          PlaybackFinished = true;
        }
      }
    }
  }

  closeDevice(false, true);
  sbcEncoder.Shutdown();
  opusEncoder.Shutdown();
}

void Init() {
  if (Initialized) return;
  Initialized = true;
  WasEnabled = false;
  NextRetryTicksMs = 0;
  DeviceReady.store(false, std::memory_order_release);
  DeviceListChanged.store(false, std::memory_order_release);
  DeviceError.store(false, std::memory_order_release);
  OutputGain.store(1.0f, std::memory_order_relaxed);

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    Target = {};
    DeviceGeneration = 0;
    ActiveSampleRate = DualShock4SampleRate;
    PlayedSampleRate = DualShock4SampleRate;
    PcmQueue.clear();
    QueueGeneration = 0;
    PlayedSamples = 0;
    QueueFinished = false;
    PlaybackFinished = true;
    Paused = false;
    ShuttingDown = false;
  }

  HidInitialized = SDL_hid_init() == 0;
  if (!HidInitialized) {
    ImpLog(LogLevel::Warning, LogChannel::Audio,
           "Failed to initialize SDL HID API for controller speaker\n");
    return;
  }

  AudioInitialized = SDL_InitSubSystem(SDL_INIT_AUDIO);
  if (!AudioInitialized) {
    ImpLog(LogLevel::Warning, LogChannel::Audio,
           "Failed to initialize SDL audio for USB controller speaker: {:s}\n",
           SDL_GetError());
  }

  SDL_AddEventWatch(DeviceEventCallback, nullptr);
  WorkerThread = std::thread(StreamWorkerLoop);
}

void Update(bool enabled) {
  if (!Initialized || !HidInitialized) return;

  if (!enabled) {
    DeviceListChanged.store(false, std::memory_order_release);
    DeviceError.store(false, std::memory_order_release);
    NextRetryTicksMs = 0;
    if (WasEnabled) {
      WasEnabled = false;
      ResetResampler();
      DeviceReady.store(false, std::memory_order_release);
      {
        std::lock_guard<std::mutex> lock(StateMutex);
        Target = {};
        ++DeviceGeneration;
        PcmQueue.clear();
        ++QueueGeneration;
        QueueFinished = false;
        PlaybackFinished = true;
        Paused = false;
      }
      StateCv.notify_all();
    }
    return;
  }

  bool rescan = !WasEnabled;
  WasEnabled = true;

  if (DeviceListChanged.exchange(false, std::memory_order_acq_rel)) {
    rescan = true;
    NextRetryTicksMs = 0;
  }

  if (DeviceError.exchange(false, std::memory_order_acq_rel)) {
    NextRetryTicksMs = SDL_GetTicks() + DeviceRetryCooldownMs;
  }

  if (NextRetryTicksMs != 0 && SDL_GetTicks() >= NextRetryTicksMs) {
    NextRetryTicksMs = 0;
    rescan = true;
  }

  if (!rescan) return;

  TargetDevice newTarget = FindControllerTarget();
  {
    std::lock_guard<std::mutex> lock(StateMutex);
    if (newTarget != Target || (newTarget.Mode != TransportMode::None &&
                                !DeviceReady.load(std::memory_order_acquire))) {
      Target = std::move(newTarget);
      ++DeviceGeneration;
      DeviceReady.store(false, std::memory_order_release);
    }
  }
  StateCv.notify_all();
}

void Shutdown() {
  if (!Initialized) return;
  Initialized = false;
  WasEnabled = false;
  NextRetryTicksMs = 0;

  if (HidInitialized) {
    SDL_RemoveEventWatch(DeviceEventCallback, nullptr);
    {
      std::lock_guard<std::mutex> lock(StateMutex);
      ShuttingDown = true;
      Target = {};
      ++DeviceGeneration;
      PcmQueue.clear();
      ++QueueGeneration;
      PlaybackFinished = true;
    }
    StateCv.notify_all();

    if (WorkerThread.joinable()) {
      WorkerThread.join();
    }
    SDL_hid_exit();
    HidInitialized = false;
  }

  if (AudioInitialized) {
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    AudioInitialized = false;
  }

  ResetResampler();
  ResampleBuffer.clear();
  DeviceReady.store(false, std::memory_order_release);
}

bool IsAvailable() { return DeviceReady.load(std::memory_order_acquire); }

bool QueuePcm(void const* data, int sampleCount, int sampleRate,
              int channelCount, int bitDepth) {
  if (!data || sampleCount <= 0 || !IsAvailable()) return false;

  int targetSampleRate = 0;
  {
    std::lock_guard<std::mutex> lock(StateMutex);
    if (!DeviceReady.load(std::memory_order_acquire)) return false;
    targetSampleRate = ActiveSampleRate;
  }

  if (!EnsureResampler(sampleRate, channelCount, bitDepth, targetSampleRate)) {
    return false;
  }

  const int outCapacity = swr_get_out_samples(Resampler, sampleCount);
  if (outCapacity <= 0) return false;

  if (ResampleBuffer.size() < static_cast<size_t>(outCapacity)) {
    ResampleBuffer.resize(static_cast<size_t>(outCapacity));
  }

  uint8_t* outPlanes[1] = {reinterpret_cast<uint8_t*>(ResampleBuffer.data())};
  uint8_t const* inPlanes[1] = {static_cast<uint8_t const*>(data)};
  const int converted =
      swr_convert(Resampler, outPlanes, outCapacity, inPlanes, sampleCount);
  if (converted <= 0) return false;

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    if (!DeviceReady.load(std::memory_order_acquire) ||
        ActiveSampleRate != targetSampleRate) {
      return false;
    }
    PcmQueue.insert(PcmQueue.end(), ResampleBuffer.begin(),
                    ResampleBuffer.begin() + converted);
    QueueFinished = false;
    PlaybackFinished = false;
  }
  StateCv.notify_all();
  return true;
}

void FinishQueue() {
  if (!Initialized) return;

  int flushed = 0;
  const int flushedSampleRate = ResamplerOutRate;
  if (Resampler) {
    const int delayed = swr_get_out_samples(Resampler, 0);
    if (delayed > 0) {
      if (ResampleBuffer.size() < static_cast<size_t>(delayed)) {
        ResampleBuffer.resize(static_cast<size_t>(delayed));
      }
      uint8_t* outPlanes[1] = {
          reinterpret_cast<uint8_t*>(ResampleBuffer.data())};
      flushed = swr_convert(Resampler, outPlanes, delayed, nullptr, 0);
    }
    ResetResampler();
  }

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    const bool ready = DeviceReady.load(std::memory_order_acquire);
    if (flushed > 0 && ready && ActiveSampleRate == flushedSampleRate) {
      PcmQueue.insert(PcmQueue.end(), ResampleBuffer.begin(),
                      ResampleBuffer.begin() + flushed);
      PlaybackFinished = false;
    }
    QueueFinished = true;
    if (!ready) {
      PlaybackFinished = true;
    }
  }
  StateCv.notify_all();
}

void StopPlayback() {
  if (!Initialized) return;
  ResetResampler();

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    PcmQueue.clear();
    QueueFinished = false;
    PlaybackFinished = true;
    Paused = false;
    PlayedSamples = 0;
    PlayedSampleRate = ActiveSampleRate;
    ++QueueGeneration;
  }
  StateCv.notify_all();
}

void SetPaused(bool paused) {
  if (!Initialized) return;
  bool changed = false;
  {
    std::lock_guard<std::mutex> lock(StateMutex);
    if (Paused != paused) {
      Paused = paused;
      changed = true;
    }
  }
  if (changed) StateCv.notify_all();
}

void SetGain(float gain) {
  OutputGain.store(std::max(0.0f, gain), std::memory_order_relaxed);
}

int GetQueuedDurationMs() {
  std::lock_guard<std::mutex> lock(StateMutex);
  if (ActiveSampleRate <= 0) return 0;
  return static_cast<int>((PcmQueue.size() * 1000ULL) /
                          static_cast<uint64_t>(ActiveSampleRate));
}

uint64_t GetPlayedSamples(int sampleRate) {
  if (sampleRate <= 0) return 0;
  std::lock_guard<std::mutex> lock(StateMutex);
  if (PlayedSampleRate <= 0) return 0;
  return (PlayedSamples * static_cast<uint64_t>(sampleRate)) /
         static_cast<uint64_t>(PlayedSampleRate);
}

bool IsFinished() {
  std::lock_guard<std::mutex> lock(StateMutex);
  return PlaybackFinished || !DeviceReady.load(std::memory_order_acquire);
}

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto

#else

namespace Impacto {
namespace Audio {
namespace ControllerAudio {

void Init() {}
void Update(bool) {}
void Shutdown() {}

bool IsAvailable() { return false; }

bool QueuePcm(void const*, int, int, int, int) { return false; }
void FinishQueue() {}
void StopPlayback() {}
void SetPaused(bool) {}
void SetGain(float) {}

int GetQueuedDurationMs() { return 0; }
uint64_t GetPlayedSamples(int) { return 0; }
bool IsFinished() { return true; }

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto

#endif
