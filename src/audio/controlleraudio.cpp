#include "controlleraudio.h"
#include "../log.h"

#ifndef IMPACTO_DISABLE_FFMPEG

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
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_hidapi.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

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
static constexpr std::array<uint16_t, 3> DualShock4ProductIds = {0x05C4, 0x09CC,
                                                                 0x05C5};

static constexpr int SpeakerSampleRate = 32000;
static constexpr int SbcSamplesPerFrame = 128;
static constexpr int SbcFramesPerReport = 4;
static constexpr int SamplesPerReport = SbcSamplesPerFrame * SbcFramesPerReport;
static constexpr int64_t SbcTargetBitRate = 216000;
static constexpr uint64_t ReportIntervalNs = 16'000'000ULL;
static constexpr int TailSilentReports = 3;
static constexpr uint64_t HidRetryCooldownMs = 2000;

static constexpr size_t ControlReportSize = 78;
static constexpr size_t StreamReportSize = 462;
static constexpr size_t StreamReportHeaderSize = 6;
static constexpr size_t StreamReportCrcSize = 4;
static constexpr size_t StreamReportPayloadMax =
    StreamReportSize - StreamReportHeaderSize - StreamReportCrcSize;
static constexpr size_t MaxSbcFrameSize =
    StreamReportPayloadMax / SbcFramesPerReport;
static constexpr uint8_t BtCrcOutputPrefix = 0xA2;

static constexpr uint8_t AudioModeDisabled = 0x00;
static constexpr uint8_t AudioModeSpeaker = 0xA0;
static constexpr uint8_t AudioTargetInternalSpeaker = 0x02;
static constexpr uint8_t AudioArmHandshake = 0x85;
static constexpr uint8_t AudioDisarmHandshake = 0x00;
static constexpr uint8_t MaxHardwareSpeakerVolume = 0x4F;

static std::mutex StateMutex;
static std::condition_variable StateCv;
static std::thread WorkerThread;

static std::string TargetHidPath;
static uint64_t DeviceGeneration = 0;
static std::deque<int16_t> PcmQueue;
static uint64_t QueueGeneration = 0;
static uint64_t PlayedSamples32k = 0;
static bool QueueFinished = false;
static bool PlaybackFinished = true;
static bool Paused = false;
static bool ShuttingDown = false;

static std::atomic<bool> DeviceReady = false;
static std::atomic<bool> GamepadListChanged = false;
static std::atomic<bool> HidError = false;
static std::atomic<float> OutputGain = 1.0f;

static bool Initialized = false;
static bool HidInitialized = false;
static bool WasEnabled = false;
static uint64_t NextRetryTicksMs = 0;

static SwrContext* Resampler = nullptr;
static int ResamplerRate = 0;
static int ResamplerChannels = 0;
static int ResamplerBitDepth = 0;
static std::vector<int16_t> ResampleBuffer;

static bool IsBluetoothHidDevice(SDL_hid_device_info const* dev) {
  if (!dev || !dev->path) return false;
  if (dev->bus_type == SDL_HID_API_BUS_BLUETOOTH) return true;
  if (dev->bus_type == SDL_HID_API_BUS_USB) return false;

  return SDL_strcasestr(dev->path, "00001124") != nullptr ||
         SDL_strcasestr(dev->path, "bth") != nullptr ||
         SDL_strcasestr(dev->path, "bluetooth") != nullptr ||
         SDL_strcasestr(dev->path, "0002054c") != nullptr ||
         SDL_strcasestr(dev->path, "0001054c") != nullptr;
}

