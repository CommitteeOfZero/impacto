#include "subtitlerenderer.h"

#include <ass/ass.h>
#include "../../log.h"
#include "../../renderer/renderer.h"
#include "../../profile/subtitle.h"

namespace Impacto {
namespace Subtitle {
namespace Ass {
std::unique_ptr<ASS_Library, AssDeleter> SubtitleRenderer::AssLibrary;

using namespace Profile::Subtitle;

void AssMsgHandler(int level, const char* fmt, va_list va, void* data) {
  LogLevel impLevel = [&level] {
    switch (level) {
      case 0:
        return LogLevel::Fatal;
      case 1:
      case 2:
      case 3:
        return LogLevel::Error;
      case 4:
        return LogLevel::Warning;
      case 5:
        return LogLevel::Info;
      case 6:
        return LogLevel::Debug;
      case 7:
        return LogLevel::Trace;
    }
    return LogLevel::Off;
  }();
  std::array<char, 512> buffer{};
  int result = vsnprintf(buffer.data(), buffer.size(), fmt, va);
  size_t actualSize = std::min(static_cast<size_t>(result), buffer.size() - 1);
  ImpLog(impLevel, LogChannel::Subtitle, "LibAss: {}\n",
         std::string_view{buffer.data(), actualSize});
}

SubtitleRenderTrack::SubtitleRenderTrack(SubtitleRenderer& renderer,
                                         std::string_view header)
    : SubtitlesSpritesheet(renderer.GetWidth(), renderer.GetHeight()),
      SubRenderer(&renderer),
      AssTrack(ass_new_track(SubtitleRenderer::GetAssLibrary())) {
  if (!AssTrack) {
    ImpLog(LogLevel::Error, LogChannel::Subtitle,
           "Failed to initialize ass track\n");
    throw std::runtime_error("Failed to initialize ass track\n");
  }
  ass_process_data(AssTrack.get(), header.data(),
                   static_cast<int>(header.size()));
}

SubtitleRenderTrack::SubtitleRenderTrack(SubtitleRenderer& renderer,
                                         std::span<const char> fileBuffer)
    : SubtitlesSpritesheet(renderer.GetWidth(), renderer.GetHeight()),
      SubRenderer(&renderer),
      AssTrack(ass_read_memory(SubtitleRenderer::GetAssLibrary(),
                               const_cast<char*>(fileBuffer.data()),
                               fileBuffer.size(), "UTF-8")) {
  if (!AssTrack) {
    ImpLog(LogLevel::Error, LogChannel::Subtitle,
           "Failed to initialize ass track\n");
    throw std::runtime_error("Failed to initialize ass track\n");
  }
}

void SubtitleRenderTrack::Push(SubtitleEntry entry) {
  auto&& pushedEntry = Entries.emplace_back(std::move(entry));
  EntryCounter++;
  auto const& assStr = std::get<std::string>(pushedEntry.Data);
  ass_process_chunk(AssTrack.get(), assStr.c_str(),
                    static_cast<int>(assStr.size()), entry.StartMs.count(),
                    entry.Duration.count());
}

void SubtitleRenderTrack::Render() {
  int wasChanged = 0;
  ASS_Image* images =
      ass_render_frame(SubRenderer->GetAssRenderer(), AssTrack.get(),
                       SubRenderer->GetTime().count(), &wasChanged);
  Change = static_cast<ChangeStatus>(wasChanged);
  if (SubtitlesSpritesheet.Texture &&
      (Change != ChangeStatus::UNCHANGED || !images)) {
    Renderer->FreeTexture(SubtitlesSpritesheet.Texture);
    SubtitlesSpritesheet.Texture = 0;
  }
  if (Change != ChangeStatus::UNCHANGED && images) {
    Texture t;
    t.Init(TexFmt_RGBA, static_cast<int>(SubRenderer->GetWidth()),
           static_cast<int>(SubRenderer->GetHeight()));
    auto drawGlyph = [&t](ASS_Image& img) {
      if (img.w == 0 || img.h == 0) return;
      const uint8_t r = (img.color >> 24) & 0xFF;
      const uint8_t g = (img.color >> 16) & 0xFF;
      const uint8_t b = (img.color >> 8) & 0xFF;
      const uint8_t a = 255 - (img.color & 0xFF);
      for (int y = 0; y < img.h; ++y) {
        for (int x = 0; x < img.w; ++x) {
          const uint8_t k = img.bitmap[y * img.stride + x];
          const int dstX = img.dst_x + x;
          const int dstY = img.dst_y + y;
          if (!k || dstX < 0 || dstX > t.Width || dstY < 0 || dstY > t.Height)
            continue;

          const size_t idx = (dstY * t.Width + dstX) * 4;

          constexpr uint32_t mult = 255 * 255;
          const uint32_t rr = (r * a * k);
          const uint32_t gg = (g * a * k);
          const uint32_t bb = (b * a * k);
          const uint32_t aa = a * k;

          uint8_t& dstR = t.Buffer[idx + 0];
          uint8_t& dstG = t.Buffer[idx + 1];
          uint8_t& dstB = t.Buffer[idx + 2];
          uint8_t& dstA = t.Buffer[idx + 3];

          dstR = static_cast<uint8_t>((rr + dstR * (mult - aa)) / mult);
          dstG = static_cast<uint8_t>((gg + dstG * (mult - aa)) / mult);
          dstB = static_cast<uint8_t>((bb + dstB * (mult - aa)) / mult);
          dstA = static_cast<uint8_t>((aa * 255 + dstA * (mult - aa)) / (mult));
        }
      }
    };
    for (ASS_Image* img = images; img; img = img->next) {
      drawGlyph(*img);
    }
    SubtitlesSpritesheet.Texture = t.Submit();
  }
  if (SubtitlesSpritesheet.Texture) {
    Renderer->DrawSprite(
        Sprite(SubtitlesSpritesheet, 0, 0, SubtitlesSpritesheet.DesignWidth,
               SubtitlesSpritesheet.DesignHeight),
        {0, 0});
  }
}

void SubtitleRenderer::InitSystem() {
  if (!AssLibrary) {
    AssLibrary.reset(ass_library_init());
    ass_set_message_cb(AssLibrary.get(), AssMsgHandler, nullptr);
    ass_set_extract_fonts(AssLibrary.get(), 1);

    if (SubtitleFontsDir) {
      ass_set_fonts_dir(AssLibrary.get(), SubtitleFontsDir->c_str());
    }
  }
  if (!AssLibrary) {
    ImpLog(LogLevel::Error, LogChannel::Subtitle,
           "Failed to initialize ass renderer\n");
    throw std::runtime_error("Failed to initialize libass\n");
  }
}

SubtitleRenderer::SubtitleRenderer(SubtitlePlayer& player, float width,
                                   float height)
    : Subtitle::SubtitleRenderer(player, width, height),
      AssRenderer(ass_renderer_init(AssLibrary.get())) {
  if (!AssRenderer) {
    ImpLog(LogLevel::Error, LogChannel::Subtitle,
           "Failed to initialize ass renderer\n");
    throw std::runtime_error("Failed to initialize ass renderer\n");
  }
  ass_set_storage_size(AssRenderer.get(), static_cast<int>(Width),
                       static_cast<int>(Height));

  ass_set_frame_size(AssRenderer.get(), static_cast<int>(Width),
                     static_cast<int>(Height));
  ass_set_fonts(AssRenderer.get(), NULL, "sans-serif",
                ASS_FONTPROVIDER_AUTODETECT, NULL, 1);
}

void SubtitleRenderer::AddTrack(int trackId, void* trackInfo) {
  auto [ref, inserted] = Tracks.try_emplace(
      trackId, *this, *(static_cast<std::string_view*>(trackInfo)));
  if (!inserted) {
    ImpLog(LogLevel::Error, LogChannel::Subtitle,
           "Attempted to insert to existing Track ID {:d}", trackId);
  }
}

void SubtitleRenderer::AddTrackFile(int trackId,
                                    std::span<const char> fileBuffer) {
  auto [ref, inserted] = Tracks.try_emplace(trackId, *this, fileBuffer);
  if (!inserted) {
    ImpLog(LogLevel::Error, LogChannel::Subtitle,
           "Attempted to insert to existing Track ID {:d}", trackId);
  }
}

void SubtitleRenderer::SetTrackVisibility(int trackId, bool enable) {}

void SubtitleRenderer::AddSubtitleEntry(int trackId, SubtitleEntry entry) {
  if (auto trackItr = Tracks.find(trackId); trackItr != Tracks.end()) {
    trackItr->second.Push(std::move(entry));
  } else {
    ImpLog(LogLevel::Error, LogChannel::Subtitle, "Track ID {:d} not found",
           trackId);
  }
}

void SubtitleRenderer::Update(Video::Clock::Seconds time) {
  ImpLog(LogLevel::Trace, LogChannel::Subtitle, "Updating subtitle time: {}",
         time);
  Timer = std::chrono::duration_cast<std::chrono::milliseconds>(time);
}

void SubtitleRenderer::Render() {
  for (auto& [id, track] : Tracks) {
    if (!track.Enabled) continue;
    track.Render();
  }
}

};  // namespace Ass
};  // namespace Subtitle
};  // namespace Impacto
