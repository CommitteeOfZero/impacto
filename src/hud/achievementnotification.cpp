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
#include <string>
#include <vector>

namespace Impacto {
namespace AchievementNotification {

static Sprite BackgroundSprite;
static ExternalFont NotificationFont;
static std::vector<ExternalFontGlyph> TextGlyphs;
static Animation FadeAnimation;
static std::string BackgroundPath;
static std::string FontPath;
static float DisplayDuration;
static float FadeDuration;
static float IconSize;
static glm::vec2 IconOffset;
static float TextGap;
static float TextRightPadding;
static float TitleFontSize;
static float DescriptionFontSize;
static float TextLineGap;
static uint32_t TextColor;
static uint32_t OutlineColor;
static float DisplayTimer = 0.0f;
static int CurrentAchievementId = -1;
static bool IsConfigured = false;
static bool IsShowing = false;
static bool TextConfigured = false;
static std::queue<uint32_t> NotificationQueue;

static void FreeTextGlyphs() { ExternalFont::FreeGlyphTextures(TextGlyphs); }

static float GetEffectiveScale() {
  const float widthScale =
      Profile::DesignWidth / static_cast<float>(Profile::ResolutionWidth);
  const float heightScale =
      Profile::DesignHeight / static_cast<float>(Profile::ResolutionHeight);
  return std::min(widthScale, heightScale);
}

static void LoadConfig() {
  Profile::EnsurePushMemberOfType("AchievementNotification", LUA_TTABLE);

  BackgroundPath = Profile::EnsureGetMember<std::string>("BackgroundPath");
  FontPath = Profile::EnsureGetMember<std::string>("FontPath");
  DisplayDuration = Profile::EnsureGetMember<float>("DisplayDuration");
  FadeDuration = Profile::EnsureGetMember<float>("FadeDuration");
  IconSize = Profile::EnsureGetMember<float>("IconSize");
  IconOffset = Profile::EnsureGetMember<glm::vec2>("IconOffset");
  TextGap = Profile::EnsureGetMember<float>("TextGap");
  TextRightPadding = Profile::EnsureGetMember<float>("TextRightPadding");
  TitleFontSize = Profile::EnsureGetMember<float>("TitleFontSize");
  DescriptionFontSize = Profile::EnsureGetMember<float>("DescriptionFontSize");
  TextLineGap = Profile::EnsureGetMember<float>("TextLineGap");
  TextColor = Profile::EnsureGetMember<uint32_t>("TextColor");
  OutlineColor = Profile::EnsureGetMember<uint32_t>("OutlineColor");

  Profile::Pop();
}

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

  const glm::vec4 textTint = RgbIntToFloat(color);
  const glm::vec4 outlineTint = RgbIntToFloat(OutlineColor);

  NotificationFont.RenderShapedLine(glyphs, finalFontSize,
                                    {left + 1.0f, baselineY + 1.0f},
                                    outlineTint, TextGlyphs);
  NotificationFont.RenderShapedLine(glyphs, finalFontSize, {left, baselineY},
                                    textTint, TextGlyphs);
}

static void BuildTextGlyphs(AchievementSystem::Achievement const& achievement) {
  FreeTextGlyphs();
  if (!TextConfigured || !NotificationFont.IsLoaded()) return;

  const float scale = GetEffectiveScale();
  const float backgroundWidth = BackgroundSprite.ScaledWidth() * scale;
  const float backgroundHeight = BackgroundSprite.ScaledHeight() * scale;
  const float iconRight = (IconOffset.x + IconSize) * scale;
  const float textLeft = iconRight + TextGap * scale;
  const float textRight = backgroundWidth - TextRightPadding * scale;
  const float centerY = backgroundHeight / 2.0f;

  const float titleFontSize = TitleFontSize * scale;
  const float descriptionFontSize = DescriptionFontSize * scale;
  const float textLineGap = TextLineGap * scale;
  const float titleBaseline =
      centerY - (descriptionFontSize + textLineGap) / 2.0f;
  const float descriptionBaseline =
      centerY + (titleFontSize + textLineGap) / 2.0f;
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

  DisplayTimer = DisplayDuration;
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

  LoadConfig();

  FadeAnimation.DurationIn = FadeDuration;
  FadeAnimation.DurationOut = FadeDuration;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;

  if (!LoadBackground(BackgroundPath)) return;

  if (!FontPath.empty()) {
    TextConfigured =
        NotificationFont.Load(FontPath, "achievement notification font");
  } else {
    ImpLog(LogLevel::Warning, LogChannel::Profile,
           "Achievement notification font path is not configured\n");
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

  const float scale = GetEffectiveScale();
  const float backgroundWidth = BackgroundSprite.ScaledWidth() * scale;
  const float backgroundHeight = BackgroundSprite.ScaledHeight() * scale;
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
    const RectF iconDest(pos.x + IconOffset.x * scale,
                         pos.y + IconOffset.y * scale, IconSize * scale,
                         IconSize * scale);
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
