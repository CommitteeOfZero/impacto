#include "achievementnotification.h"

#include "../animation.h"
#include "../data/achievementsystem.h"
#include "../font.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../profile/game.h"
#include "../profile/profile_internal.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"

#include <algorithm>
#include <queue>
#include <vector>

namespace Impacto {
namespace AchievementNotification {

static constexpr float DefaultDisplayDuration = 5.0f;
static constexpr float DefaultFadeDuration = 0.5f;
static constexpr float VisualScale = 0.75f;
static constexpr float IconSize = 64.0f;
static constexpr glm::vec2 IconOffset = {20.0f, 20.0f};
static constexpr float TextGap = 20.0f;
static constexpr float TextRightPadding = 20.0f;
static constexpr float TitleFontSize = 24.0f;
static constexpr float DescriptionFontSize = 18.0f;
static constexpr float TextLineGap = 6.0f;
static constexpr uint32_t TextColor = 0xFFFFFF;
static constexpr uint32_t OutlineColor = 0x000000;

static Sprite BackgroundSprite;
static ExternalFont NotificationFont;
static std::vector<ExternalFontGlyph> TextGlyphs;
static Animation FadeAnimation;
static float DisplayTimer = 0.0f;
static int CurrentAchievementId = -1;
static bool IsConfigured = false;
static bool IsShowing = false;
static bool TextConfigured = false;
static std::queue<uint32_t> NotificationQueue;

static void FreeTextGlyphs() { ExternalFont::FreeGlyphTextures(TextGlyphs); }

static bool LoadBackground(std::string const& path) {
  Io::Stream* stream = nullptr;
  IoError err = Io::PhysicalFileStream::Create(path, &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not open achievement notification background {:s}\n", path);
    return false;
  }

  Texture texture;
  if (!texture.Load(stream)) {
    ImpLog(LogLevel::Error, LogChannel::TextureLoad,
           "Could not load achievement notification background {:s}\n", path);
    delete stream;
    return false;
  }
  delete stream;

  SpriteSheet sheet(static_cast<float>(texture.Width),
                    static_cast<float>(texture.Height));
  sheet.Texture = texture.Submit();
  BackgroundSprite =
      Sprite(sheet, 0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
  return true;
}

static void BuildTextLine(std::string const& text, float fontSize, float left,
                          float baselineY, float availableWidth,
                          uint32_t color) {
  float width = 0.0f;
  std::vector<ExternalFontShapedGlyph> glyphs =
      NotificationFont.ShapeLine(text, fontSize, width);
  if (glyphs.empty()) return;

  float finalFontSize = fontSize;
  if (width > availableWidth && width > 0.0f) {
    finalFontSize *= availableWidth / width;
    glyphs = NotificationFont.ShapeLine(text, finalFontSize, width);
  }

  const float lineLeft = left + std::max(0.0f, (availableWidth - width) / 2.0f);
  const glm::vec4 textTint = RgbIntToFloat(color);
  const glm::vec4 outlineTint = RgbIntToFloat(OutlineColor);

  NotificationFont.RenderShapedLine(glyphs, finalFontSize,
                                    {lineLeft + 1.0f, baselineY + 1.0f},
                                    outlineTint, TextGlyphs);
  NotificationFont.RenderShapedLine(
      glyphs, finalFontSize, {lineLeft, baselineY}, textTint, TextGlyphs);
}

static void BuildTextGlyphs(AchievementSystem::Achievement const& achievement) {
  FreeTextGlyphs();
  if (!TextConfigured || !NotificationFont.IsLoaded()) return;

  const float backgroundWidth = BackgroundSprite.ScaledWidth() * VisualScale;
  const float backgroundHeight = BackgroundSprite.ScaledHeight() * VisualScale;
  const float iconRight = (IconOffset.x + IconSize) * VisualScale;
  const float textLeft = iconRight + TextGap * VisualScale;
  const float textRight = backgroundWidth - TextRightPadding * VisualScale;
  const float centerY = backgroundHeight / 2.0f;

  const float titleFontSize = TitleFontSize * VisualScale;
  const float descriptionFontSize = DescriptionFontSize * VisualScale;
  const float titleBaseline =
      centerY - (descriptionFontSize + TextLineGap) / 2.0f;
  const float descriptionBaseline =
      centerY + (titleFontSize + TextLineGap) / 2.0f;
  const float availableWidth = textRight - textLeft;

  BuildTextLine(achievement.Name(), titleFontSize, textLeft, titleBaseline,
                availableWidth, TextColor);
  BuildTextLine(achievement.Description(), descriptionFontSize, textLeft,
                descriptionBaseline, availableWidth, TextColor);
}

static void StartNextNotification() {
  if (NotificationQueue.empty()) return;

  CurrentAchievementId = static_cast<int>(NotificationQueue.front());
  NotificationQueue.pop();

  const AchievementSystem::Achievement* achievement =
      AchievementSystem::GetAchievement(CurrentAchievementId);
  if (achievement != nullptr) {
    BuildTextGlyphs(*achievement);
  } else {
    FreeTextGlyphs();
  }

  DisplayTimer = DefaultDisplayDuration;
  IsShowing = true;
  FadeAnimation.StartIn(true);
}

void Init() {
  IsConfigured = false;
  DisplayTimer = 0.0f;
  CurrentAchievementId = -1;
  IsShowing = false;
  TextConfigured = false;
  NotificationQueue = {};
  FreeTextGlyphs();
  NotificationFont.Reset();

  FadeAnimation.DurationIn = DefaultFadeDuration;
  FadeAnimation.DurationOut = DefaultFadeDuration;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;

  std::string path;
  if (!Profile::TryGetMember<std::string>("AchievementNotificationPath",
                                          path)) {
    return;
  }

  if (!LoadBackground(path)) return;

  std::string fontPath;
  if (Profile::TryGetMember<std::string>("AchievementNotificationFontPath",
                                         fontPath)) {
    TextConfigured =
        NotificationFont.Load(fontPath, "achievement notification font");
  } else {
    ImpLog(LogLevel::Warning, LogChannel::Profile,
           "AchievementNotificationFontPath is not configured\n");
  }

  IsConfigured = true;
}

void Update(float dt) {
  if (!IsConfigured) return;

  FadeAnimation.Update(dt);

  if (IsShowing && FadeAnimation.IsIn()) {
    DisplayTimer -= dt;
    if (DisplayTimer <= 0.0f) {
      IsShowing = false;
      FadeAnimation.StartOut();
    }
  }

  if (!IsShowing && FadeAnimation.IsOut()) {
    StartNextNotification();
  }
}

void Render() {
  if (!IsConfigured || FadeAnimation.IsOut()) return;

  glm::vec4 tint(1.0f);
  tint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  const float backgroundWidth = BackgroundSprite.ScaledWidth() * VisualScale;
  const float backgroundHeight = BackgroundSprite.ScaledHeight() * VisualScale;
  const glm::vec2 pos = {Profile::DesignWidth - backgroundWidth,
                         Profile::DesignHeight - backgroundHeight};

  Renderer->DrawSprite(BackgroundSprite,
                       RectF(pos.x, pos.y, backgroundWidth, backgroundHeight),
                       tint);

  const AchievementSystem::Achievement* achievement =
      AchievementSystem::GetAchievement(CurrentAchievementId);
  if (achievement == nullptr) return;

  const Sprite& icon = achievement->Icon();
  if (icon.ScaledWidth() > 0.0f && icon.ScaledHeight() > 0.0f) {
    const RectF iconDest(pos.x + IconOffset.x * VisualScale,
                         pos.y + IconOffset.y * VisualScale,
                         IconSize * VisualScale, IconSize * VisualScale);
    Renderer->DrawSprite(icon, iconDest, tint);
  }

  if (!TextGlyphs.empty()) {
    Renderer->SetBlendMode(RendererBlendMode::Premultiplied);
    for (ExternalFontGlyph const& glyph : TextGlyphs) {
      glm::vec4 glyphTint = glyph.Tint;
      glyphTint.a *= tint.a;
      Renderer->DrawSubtitleGlyph(glyph.GlyphSprite, pos + glyph.Position,
                                  glyphTint);
    }
    Renderer->SetBlendMode(RendererBlendMode::Normal);
  }
}

void Show(int achievementId) {
  if (!IsConfigured) return;

  NotificationQueue.push(achievementId);
  if (!IsShowing && FadeAnimation.IsOut()) {
    StartNextNotification();
  }
}

}  // namespace AchievementNotification
}  // namespace Impacto
