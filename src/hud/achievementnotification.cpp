#include "achievementnotification.h"

#include "../animation.h"
#include "../data/achievementsystem.h"
#include "../text/fonts/font.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../profile/game.h"
#include "../profile/hud/achievementnotification.h"
#include "../userconfig.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"

#include <algorithm>
#include <cassert>
#include <optional>
#include <queue>
#include <string>
#include <vector>

namespace Impacto {
namespace AchievementNotification {

static Sprite BackgroundSprite;
static std::optional<ExternalFont> NotificationFont;
static std::vector<ProcessedTextGlyph> TextGlyphs;
static Animation FadeAnimation;
static float DisplayTimer = 0.0f;
static std::optional<int> CurrentAchievementId;
static bool IsConfigured = false;
static bool TextConfigured = false;
static std::queue<int> NotificationQueue;

using namespace Profile::AchievementNotification;

static float GetEffectiveScale() {
  auto const& config = UserConfig::CommonSettings;
  const float widthScale =
      Profile::Game::DesignWidth / static_cast<float>(config.ResolutionWidth);
  const float heightScale =
      Profile::Game::DesignHeight / static_cast<float>(config.ResolutionHeight);
  return std::min(widthScale, heightScale);
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
      NotificationFont->ShapeLine(text, fontSize, width);
  if (glyphs.empty()) return;

  float finalFontSize = fontSize;
  if (width > availableWidth && width > 0.0f) {
    finalFontSize *= availableWidth / width;
    glyphs = NotificationFont->ShapeLine(text, finalFontSize, width);
  }

  const DialogueColorPair colors{color, OutlineColor};

  glm::vec2 pen{left, baselineY};
  for (ExternalFontShapedGlyph const& glyph : glyphs) {
    const glm::vec2 glyphPos = pen + glyph.Offset;

    ProcessedTextGlyph ptg;
    ptg.CharId = glyph.GlyphIndex;
    ptg.Colors = colors;
    ptg.Opacity = 1.0f;
    ptg.DestRect =
        RectF(glyphPos.x, glyphPos.y, glyph.Advance.x, finalFontSize);
    TextGlyphs.push_back(ptg);

    pen += glyph.Advance;
  }
}

static void BuildTextGlyphs(AchievementSystem::Achievement const* achievement,
                            int achievementId) {
  TextGlyphs.clear();
  assert(TextConfigured);

  std::string title = achievement != nullptr ? achievement->Name() : "";
  std::string description =
      achievement != nullptr ? achievement->Description() : "";
  if (title.empty() && description.empty()) {
    title = "Achievement " + std::to_string(achievementId);
    description = "Achievement System not implemented.";
  }

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

  BuildTextLine(title, titleFontSize, textLeft, titleBaseline, availableWidth,
                TextColor);
  BuildTextLine(description, descriptionFontSize, textLeft, descriptionBaseline,
                availableWidth, TextColor);
}

static void StartNextNotification() {
  if (NotificationQueue.empty()) return;

  CurrentAchievementId = NotificationQueue.front();
  NotificationQueue.pop();

  const AchievementSystem::Achievement* achievement =
      AchievementSystem::GetAchievement(*CurrentAchievementId);
  BuildTextGlyphs(achievement, *CurrentAchievementId);

  DisplayTimer = DisplayDuration;
  IsShowing = true;
  FadeAnimation.StartIn(true);
}

void Init() {
  TextGlyphs.clear();
  NotificationFont.reset();
  TextConfigured = false;

  Configure();

  FadeAnimation.DurationIn = FadeDuration;
  FadeAnimation.DurationOut = FadeDuration;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;

  if (!LoadBackground(BackgroundPath)) return;

  if (!FontPath.empty()) {
    NotificationFont.emplace(FontPath, "achievement notification font");
    TextConfigured = NotificationFont->IsLoaded();
  } else {
    ImpLog(LogLevel::Warning, LogChannel::Profile,
           "Achievement notification font path is not configured\n");
  }

  IsConfigured = true;
}

void Update(float dt) {
  assert(IsConfigured);

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
  assert(IsConfigured);
  if (FadeAnimation.IsOut()) return;

  glm::vec4 tint(1.0f);
  tint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  const float scale = GetEffectiveScale();
  const float backgroundWidth = BackgroundSprite.ScaledWidth() * scale;
  const float backgroundHeight = BackgroundSprite.ScaledHeight() * scale;
  const glm::vec2 pos = {Profile::Game::DesignWidth - backgroundWidth,
                         Profile::Game::DesignHeight - backgroundHeight};

  Renderer->DrawSprite(BackgroundSprite,
                       RectF(pos.x, pos.y, backgroundWidth, backgroundHeight),
                       tint);

  const AchievementSystem::Achievement* achievement =
      CurrentAchievementId.has_value()
          ? AchievementSystem::GetAchievement(*CurrentAchievementId)
          : nullptr;
  if (achievement != nullptr && achievement->Icon().ScaledWidth() > 0.0f &&
      achievement->Icon().ScaledHeight() > 0.0f) {
    const Sprite& icon = achievement->Icon();
    const RectF iconDest(pos.x + IconOffset.x * scale,
                         pos.y + IconOffset.y * scale, IconSize * scale,
                         IconSize * scale);
    Renderer->DrawSprite(icon, iconDest, tint);
  }

  if (!TextGlyphs.empty()) {
    NotificationFont->DrawProcessedText(TextGlyphs, tint.a, tint.a,
                                        RendererOutlineMode::BottomRight, pos);
  }
}

void Show(int achievementId) {
  assert(IsConfigured);

  NotificationQueue.push(achievementId);
  if (!IsShowing && FadeAnimation.IsOut()) {
    StartNextNotification();
  }
}

}  // namespace AchievementNotification
}  // namespace Impacto
