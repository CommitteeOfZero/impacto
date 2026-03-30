#pragma once
#include <vector>
#include <ass/ass.h>
#include "../subtitlerenderer.h"
#include "../../spritesheet.h"
#include "../../profile/subtitle.h"

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
  using BackendType = SubtitleRenderer;
  SubtitleRenderTrack(SubtitleRenderer& renderer, std::string_view header);
  SubtitleRenderTrack(SubtitleRenderer& renderer,
                      std::span<const char> fileBuffer);
  void Push(SubtitleEntry entry);
  void Render();

  bool Visible = true;

 private:
  enum class ChangeStatus : uint8_t {
    UNCHANGED = 0,
    POSITION = 1,
    CONTENT = 2
  };

  struct GlyphKey {
    enum AssImageType : uint8_t {
      Character,
      Outline,
      Shadow,
    };
    GlyphKey(ASS_Image const& img);
    uint64_t BitmapHash;
    glm::ivec2 Dimensions;
    AssImageType Type;

    friend bool operator==(const GlyphKey& lhs, const GlyphKey& rhs) {
      return lhs.BitmapHash == rhs.BitmapHash &&
             lhs.Dimensions == rhs.Dimensions && lhs.Type == rhs.Type;
    }

    struct hash {
      size_t operator()(const GlyphKey& k) const {
        std::size_t seed = 0;
        HashCombine(seed, k.BitmapHash, k.Dimensions.x, k.Dimensions.y,
                    to_underlying(k.Type));
        return seed;
      }
    };
  };

  struct SubtitleGlyph {
    Sprite GlyphSprite;
    glm::vec4 Tint;
    glm::vec2 Position;
  };

  void UpdateSubtitleGlyphs(ASS_Image* images);

  std::vector<SubtitleEntry> Entries;
  std::vector<SubtitleGlyph> SubtitleGlyphs;
  ankerl::unordered_dense::map<GlyphKey, uint32_t, GlyphKey::hash>
      GlyphTextures;

  std::reference_wrapper<SubtitleRenderer> SubRenderer;
  std::unique_ptr<ASS_Track, AssDeleter> AssTrack;
  size_t EntryCounter = 0;
  ChangeStatus Change;
};

class SubtitleRenderer : public Subtitle::SubtitleRenderer {
 public:
  constexpr static Profile::Subtitle::SubtitleType Type =
      Profile::Subtitle::SubtitleType::Ass;
  SubtitleRenderer(SubtitlePlayer& player, float width, float height);
  void AddTrack(int trackId, SubtitleRenderTrack&& track);
  void AddTrackFile(int trackId, std::span<const char> fileBuffer) override;
  void AddSubtitleEntry(int trackId, SubtitleEntry entry) override;

  void Update(Video::Clock::Microseconds elapsedTime) override;
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