static std::string FindBluetoothDualShock4Path() {
  int gamepadCount = 0;
  if (SDL_JoystickID* gamepads = SDL_GetGamepads(&gamepadCount)) {
    std::string fallbackPath;
    for (int i = 0; i < gamepadCount; ++i) {
      if (SDL_GetGamepadTypeForID(gamepads[i]) != SDL_GAMEPAD_TYPE_PS4) {
        continue;
      }
      char const* padPath = SDL_GetGamepadPathForID(gamepads[i]);
      const uint16_t vendor = SDL_GetGamepadVendorForID(gamepads[i]);
      const uint16_t product = SDL_GetGamepadProductForID(gamepads[i]);
      if (vendor == 0 || product == 0) continue;

      if (SDL_hid_device_info* devs = SDL_hid_enumerate(vendor, product)) {
        std::string exactPath;
        for (SDL_hid_device_info* dev = devs; dev != nullptr; dev = dev->next) {
          if (!IsBluetoothHidDevice(dev)) continue;
          if (padPath && SDL_strcasecmp(dev->path, padPath) == 0) {
            exactPath = dev->path;
            break;
          }
          if (fallbackPath.empty()) fallbackPath = dev->path;
        }
        SDL_hid_free_enumeration(devs);
        if (!exactPath.empty()) {
          SDL_free(gamepads);
          return exactPath;
        }
      }
    }
    SDL_free(gamepads);
    if (!fallbackPath.empty()) return fallbackPath;
  }

  if (SDL_hid_device_info* devs = SDL_hid_enumerate(SonyVendorId, 0)) {
    std::string matchedPath;
    for (SDL_hid_device_info* dev = devs; dev != nullptr; dev = dev->next) {
      if (std::find(DualShock4ProductIds.begin(), DualShock4ProductIds.end(),
                    dev->product_id) == DualShock4ProductIds.end()) {
        continue;
      }
      if (IsBluetoothHidDevice(dev)) {
        matchedPath = dev->path;
        break;
      }
    }
    SDL_hid_free_enumeration(devs);
    return matchedPath;
  }

  return {};
}

static void AppendBluetoothCrc32(uint8_t* report, size_t totalSize) {
  uint32_t crc = SDL_crc32(0, &BtCrcOutputPrefix, 1);
  crc = SDL_crc32(crc, report, totalSize - 4);
  report[totalSize - 4] = static_cast<uint8_t>(crc & 0xFFu);
  report[totalSize - 3] = static_cast<uint8_t>((crc >> 8) & 0xFFu);
  report[totalSize - 2] = static_cast<uint8_t>((crc >> 16) & 0xFFu);
  report[totalSize - 1] = static_cast<uint8_t>((crc >> 24) & 0xFFu);
}

