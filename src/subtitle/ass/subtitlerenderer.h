#pragma once
#include <vector>
#include <ass/ass.h>
#include "../subtitlerenderer.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Subtitle {
namespace Ass {

class SubtitleRenderer;
struct AssDeleter {
  void operator()(ASS_Track* ptr) const {
    ass_free_track(ptr);
    ptr = nullptr;
  }
  void operator()(ASS_Renderer* ptr) const {
    ass_renderer_done(ptr);
    ptr = nullptr;
  }
  void operator()(ASS_Library* ptr) const {
    ass_library_done(ptr);
    ptr = nullptr;
  }
};

class SubtitleRenderTrack {
 public:
  enum class ChangeStatus : uint8_t {
    UNCHANGED = 0,
    POSITION = 1,
    CONTENT = 2
  };
  SubtitleRenderTrack(SubtitleRenderer& renderer, std::string_view header);
  SubtitleRenderTrack(SubtitleRenderer& renderer,
                      std::span<const char> fileBuffer);
  void Push(SubtitleEntry entry);
  void Render();
  SpriteSheet SubtitlesSpritesheet;
  std::vector<SubtitleEntry> Entries;
  SubtitleRenderer* SubRenderer;
  std::unique_ptr<ASS_Track, AssDeleter> AssTrack;
  size_t EntryCounter = 0;
  bool Enabled;
  ChangeStatus Change;
};

class SubtitleRenderer : public Subtitle::SubtitleRenderer {
 public:
  SubtitleRenderer(SubtitlePlayer& player, float width, float height);
  void AddTrack(int trackId, void* trackInfo) override;
  void AddTrackFile(int trackId, std::span<const char> fileBuffer) override;
  void AddSubtitleEntry(int trackId, SubtitleEntry entry) override;

  void Update(Video::Clock::Seconds time) override;
  void Render() override;
  void SetTrackVisibility(int trackId, bool enable) override;
  std::chrono::milliseconds GetTime() const { return Timer; }

  static void InitSystem();
  static ASS_Library* GetAssLibrary() { return AssLibrary.get(); }
  ASS_Renderer* GetAssRenderer() const { return AssRenderer.get(); }

 private:
  static std::unique_ptr<ASS_Library, AssDeleter> AssLibrary;
  std::unique_ptr<ASS_Renderer, AssDeleter> AssRenderer;
  ankerl::unordered_dense::map<int, SubtitleRenderTrack> Tracks;
  std::chrono::milliseconds Timer;
};

}  // namespace Ass
}  // namespace Subtitle
}  // namespace Impacto