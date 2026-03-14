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
    case +SubtitleAssBackendType::LibAss:
      Ass::SubtitleRenderer::InitSystem();
      break;
#endif
    case +SubtitleAssBackendType::None:
    default:
      ImpLog(LogLevel::Warning, LogChannel::Subtitle,
             "Unknown or unsupported ass subtitle backend selected!.\n");
  }
  switch (Profile::SubtitleTextBackend) {
    case +SubtitleTextBackendType::None:
      ImpLog(LogLevel::Warning, LogChannel::Subtitle,
             "Unknown or unsupported text subtitle backend selected!.\n");
      break;
  }
  switch (Profile::SubtitleBmpBackend) {
    case +SubtitleBmpBackendType::None:
      ImpLog(LogLevel::Warning, LogChannel::Subtitle,
             "Unknown or unsupported bitmap subtitle backend selected!.\n");
      break;
  }
}

SubtitlePlayer::SubtitlePlayer(float width, float height) {
  switch (Profile::SubtitleAssBackend) {
#ifndef IMPACTO_DISABLE_LIBASS
    case +SubtitleAssBackendType::LibAss:
      Backends[GetBackendIndex(SubtitleType::ASS)] =
          std::make_unique<Ass::SubtitleRenderer>(*this, width, height);
      break;
#endif
    case +SubtitleAssBackendType::None:
      break;
  }
  switch (Profile::SubtitleTextBackend) {
    case +SubtitleTextBackendType::None:
      break;
  }
  switch (Profile::SubtitleBmpBackend) {
    case +SubtitleBmpBackendType::None:
      break;
  }
}

void SubtitlePlayer::AddTrack(int trackId, SubtitleType type, void* trackInfo) {
  auto& backend = Backends[GetBackendIndex(type)];
  if (!backend) {
    ImpLog(LogLevel::Warning, LogChannel::Subtitle,
           "Subtitle backend not initialized for {}", type._to_string());
  }
  backend->AddTrack(trackId, trackInfo);
  TrackTypeMap.try_emplace(trackId, type);
}

void SubtitlePlayer::AddTrackFile(int trackId, SubtitleType type,
                                  std::string const& path) {
  const auto backendIndex = GetBackendIndex(type);
  auto& backend = Backends[backendIndex];
  if (!backend) {
    ImpLog(LogLevel::Warning, LogChannel::Subtitle,
           "Subtitle backend not initialized for {}", type._to_string());
    return;
  }

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
           "Subtitle backend not initialized for {}", type._to_string());
    return;
  }
  backend->AddSubtitleEntry(trackId, std::move(entry));
}

void SubtitlePlayer::Update(Video::Clock::Seconds frameTime) {
  for (auto& backend : Backends) {
    if (backend) {
      backend->Update(frameTime);
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

int8_t SubtitlePlayer::GetBackendIndex(SubtitleType type) {
  switch (type) {
    case +SubtitleType::ASS:
      return 0;
    case +SubtitleType::BITMAP:
      return 1;
    case +SubtitleType::TEXT:
      return 2;
    case +SubtitleType::NONE:
      break;
  }
  throw std::runtime_error(
      fmt::format("Invalid SubtitleType {}", type._to_integral()));
}

}  // namespace Impacto::Subtitle