static bool SendAudioControlReport(SDL_hid_device* device, bool enable) {
  std::array<uint8_t, ControlReportSize> report{};
  report[0] = 0x11;
  report[1] = 0xC0;
  report[2] = enable ? AudioModeSpeaker : AudioModeDisabled;
  report[3] = enable ? 0xF3 : 0xF0;
  report[21] = 0x00;
  report[22] = 0x00;
  report[23] = 0x00;
  report[24] = enable ? MaxHardwareSpeakerVolume : 0x00;
  report[25] = enable ? AudioArmHandshake : AudioDisarmHandshake;
  AppendBluetoothCrc32(report.data(), report.size());

  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

static bool SendAudioStreamReport(
    SDL_hid_device* device, uint16_t frameNumber,
    std::array<std::array<uint8_t, MaxSbcFrameSize>, SbcFramesPerReport> const&
        sbcFrames,
    size_t frameSize) {
  std::array<uint8_t, StreamReportSize> report{};
  report[0] = 0x17;
  report[1] = 0x40;
  report[2] = AudioModeSpeaker;
  report[3] = static_cast<uint8_t>(frameNumber & 0xFFu);
  report[4] = static_cast<uint8_t>((frameNumber >> 8) & 0xFFu);
  report[5] = AudioTargetInternalSpeaker;
  size_t offset = StreamReportHeaderSize;
  for (auto const& frame : sbcFrames) {
    std::memcpy(report.data() + offset, frame.data(), frameSize);
    offset += frameSize;
  }
  AppendBluetoothCrc32(report.data(), report.size());

  return SDL_hid_write(device, report.data(), report.size()) >= 0;
}

class SbcEncoder {
 public:
  ~SbcEncoder() { Shutdown(); }

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
      Frame->sample_rate = SpeakerSampleRate;
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
    if (!ProbeBitRate(codec, SbcTargetBitRate, actualSize) || actualSize <= 0 ||
        static_cast<size_t>(actualSize) > MaxSbcFrameSize) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "DualShock 4 SBC encoder produced unexpected frame size {:d}\n",
             actualSize);
      Shutdown();
      return false;
    }

    VerifiedBitRate = SbcTargetBitRate;
    FrameSize = static_cast<size_t>(actualSize);
    return OpenContext(codec, VerifiedBitRate);
  }

  size_t GetFrameSize() const { return FrameSize; }

  bool Reset() {
    if (VerifiedBitRate == 0) return Init();
    AVCodec const* codec = avcodec_find_encoder(AV_CODEC_ID_SBC);
    if (!codec) return false;
    return OpenContext(codec, VerifiedBitRate);
  }

  bool EncodeMonoFrame(int16_t const* monoSamples, float gain,
                       std::array<uint8_t, MaxSbcFrameSize>& outFrame) {
    if (!CodecCtx || !Frame || !Packet || FrameSize == 0) return false;
    if (av_frame_make_writable(Frame) < 0) return false;

    int16_t* dst = reinterpret_cast<int16_t*>(Frame->data[0]);
    for (int i = 0; i < SbcSamplesPerFrame; ++i) {
      const float scaled = std::clamp(static_cast<float>(monoSamples[i]) * gain,
                                      -32768.0f, 32767.0f);
      const int16_t sample = static_cast<int16_t>(std::lround(scaled));
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
    VerifiedBitRate = 0;
    FrameSize = 0;
  }

 private:
  bool OpenContext(AVCodec const* codec, int64_t bitRate) {
    if (CodecCtx) avcodec_free_context(&CodecCtx);
    CodecCtx = avcodec_alloc_context3(codec);
    if (!CodecCtx) return false;

    CodecCtx->sample_rate = SpeakerSampleRate;
    CodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    av_channel_layout_default(&CodecCtx->ch_layout, 2);
    CodecCtx->bit_rate = bitRate;
    av_opt_set_int(CodecCtx->priv_data, "sbc_delay", 13000, 0);

    if (avcodec_open2(CodecCtx, codec, nullptr) < 0 ||
        CodecCtx->frame_size != SbcSamplesPerFrame) {
      avcodec_free_context(&CodecCtx);
      return false;
    }
    return true;
  }

  bool ProbeBitRate(AVCodec const* codec, int64_t bitRate, int& outSize) {
    if (!OpenContext(codec, bitRate)) return false;
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
  int64_t VerifiedBitRate = 0;
  size_t FrameSize = 0;
};

static bool GamepadEventCallback(void* userdata, SDL_Event* event) {
  if (event->type == SDL_EVENT_GAMEPAD_ADDED ||
      event->type == SDL_EVENT_GAMEPAD_REMOVED) {
    GamepadListChanged.store(true, std::memory_order_release);
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
  ResamplerRate = 0;
  ResamplerChannels = 0;
  ResamplerBitDepth = 0;
}

static bool EnsureResampler(int sampleRate, int channelCount, int bitDepth) {
  if (Resampler && ResamplerRate == sampleRate &&
      ResamplerChannels == channelCount && ResamplerBitDepth == bitDepth) {
    return true;
  }

  ResetResampler();
  const AVSampleFormat inFmt = ToAvSampleFormat(bitDepth);
  if (inFmt == AV_SAMPLE_FMT_NONE || sampleRate <= 0 || channelCount <= 0) {
    return false;
  }

  AVChannelLayout outLayout{};
  AVChannelLayout inLayout{};
  av_channel_layout_default(&outLayout, 1);
  av_channel_layout_default(&inLayout, channelCount);

  const int rc = swr_alloc_set_opts2(&Resampler, &outLayout, AV_SAMPLE_FMT_S16,
                                     SpeakerSampleRate, &inLayout, inFmt,
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

  ResamplerRate = sampleRate;
  ResamplerChannels = channelCount;
  ResamplerBitDepth = bitDepth;
  return true;
}

static void StreamWorkerLoop() {
  SDL_hid_device* hidDevice = nullptr;
  uint64_t openDeviceGeneration = 0;
  bool armed = false;
  uint16_t frameNumber = 0;
  uint64_t activeQueueGeneration = 0;
  uint64_t nextReportTimeNs = 0;
  int tailRemaining = 0;
  SbcEncoder encoder;

  auto closeDevice = [&](bool notifyError) {
    if (hidDevice) {
      if (armed && !notifyError) {
        SendAudioControlReport(hidDevice, false);
      }
      SDL_hid_close(hidDevice);
      hidDevice = nullptr;
    }
    armed = false;
    tailRemaining = 0;
    DeviceReady.store(false, std::memory_order_release);
    if (notifyError) {
      HidError.store(true, std::memory_order_release);
    }
    {
      std::lock_guard<std::mutex> stateLock(StateMutex);
      PlaybackFinished = true;
    }
  };

  while (true) {
    std::unique_lock<std::mutex> lock(StateMutex);
    if (ShuttingDown) break;

    if (DeviceGeneration != openDeviceGeneration) {
      const std::string newPath = TargetHidPath;
      const uint64_t newGen = DeviceGeneration;
      lock.unlock();

      closeDevice(false);
      openDeviceGeneration = newGen;

      if (!newPath.empty()) {
        hidDevice = SDL_hid_open_path(newPath.c_str());
        if (hidDevice && encoder.Init()) {
          DeviceReady.store(true, std::memory_order_release);
          ImpLog(LogLevel::Info, LogChannel::Audio,
                 "DualShock 4 Bluetooth speaker ready (SBC frame size {:d})\n",
                 encoder.GetFrameSize());
        } else {
          if (hidDevice) {
            SDL_hid_close(hidDevice);
            hidDevice = nullptr;
          }
          ImpLog(LogLevel::Warning, LogChannel::Audio,
                 "Failed to initialize DualShock 4 Bluetooth speaker\n");
        }
      }
      continue;
    }

    if (!armed) {
      StateCv.wait(lock, [&]() {
        return ShuttingDown || DeviceGeneration != openDeviceGeneration ||
               (QueueFinished && PcmQueue.empty() && !PlaybackFinished) ||
               (hidDevice && !Paused && !PcmQueue.empty());
      });
      if (ShuttingDown || DeviceGeneration != openDeviceGeneration) {
        continue;
      }
      if (QueueFinished && PcmQueue.empty()) {
        PlaybackFinished = true;
        continue;
      }
      if (!hidDevice || Paused || PcmQueue.empty()) continue;

      activeQueueGeneration = QueueGeneration;
      tailRemaining = 0;
      lock.unlock();

      if (!encoder.Reset() || !SendAudioControlReport(hidDevice, true)) {
        ImpLog(LogLevel::Warning, LogChannel::Audio,
               "Failed to arm DualShock 4 Bluetooth speaker\n");
        closeDevice(true);
        continue;
      }
      armed = true;
      frameNumber = 0;
      nextReportTimeNs = SDL_GetTicksNS();
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
      if (!SendAudioControlReport(hidDevice, false)) {
        closeDevice(true);
      } else {
        armed = false;
      }
      continue;
    }

    if (PcmQueue.size() < SamplesPerReport && !QueueFinished &&
        activeQueueGeneration == QueueGeneration) {
      StateCv.wait_for(lock, std::chrono::milliseconds(4), [&]() {
        return ShuttingDown || Paused ||
               activeQueueGeneration != QueueGeneration ||
               DeviceGeneration != openDeviceGeneration ||
               PcmQueue.size() >= SamplesPerReport || QueueFinished;
      });
      if (ShuttingDown || Paused || activeQueueGeneration != QueueGeneration ||
          DeviceGeneration != openDeviceGeneration) {
        continue;
      }
    }

    std::array<int16_t, SamplesPerReport> chunk{};
    size_t consumedSamples = 0;
    if (!PcmQueue.empty()) {
      consumedSamples =
          std::min(PcmQueue.size(), static_cast<size_t>(SamplesPerReport));
      for (size_t i = 0; i < consumedSamples; ++i) {
        chunk[i] = PcmQueue.front();
        PcmQueue.pop_front();
      }
      if (PcmQueue.empty() && QueueFinished) {
        tailRemaining = TailSilentReports;
      }
    } else if (tailRemaining > 0) {
      --tailRemaining;
    }
    const float gain = OutputGain.load(std::memory_order_relaxed);
    const bool finishedAfterReport =
        PcmQueue.empty() && QueueFinished && tailRemaining == 0;
    lock.unlock();

    std::array<std::array<uint8_t, MaxSbcFrameSize>, SbcFramesPerReport>
        sbcFrames;
    bool encodedOk = true;
    for (int f = 0; f < SbcFramesPerReport; ++f) {
      if (!encoder.EncodeMonoFrame(chunk.data() + f * SbcSamplesPerFrame, gain,
                                   sbcFrames[static_cast<size_t>(f)])) {
        encodedOk = false;
        break;
      }
    }
    if (!encodedOk) {
      closeDevice(true);
      continue;
    }

    uint64_t nowNs = SDL_GetTicksNS();
    if (nowNs < nextReportTimeNs) {
      SDL_DelayPrecise(nextReportTimeNs - nowNs);
      nowNs = SDL_GetTicksNS();
    }
    nextReportTimeNs += ReportIntervalNs;
    if (nextReportTimeNs + ReportIntervalNs * 4 < nowNs) {
      nextReportTimeNs = nowNs + ReportIntervalNs;
    }

    if (!SendAudioStreamReport(hidDevice, frameNumber, sbcFrames,
                               encoder.GetFrameSize())) {
      ImpLog(LogLevel::Warning, LogChannel::Audio,
             "DualShock 4 Bluetooth speaker write failed\n");
      closeDevice(true);
      continue;
    }
    frameNumber = static_cast<uint16_t>(frameNumber + SbcFramesPerReport);

    {
      std::lock_guard<std::mutex> postLock(StateMutex);
      if (activeQueueGeneration == QueueGeneration) {
        PlayedSamples32k += consumedSamples;
        if (finishedAfterReport) {
          PlaybackFinished = true;
        }
      }
    }
  }

  closeDevice(false);
  encoder.Shutdown();
}

void Init() {
  if (Initialized) return;
  Initialized = true;
  WasEnabled = false;
  NextRetryTicksMs = 0;
  DeviceReady.store(false, std::memory_order_release);
  GamepadListChanged.store(false, std::memory_order_release);
  HidError.store(false, std::memory_order_release);
  OutputGain.store(1.0f, std::memory_order_relaxed);

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    TargetHidPath.clear();
    DeviceGeneration = 0;
    PcmQueue.clear();
    QueueGeneration = 0;
    PlayedSamples32k = 0;
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

  SDL_AddEventWatch(GamepadEventCallback, nullptr);
  WorkerThread = std::thread(StreamWorkerLoop);
}

void Update(bool enabled) {
  if (!Initialized || !HidInitialized) return;

  if (!enabled) {
    GamepadListChanged.store(false, std::memory_order_release);
    HidError.store(false, std::memory_order_release);
    NextRetryTicksMs = 0;
    if (WasEnabled) {
      WasEnabled = false;
      ResetResampler();
      {
        std::lock_guard<std::mutex> lock(StateMutex);
        TargetHidPath.clear();
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

  if (GamepadListChanged.exchange(false, std::memory_order_acq_rel)) {
    rescan = true;
    NextRetryTicksMs = 0;
  }

  if (HidError.exchange(false, std::memory_order_acq_rel)) {
    NextRetryTicksMs = SDL_GetTicks() + HidRetryCooldownMs;
  }

  if (NextRetryTicksMs != 0 && SDL_GetTicks() >= NextRetryTicksMs) {
    NextRetryTicksMs = 0;
    rescan = true;
  }

  if (!rescan) return;

  std::string newPath = FindBluetoothDualShock4Path();
  {
    std::lock_guard<std::mutex> lock(StateMutex);
    if (newPath != TargetHidPath ||
        (!newPath.empty() && !DeviceReady.load(std::memory_order_acquire))) {
      TargetHidPath = std::move(newPath);
      ++DeviceGeneration;
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
    SDL_RemoveEventWatch(GamepadEventCallback, nullptr);
    {
      std::lock_guard<std::mutex> lock(StateMutex);
      ShuttingDown = true;
      TargetHidPath.clear();
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

  ResetResampler();
  ResampleBuffer.clear();
  DeviceReady.store(false, std::memory_order_release);
}

bool IsAvailable() { return DeviceReady.load(std::memory_order_acquire); }

void QueuePcm(void const* data, int sampleCount, int sampleRate,
              int channelCount, int bitDepth) {
  if (!data || sampleCount <= 0 || !IsAvailable()) return;
  if (!EnsureResampler(sampleRate, channelCount, bitDepth)) return;

  const int outCapacity = swr_get_out_samples(Resampler, sampleCount);
  if (outCapacity <= 0) return;

  if (ResampleBuffer.size() < static_cast<size_t>(outCapacity)) {
    ResampleBuffer.resize(static_cast<size_t>(outCapacity));
  }

  uint8_t* outPlanes[1] = {reinterpret_cast<uint8_t*>(ResampleBuffer.data())};
  uint8_t const* inPlanes[1] = {static_cast<uint8_t const*>(data)};
  const int converted =
      swr_convert(Resampler, outPlanes, outCapacity, inPlanes, sampleCount);
  if (converted <= 0) return;

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    PcmQueue.insert(PcmQueue.end(), ResampleBuffer.begin(),
                    ResampleBuffer.begin() + converted);
    QueueFinished = false;
    PlaybackFinished = false;
  }
  StateCv.notify_all();
}

void FinishQueue() {
  if (!Initialized) return;

  if (Resampler) {
    const int delayed = swr_get_out_samples(Resampler, 0);
    if (delayed > 0) {
      if (ResampleBuffer.size() < static_cast<size_t>(delayed)) {
        ResampleBuffer.resize(static_cast<size_t>(delayed));
      }
      uint8_t* outPlanes[1] = {
          reinterpret_cast<uint8_t*>(ResampleBuffer.data())};
      const int flushed =
          swr_convert(Resampler, outPlanes, delayed, nullptr, 0);
      if (flushed > 0) {
        std::lock_guard<std::mutex> lock(StateMutex);
        PcmQueue.insert(PcmQueue.end(), ResampleBuffer.begin(),
                        ResampleBuffer.begin() + flushed);
        PlaybackFinished = false;
      }
    }
    ResetResampler();
  }

  {
    std::lock_guard<std::mutex> lock(StateMutex);
    QueueFinished = true;
    if (!DeviceReady.load(std::memory_order_acquire)) {
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
    PlayedSamples32k = 0;
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
  return static_cast<int>((PcmQueue.size() * 1000ULL) /
                          static_cast<uint64_t>(SpeakerSampleRate));
}

uint64_t GetPlayedSamples32k() {
  std::lock_guard<std::mutex> lock(StateMutex);
  return PlayedSamples32k;
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

void QueuePcm(void const*, int, int, int, int) {}
void FinishQueue() {}
void StopPlayback() {}
void SetPaused(bool) {}
void SetGain(float) {}

int GetQueuedDurationMs() { return 0; }
uint64_t GetPlayedSamples32k() { return 0; }
bool IsFinished() { return true; }

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto

#endif
