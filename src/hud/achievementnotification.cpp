#include "achievementnotification.h"

#include "../animation.h"
#include "../data/achievementsystem.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../profile/fonts.h"
#include "../profile/game.h"
#include "../profile/profile_internal.h"
#include "../renderer/renderer.h"
#include "../text/text.h"
#include "../texture/texture.h"

namespace Impacto {
namespace AchievementNotification {

static constexpr float DefaultDisplayDuration = 5.0f;
static constexpr float DefaultFadeDuration = 0.5f;
static constexpr float VisualScale = 0.75f;
static constexpr float IconSize = 64.0f;
static constexpr glm::vec2 IconOffset = {20.0f, 20.0f};
static constexpr float TextGap = 20.0f;
static constexpr float TextRightPadding = 20.0f;
static constexpr float TextFontSize = 22.0f;
static constexpr float TextLineGap = 3.0f;
static constexpr DialogueColorPair TextColor{0xffffff, 0x000000};

static Sprite BackgroundSprite;
static Font* NotificationFont = nullptr;
static Animation FadeAnimation;
static float DisplayTimer = 0.0f;
static int CurrentAchievementId = -1;
static bool IsConfigured = false;
static bool IsShowing = false;

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

static float GetLineWidth(std::vector<ProcessedTextGlyph> const& glyphs) {
  if (glyphs.empty()) return 0.0f;
  return glyphs.back().DestRect.Right() - glyphs.front().DestRect.X;
}

static void PositionTextLine(std::vector<ProcessedTextGlyph>& glyphs,
                             float left, float y, float scale) {
  if (glyphs.empty()) return;

  const float lineLeft = glyphs.front().DestRect.X;
  for (ProcessedTextGlyph& glyph : glyphs) {
    glyph.DestRect.X = left + (glyph.DestRect.X - lineLeft) * scale;
    glyph.DestRect.Y = y;
    glyph.DestRect.Width *= scale;
    glyph.DestRect.Height *= scale;
  }
}

static void DrawTextBlock(AchievementSystem::Achievement const& achievement,
                          float left, float right, float centerY,
                          float opacity) {
  if (NotificationFont == nullptr) return;

  const float fontSize = TextFontSize * VisualScale;
  const float lineGap = TextLineGap * VisualScale;
  std::vector<ProcessedTextGlyph> titleGlyphs =
      TextLayoutPlainString(achievement.Name(), NotificationFont, fontSize,
                            TextColor, 1.0f, {0.0f, 0.0f}, TextAlignment::Left);
  std::vector<ProcessedTextGlyph> descriptionGlyphs = TextLayoutPlainString(
      achievement.Description(), NotificationFont, fontSize, TextColor, 1.0f,
      {0.0f, 0.0f}, TextAlignment::Left);

  const float availableWidth = right - left;
  const float blockWidth =
      std::max(GetLineWidth(titleGlyphs), GetLineWidth(descriptionGlyphs));
  const float scale =
      blockWidth > 0.0f ? std::min(availableWidth / blockWidth, 1.0f) : 1.0f;
  const float blockLeft = left + (availableWidth - blockWidth * scale) / 2.0f;
  const float scaledFontSize = fontSize * scale;
  const float scaledLineGap = lineGap * scale;
  const float blockHeight = scaledFontSize * 2.0f + scaledLineGap;
  const float top = centerY - blockHeight / 2.0f;

  PositionTextLine(titleGlyphs, blockLeft, top, scale);
  PositionTextLine(descriptionGlyphs, blockLeft,
                   top + scaledFontSize + scaledLineGap, scale);

  Renderer->DrawProcessedText(titleGlyphs, NotificationFont, opacity,
                              RendererOutlineMode::BottomRight);
  Renderer->DrawProcessedText(descriptionGlyphs, NotificationFont, opacity,
                              RendererOutlineMode::BottomRight);
}

void Init() {
  IsConfigured = false;
  CurrentAchievementId = -1;
  DisplayTimer = 0.0f;
  IsShowing = false;
  NotificationFont = nullptr;

  FadeAnimation.DurationIn = DefaultFadeDuration;
  FadeAnimation.DurationOut = DefaultFadeDuration;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;

  std::string path;
  if (!Profile::TryGetMember<std::string>("AchievementNotificationPath",
                                          path)) {
    return;
  }

  const auto fontIt = Profile::Fonts.find("AchievementNotificationFont");
  if (fontIt != Profile::Fonts.end() && fontIt->second->Type == FontType::LB) {
    NotificationFont = fontIt->second;
  } else {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "AchievementNotificationFont LB font is not configured\n");
  }

  IsConfigured = LoadBackground(path);
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
  if (icon.ScaledWidth() <= 0.0f || icon.ScaledHeight() <= 0.0f) return;

  const RectF iconDest(pos.x + IconOffset.x * VisualScale,
                       pos.y + IconOffset.y * VisualScale,
                       IconSize * VisualScale, IconSize * VisualScale);
  Renderer->DrawSprite(icon, iconDest, tint);

  const float textLeft = iconDest.Right() + TextGap * VisualScale;
  const float textRight =
      pos.x + backgroundWidth - TextRightPadding * VisualScale;
  const float textCenterY = pos.y + backgroundHeight / 2.0f;

  DrawTextBlock(*achievement, textLeft, textRight, textCenterY, tint.a);
}

void Show(int achievementId) {
  if (!IsConfigured) return;

  CurrentAchievementId = achievementId;
  DisplayTimer = DefaultDisplayDuration;
  IsShowing = true;
  FadeAnimation.StartIn();
}

}  // namespace AchievementNotification
}  // namespace Impacto
