#include <fmt/format.h>
#include "subtitlesystem.h"

#ifndef IMPACTO_DISABLE_ASS
#include "ass/subtitlerenderer.h"
#endif

#include "../util.h"
#include "../profile/game.h"
#include "../profile/subtitle.h"
#include "../log.h"
#include "../io/physicalfilestream.h"

namespace Impacto::Subtitle {
using namespace Profile::Subtitle;

void SubtitleInit() {
  switch (Profile::SubtitleAssBackend) {
#ifndef IMPACTO_DISABLE_LIBASS
    case SubtitleAssBackendType::LibAss:
      Ass::SubtitleRenderer::InitSystem();
      break;
#endif
    case SubtitleAssBackendType::None:
      break;
    default:
      ImpLog(LogLevel::Warning, LogChannel::Subtitle,
             "Unknown or unsupported ass subtitle backend selected!.\n");
  }
  switch (Profile::SubtitleTextBackend) {
    case SubtitleTextBackendType::None:
      break;
    default:
      ImpLog(LogLevel::Warning, LogChannel::Subtitle,
             "Unknown or unsupported text subtitle backend selected!.\n");
  }
  switch (Profile::SubtitleBmpBackend) {
    case SubtitleBmpBackendType::None:
      break;
    default:
      ImpLog(LogLevel::Warning, LogChannel::Subtitle,
             "Unknown or unsupported bitmap subtitle backend selected!.\n");
  }
}

SubtitlePlayer::SubtitlePlayer(float width, float height) {
  switch (Profile::SubtitleAssBackend) {
#ifndef IMPACTO_DISABLE_LIBASS
    case SubtitleAssBackendType::LibAss:
      Backends[GetBackendIndex(SubtitleType::Ass)] =
          std::make_unique<Ass::SubtitleRenderer>(*this, width, height);
      break;
#endif
    case SubtitleAssBackendType::None:
      break;
  }
  switch (Profile::SubtitleTextBackend) {
    case SubtitleTextBackendType::None:
      break;
  }
  switch (Profile::SubtitleBmpBackend) {
    case SubtitleBmpBackendType::None:
      break;
  }
}

bool SubtitlePlayer::CanAddTrack(int trackId, SubtitleType type,
                                 Profile::SubtitleConfigType config) const {
  const auto& backend = Backends[GetBackendIndex(type)];
  if (!backend) {
    ImpLog(LogLevel::Warning, LogChannel::Subtitle,
           "Subtitle backend not initialized for {}", type);
    return false;
  }
  if ((+config & +Profile::SubtitleConfig) == 0) {
    ImpLog(LogLevel::Info, LogChannel::Subtitle,
           "Current subtitle mode is {}, skipping track {}, which is mode {}",
           Profile::SubtitleConfig, trackId, config);
    return false;
  }
  return true;
}

void SubtitlePlayer::AddTrackFile(int trackId, SubtitleType type,
                                  std::string const& path,
                                  Profile::SubtitleConfigType config) {
  if (!CanAddTrack(trackId, type, config)) return;
  auto& backend = Backends[GetBackendIndex(type)];
  Io::Stream* subtitleFileStream{};
  if (Io::PhysicalFileStream::Create(path, &subtitleFileStream) !=
      IoError::IoError_OK) {
    ImpLog(LogLevel::Warning, LogChannel::Subtitle,
           "Failed to read subtitle file {}", path);
    return;
  }
  std::vector<char> dataBuffer;

  dataBuffer.resize(subtitleFileStream->Meta.Size);
  subtitleFileStream->Read(dataBuffer.data(), dataBuffer.size());
  backend->AddTrackFile(trackId, dataBuffer);
  delete subtitleFileStream;
}

void SubtitlePlayer::PushEntry(int trackId, SubtitleEntry entry) {
  auto trackTypeItr = TrackTypeMap.find(trackId);
  if (trackTypeItr == TrackTypeMap.end()) {
    ImpLog(LogLevel::Warning, LogChannel::Subtitle,
           "Track {} not found in Subtitle Track Map", trackId);
    return;
  }
  auto type = trackTypeItr->second;
  const auto backendIndex = GetBackendIndex(type);
  auto& backend = Backends[backendIndex];
  if (!backend) {
    ImpLog(LogLevel::Warning, LogChannel::Subtitle,
           "Subtitle backend not initialized for {}", type);
    return;
  }
  backend->AddSubtitleEntry(trackId, std::move(entry));
}

void SubtitlePlayer::Update(Video::Clock::Microseconds elapsedTime) {
  for (auto& backend : Backends) {
    if (backend) {
      backend->Update(elapsedTime);
    };
  }
}

void SubtitlePlayer::Render() {
  for (auto& backend : Backends) {
    if (backend) {
      backend->Render();
    };
  }
}

int8_t SubtitlePlayer::GetBackendIndex(SubtitleType type) const {
  switch (type) {
    case SubtitleType::Ass:
      return 0;
    case SubtitleType::Bitmap:
      return 1;
    case SubtitleType::Text:
      return 2;
    case SubtitleType::None:
      break;
  }
  throw std::runtime_error(fmt::format("Invalid SubtitleType {}", type));
}

}  // namespace Impacto::Subtitle